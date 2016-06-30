#include "cliquemodule.h"
#include "utils.h"
#include "converter.h"
#include <cassert>
#include <QDebug>

extern Converter convert;

CliqueModule::CliqueModule(const QString &name) : m_Name(name)
{

}

CliqueModule::CliqueModule(const CliqueModule &other)
{
    operator =(other);
}

CliqueModule &CliqueModule::operator = (const CliqueModule &other)
{
    this->ownership = true;
    this->_isTarget = false;
    this->_isIdentity = false;

    QHash<CliqueNetwork*,CliqueNetwork*> assoc;

    for (CliqueNetwork *nw : other.nws) {
        assoc[nw] = new CliqueNetwork(*nw);
    }

    for (auto x: other.inputs) {
        inputs.push_back(assoc[x]);
    }
    for (auto x: other.outputs) {
        outputs.push_back(assoc[x]);
    }
    for (auto x: other.nws) {
        nws.insert(assoc[x]);
    }

    return *this;
}

CliqueModule::~CliqueModule() {
    if (ownership) {
        qDeleteAll(nws);
        nws.clear();
    }
}

void CliqueModule::addInputNetwork(CliqueNetwork *nw)
{
    addNetwork(nw);
    inputs.push_back(nw);
}

void CliqueModule::addOutputNetwork(CliqueNetwork *nw)
{
    addNetwork(nw);
    outputs.push_back(nw);
}

CliqueNetwork *CliqueModule::getInputNetwork(int i) const
{
    return inputs[i];
}

CliqueNetwork *CliqueModule::getOutputNetwork(int i) const
{
    return outputs[i];
}

void CliqueModule::setOwnership(bool ownership)
{
    this->ownership = ownership;
}

Clique CliqueModule::getOutput(const Clique &input)
{
    if (_isIdentity) {
        return input;
    }

    /* No safety checks done. Assumes reponsible caller */
    auto cl = CliqueNetworkManager::getOutput(inputs.first(), outputs.first(), input);

    if (destinationModules.contains(cl)) {
        /* Possible infinite recursion */
        cl = destinationModules[cl]->getOutput(input);
    }

    return cl;
}

QList<Clique> CliqueModule::getOutputs(const QList<Clique> &inputs)
{
    if (_isIdentity) {
        return inputs;
    }

    QList<Clique> ret;

    shutdown();

    for (int i = 0; i < inputs.size(); i++) {
        if (this->inputs.size() > i && !this->inputs[i]->isEmpty()) {
            this->inputs[i]->activateClique(inputs[i]);
        } else {
            qDebug() << "Ignore input " << i;
        }
    }
    iterate();
    for (auto output : outputs) {
        ret.push_back(output->activatedClique());
    }

    return ret;
}

QList<cl::Transformation> CliqueModule::getCombinationInputs(const QList<Clique> &inputs, const QList<Clique> &outputs, int firstOutput, const std::deque<int> &remainingOutputs)
{
    QList<cl::Transformation> ret;

    //qDebug() << "Process: " << toInt(convert.words(inputs)) << toInt(convert.words(outputs));

    /* k correponds to the number of additional outputs */
    for (int k = 0; k < remainingOutputs.size() + 1 && k < noutputs(); k++) {
        auto combs = comb(remainingOutputs, k);

        if (k == 0) {
            std::deque<int> simple;
            simple.push_front(firstOutput);
            combs.push_back(simple);
        } else {
            for (auto &el : combs) {
                el.push_front(firstOutput);
            }
        }

        std::deque<int> possibleInputs;
        for (int i = 0; i < inputs.size(); i++) {
            possibleInputs.push_back(i);
        }
        auto combsIn = comb(possibleInputs, ninputs());

        for (auto elIn : combsIn) {
            do {
                QList<Clique> testIn;
                for (int i : elIn) testIn.push_back(inputs[i]);

                QList<Clique> res = getOutputs(testIn);

                //qDebug() << "Inputs: " << toInt(convert.words(testIn)) << " outputs " << toInt(convert.words(res));

                for (auto elOut : combs) {
                    do {
                        QList<Clique> shouldOut;

                        for (int i = 0; i < elOut.size(); i++) {
                            shouldOut.push_back(outputs[elOut[i]]);
                        }

                        if (shouldOut == res) {
                            //qDebug() << toInt(convert.words(shouldOut)) << "matching";
                            cl::Transformation tr;
                            tr.module = this;
                            for (int i : elIn) tr.inputs << i;
                            for (int i : elOut) tr.outputs << i;

                            ret.push_back(tr);
                        } else {
                            //qDebug() << toInt(convert.words(shouldOut)) << "not matching";
                        }
                    } while (std::next_permutation(elOut.begin(), elOut.end()));
                }
            } while (std::next_permutation(elIn.begin(), elIn.end()));
        }
    }

    return ret;
}

QList<Clique> CliqueModule::analyzeInput(const Clique &output)
{
    QList<Clique> ret;
    CliqueNetwork *src = inputs.first();
    CliqueNetwork *dest = outputs.first();

    for (const Clique &cl : src->allCliques()) {
        if (src->isFullyConnectedTo(cl, dest, output)) {
            ret.push_back(cl);
        }
    }

    return ret;
}

QList<Clique> CliqueModule::analyzeOutput(const Clique &input)
{
    QList<Clique> ret;
    CliqueNetwork *src = inputs.first();
    CliqueNetwork *dest = outputs.first();

    for (const Clique &cl : dest->allCliques()) {
        if (src->isFullyConnectedTo(input, dest, cl)) {
            ret.push_back(cl);
        }
    }

    return ret;
}

void CliqueModule::addModule(CliqueModule *module, QList<int> ins, QList<int> outs)
{
    for (CliqueNetwork *nw : module->nws) {
        addNetwork(nw);
    }

    for (int i = 0; i < ins.size(); i++) {
        getInputNetwork(ins[i])->buildIdentity(module->getInputNetwork(i));
    }
    for (int i = 0; i < outs.size(); i++) {
        module->getOutputNetwork(i)->buildIdentity(getOutputNetwork(outs[i]));
    }
}

Clique CliqueModule::addDestinationModule(CliqueModule *module)
{
    auto output = outputs.first();
    auto cl = output->randomClique();

    output->addClique(cl);
    destinationModules[cl] = module;

    return cl;
}

void CliqueModule::addDestinationModule(CliqueModule *module, const Clique &cl)
{
    outputs.first()->addClique(cl);
    destinationModules[cl] = module;
}

void CliqueModule::clearOutputCliques()
{
    for (auto nw : outputs) {
        nw->clear();
    }
}

void CliqueModule::linkInputOutput(const Clique &input, const Clique &output)
{
    inputs.first()->linkClique(input, outputs.first(), output);
}

void CliqueModule::buildIdentity()
{
    _isIdentity = true;
    assert(inputs.size() == 1 && outputs.size() == 1);

    for (const Clique &c : inputs.first()->allCliques()) {
        linkInputOutput(c, c);
    }
}

void CliqueModule::buildTarget(const Clique &target)
{
    assert(inputs.size() == 1 && outputs.size() == 1);

    for (const Clique &c : inputs.first()->allCliques()) {
        linkInputOutput(c, target);
    }

    _isTarget = true;
}
