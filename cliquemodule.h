#ifndef CLIQUEMODULE_H
#define CLIQUEMODULE_H

#include "cliquenetworkmanager.h"

class CliqueModule : public CliqueNetworkManager
{
public:
    CliqueModule();

    void addInputNetwork(CliqueNetwork *nw);
    void addOutputNetwork(CliqueNetwork *nw);

    Clique getOutput(const Clique &input);
    QList<Clique> getOutputs(const QList<Clique> &inputs);

    void linkInputOutput(const Clique &input, const Clique &output);
private:
    QList<CliqueNetwork*> inputs;
    QList<CliqueNetwork*> outputs;
};

#endif // CLIQUEMODULE_H
