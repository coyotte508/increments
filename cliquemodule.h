#ifndef CLIQUEMODULE_H
#define CLIQUEMODULE_H

#include "cliquenetworkmanager.h"
#include "cltransformations.h"

#include <deque>

class CliqueModule : public CliqueNetworkManager
{
public:
    CliqueModule(const QString &name = "");
    CliqueModule(const CliqueModule &other);

    CliqueModule &operator = (const CliqueModule &other);

    ~CliqueModule();

    void addInputNetwork(CliqueNetwork *nw);
    void addOutputNetwork(CliqueNetwork *nw);
    CliqueNetwork *getInputNetwork(int i) const;
    CliqueNetwork *getOutputNetwork(int i) const;

    void setOwnership(bool ownership);

    Clique getOutput(const Clique &input);
    virtual QList<Clique> getOutputs(const QList<Clique> &inputs);

    QList<cl::Transformation> getCombinationInputs(const QList<Clique> &inputs, const QList<Clique> &outputs, int firstOutput, const std::deque<int> &remainingOutputs);

    /* More costly functions which will determine which cliques are connected
     * to a specific input or output exactly */
    QList<Clique> analyzeInput(const Clique &output);
    QList<Clique> analyzeOutput(const Clique &input);

    void addModule(CliqueModule *module, QList<int> ins, QList<int> outs);
    Clique addDestinationModule(CliqueModule *module);
    void addDestinationModule(CliqueModule *module, const Clique &c);
    void clearOutputCliques();
    void linkInputOutput(const Clique &input, const Clique &output);
    void buildIdentity();
    void buildTarget(const Clique &dest);

    int ninputs () const { return inputs.size(); }
    int noutputs () const { return outputs.size(); }

    bool isTarget() const { return _isTarget;}

    QString name() const { return m_Name;}
    void setName(const QString &name) {m_Name = name;}
private:
    QList<CliqueNetwork*> inputs;
    QList<CliqueNetwork*> outputs;

    QHash<Clique, CliqueModule*> destinationModules;

    bool ownership = false;
    bool _isTarget = false;
    bool _isIdentity = false;
    QString m_Name;
};

#endif // CLIQUEMODULE_H
