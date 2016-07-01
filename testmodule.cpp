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

    auto &m = modules[0];
    for (int i = 0; i < m->ninputs(); i++) {
        addInputNetwork(m->getInputNetwork(i));
    }
    for (int i = 0; i < m->noutputs(); i++) {
        addOutputNetwork(m->getOutputNetwork(i));
    }
}

QList<Clique> TestModule::getOutputs(const QList<Clique> &inputs)
{
    assert(!modules.empty() > 0);

    for (int i = protos.size()-1; i > 0; i--) {
        if (matchCharacteristics(inputs, protos[i])) {
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
