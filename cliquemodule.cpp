#include "cliquemodule.h"
#include <cassert>
#include <QDebug>

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
