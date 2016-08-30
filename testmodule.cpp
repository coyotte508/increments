#include <cassert>
#include "testmodule.h"
#include <QDebug>

TestModule::TestModule(const QString &name) : CliqueModule(name)
{

}

void TestModule::setCharacteristics(const QList<Classifier::Characteristics> &protos, const QList<CliqueModule *> &modules)
{
    this->protos = protos;
    this->modules = modules;
}

QList<clword> TestModule::getOutputs(const QList<clword> &inputs)
{
    assert(!modules.empty() > 0);

    for (int i = protos.size()-1; i > 0; i--) {
        if (matchCharacteristics(inputs[0], protos[i])) {
            qDebug() << "using " << modules[i]->name();
            return modules[i]->getOutputs(inputs);
        }
    }
    qDebug() << "using " << modules[0]->name();
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

int TestModule::getOutputSize() const {
    if (!modules.empty()) {
        return modules[0]->getOutputSize();
    }
    return 0;
}

int TestModule::getInputSize() const {
    int maxIn = 0;
    for (CliqueModule *m : modules) {
        maxIn = std::max(m->getInputSize(), maxIn);
    }

    return maxIn;
}
