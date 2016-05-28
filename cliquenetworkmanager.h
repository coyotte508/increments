#ifndef CLIQUENETWORKMANAGER_H
#define CLIQUENETWORKMANAGER_H

#include "cliquenetwork.h"

class CliqueNetworkManager
{
public:
    CliqueNetworkManager();

    void addNetwork(CliqueNetwork *nw);
    void iterate();
    void shutdown();
    Clique getOutput(CliqueNetwork *ori, CliqueNetwork *dest, const Clique &inputClique);
protected:
    QSet<CliqueNetwork *> nws;
};

#endif // CLIQUENETWORKMANAGER_H
