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
    QString m_Name;
};

#endif // CLIQUEMODULE_H
