#include "cliquemodule.h"
#include "utils.h"
#include "converter.h"
#include <cassert>
#include <QDebug>

extern Converter convert;

CliqueModule::CliqueModule(const QString &name) : m_Name(name)
{

}


CliqueModule::~CliqueModule() {
}

void CliqueModule::addInputNetwork()
{
    inputs.push_back(Clique());
}

void CliqueModule::addOutputNetwork()
{
    outputs.push_back(Clique());
}

Clique CliqueModule::getOutput(const Clique &input)
{
    if (_isIdentity) {
        return input;
    }
    if (_isTarget) {
        return target;
    }

    /* No safety checks done. Assumes reponsible caller */
    return inputsOutputs.value(input);
}

QList<clword> CliqueModule::getOutputs(const QList<clword> &inputs)
{
    if (_isIdentity) {
        return inputs;
    }

    if (!transformations.isEmpty()) {
        return transformations.transform(inputs);
    }

    if (isOneDimensional(this->inputs) && isOneDimensional(this->outputs)) {
        return QList<clword>() << (clword() << getOutput(inputs[0].first()));
    }

    assert(0);
}

//QList<cl::Transformation> CliqueModule::getCombinationInputs(const QList<Clique> &inputs, const QList<Clique> &outputs, int firstOutput, const std::deque<int> &remainingOutputs)
//{
//    QList<cl::Transformation> ret;

//    //qDebug() << "Process: " << toInt(convert.words(inputs)) << toInt(convert.words(outputs));

//    /* k correponds to the number of additional outputs */
//    for (int k = 0; k < int(remainingOutputs.size()) + 1 && k < noutputs(); k++) {
//        auto combs = comb(remainingOutputs, k);

//        if (k == 0) {
//            std::deque<int> simple;
//            simple.push_front(firstOutput);
//            combs.push_back(simple);
//        } else {
//            for (auto &el : combs) {
//                el.push_front(firstOutput);
//            }
//        }

//        std::deque<int> possibleInputs;
//        for (int i = 0; i < inputs.size(); i++) {
//            possibleInputs.push_back(i);
//        }
//        auto combsIn = comb(possibleInputs, ninputs());

//        for (auto elIn : combsIn) {
//            do {
//                QList<Clique> testIn;
//                for (int i : elIn) testIn.push_back(inputs[i]);

//                QList<Clique> res = getOutputs(testIn);

//                //qDebug() << "Inputs: " << toInt(convert.words(testIn)) << " outputs " << toInt(convert.words(res));

//                for (auto elOut : combs) {
//                    do {
//                        QList<Clique> shouldOut;

//                        for (unsigned i = 0; i < elOut.size(); i++) {
//                            shouldOut.push_back(outputs[elOut[i]]);
//                        }

//                        if (shouldOut == res) {
//                            //qDebug() << toInt(convert.words(shouldOut)) << "matching";
//                            cl::Transformation tr;
//                            tr.module = this;
//                            for (int i : elIn) tr.inputs << i;
//                            for (int i : elOut) tr.outputs << i;

//                            ret.push_back(tr);
//                        } else {
//                            //qDebug() << toInt(convert.words(shouldOut)) << "not matching";
//                        }
//                    } while (std::next_permutation(elOut.begin(), elOut.end()));
//                }
//            } while (std::next_permutation(elIn.begin(), elIn.end()));
//        }
//    }

//    return ret;
//}

void CliqueModule::addModule(CliqueModule *module, QList<int> ins, QList<int> outs)
{
    transformations.push_back({ins, outs, module});
}

void CliqueModule::linkInputOutput(const Clique &input, const Clique &output)
{
    inputsOutputs[input] = output;
}

void CliqueModule::buildIdentity()
{
    _isIdentity = true;
}

void CliqueModule::buildTarget(const Clique &target)
{
    assert(inputs.size() == 1 && outputs.size() == 1);

    _isTarget = true;
    this->target = target;
}

int CliqueModule::getInputSize() const
{
    int maxIn = inputs.size();

    for (const auto &tr : transformations) {
        for (int i : tr.inputs) {
            maxIn = std::max(i+1, maxIn);
        }
    }

    return maxIn;
}

int CliqueModule::getOutputSize() const
{
    int maxIn = outputs.size();

    for (const auto &tr : transformations) {
        for (int i : tr.outputs) {
            maxIn = std::max(i+1, maxIn);
        }
    }

    return maxIn;
}

CliqueModule * CliqueModule::cloneDimensions() const
{
    CliqueModule *cm = new CliqueModule();

    cm->inputs = inputs;
    cm->outputs = outputs;

    return cm;
}
