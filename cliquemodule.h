#ifndef CLIQUEMODULE_H
#define CLIQUEMODULE_H

#include "cliquenetworkmanager.h"
#include "cltransformations.h"

#include <deque>

class CliqueModule : public CliqueNetworkManager
{
public:
    CliqueModule(const QString &name = "");
    ~CliqueModule();

    void addInputNetwork();
    void addOutputNetwork();

    Clique getOutput(const Clique &input);
    virtual QList<Clique> getOutputs(const QList<Clique> &inputs);

    QList<cl::Transformation> getCombinationInputs(const QList<Clique> &inputs, const QList<Clique> &outputs, int firstOutput, const std::deque<int> &remainingOutputs);

    void addModule(CliqueModule *module, QList<int> ins, QList<int> outs);
    void linkInputOutput(const Clique &input, const Clique &output);
    void buildIdentity();
    void buildTarget(const Clique &dest);

    virtual int ninputs () const ;
    virtual int noutputs () const ;

    bool isTarget() const { return _isTarget;}

    QString name() const { return m_Name;}
    void setName(const QString &name) {m_Name = name;}

    template<class T>
    void setParent(T* ptr) {
        this->parent = ptr;
    }

    template <class T>
    bool isParent(T* ptr) {
        return parent == ptr;
    }

    CliqueModule *cloneDimensions() const;
private:
    QList<Clique> inputs;
    QList<Clique> outputs;

    QHash<Clique, Clique> inputsOutputs;

    cl::TransformationSet transformations;

    bool _isTarget = false;
    bool _isIdentity = false;
    void *parent = nullptr;
    Clique target;
    QString m_Name;
};

#endif // CLIQUEMODULE_H
