#ifndef CLIQUENETWORKMANAGER_H
#define CLIQUENETWORKMANAGER_H

#include "cliquenetwork.h"

class CliqueNetworkManager
{
public:
    CliqueNetworkManager();

    void addNetwork(CliqueNetwork &nw);
    void iterate();
    void shutdown();
private:
    QSet<CliqueNetwork *> nws;
};

#endif // CLIQUENETWORKMANAGER_H
