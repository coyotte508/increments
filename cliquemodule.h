#ifndef CLIQUEMODULE_H
#define CLIQUEMODULE_H

#include "cliquenetworkmanager.h"

class CliqueModule : public CliqueNetworkManager
{
public:
    CliqueModule(const QString &name = "");
    CliqueModule(const CliqueModule &other);

    CliqueModule &operator = (const CliqueModule &other);

    ~CliqueModule();

    void addInputNetwork(CliqueNetwork *nw);
    void addOutputNetwork(CliqueNetwork *nw);

    void setOwnership(bool ownership);

    Clique getOutput(const Clique &input);
    QList<Clique> getOutputs(const QList<Clique> &inputs);

    /* More costly functions which will determine which cliques are connected
     * to a specific input or output exactly */
    QList<Clique> analyzeInput(const Clique &output);
    QList<Clique> analyzeOutput(const Clique &input);

    void linkInputOutput(const Clique &input, const Clique &output);
    void buildIdentity();
    void buildTarget(const Clique &dest);

    int ninputs () const { return inputs.size(); }
    int noutputs () const { return outputs.size(); }

    QString name() const { return m_Name;}
    void setName(const QString &name) {m_Name = name;}
private:
    QList<CliqueNetwork*> inputs;
    QList<CliqueNetwork*> outputs;

    bool ownership = false;
    bool isTarget = false;
    QString m_Name;
};

#endif // CLIQUEMODULE_H
