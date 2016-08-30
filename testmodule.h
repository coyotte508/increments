#ifndef TESTMODULE_H
#define TESTMODULE_H

#include "cliquemodule.h"
#include "classifier.h"

class TestModule : public CliqueModule
{
public:
    TestModule(const QString &name = "");

    void setCharacteristics(const QList<Classifier::Characteristics>& protos, const QList<CliqueModule*> &modules);

    virtual QList<clword> getOutputs(const QList<clword> &inputs) override;

    int getInputSize() const override;
    int getOutputSize() const override;
private:
    bool matchCharacteristics(const QList<Clique> &inputs, const Classifier::Characteristics &proto);

    QList<Classifier::Characteristics> protos;
    QList<CliqueModule*> modules;
};

#endif // TESTMODULE_H
