#include <cassert>
#include "testmodule.h"

TestModule::TestModule()
{

}

void TestModule::setCharacteristics(const QList<Classifier::Characteristics> &protos, const QList<CliqueModule *> &modules)
{
    this->protos = protos;
    this->modules = modules;
}

QList<Clique> TestModule::getOutputs(const QList<Clique> &inputs)
{
    assert(!modules.empty() > 0);

    for (int i = protos.size()-1; i > 0; i--) {
        if (matchCharacteristics(inputs, protos[i])) {
            return modules[i]->getOutputs(inputs);
        }
    }
    return modules[0]->getOutputs(inputs);
}

bool TestModule::matchCharacteristics(const QList<Clique> &inputs, const Classifier::Characteristics &proto)
{
    if (proto.isEmpty()) {
        return false;
    }

    for (int key  : proto.keys()) {
        if (inputs[key] != proto[key]) {
            return false;
        }
    }

    return true;
}
