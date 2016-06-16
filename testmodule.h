#ifndef TESTMODULE_H
#define TESTMODULE_H

#include "cliquemodule.h"
#include "classifier.h"

class TestModule : public CliqueModule
{
public:
    TestModule();

    void setCharacteristics(const QList<Classifier::Characteristics>& protos, const QList<CliqueModule*> &modules);

    QList<Clique> getOutputs(const QList<Clique> &inputs) override;
private:
    bool matchCharacteristics(const QList<Clique> &inputs, const Classifier::Characteristics &proto);

    QList<Classifier::Characteristics> protos;
    QList<CliqueModule*> modules;
};

#endif // TESTMODULE_H