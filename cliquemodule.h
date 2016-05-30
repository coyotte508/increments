#ifndef CLIQUEMODULE_H
#define CLIQUEMODULE_H

#include "cliquenetworkmanager.h"

class CliqueModule : public CliqueNetworkManager
{
public:
    CliqueModule();
    CliqueModule(const CliqueModule &other);

    CliqueModule &operator = (const CliqueModule &other);

    ~CliqueModule();

    void addInputNetwork(CliqueNetwork *nw);
    void addOutputNetwork(CliqueNetwork *nw);

    Clique getOutput(const Clique &input);
    QList<Clique> getOutputs(const QList<Clique> &inputs);

    void linkInputOutput(const Clique &input, const Clique &output);
    void buildIdentity();

    int ninputs () const { return inputs.size(); }
    int noutputs () const { return outputs.size(); }
private:
    QList<CliqueNetwork*> inputs;
    QList<CliqueNetwork*> outputs;

    bool ownership = false;
};

#endif // CLIQUEMODULE_H
