#ifndef TOURNAMENT_H
#define TOURNAMENT_H

#include <QHash>
#include "cliquenetwork.h"

class Tournament
{
public:
    Tournament();

    void init (int n, int c, int l);
    void convoluate(const QList<Clique> &cliques);
    void learnClique(const Clique &c);
    CliqueNetwork &network(int i);

    int size() const { return networks.size();}

    QList<QList<Clique>> topSequences(int n);
    void clearCache();

    QList<QPair<CliqueNetwork*, cl::coord>> getCoords(const QList<Clique> &cls);

    static QList<int> joinCliques(const QList<Clique> &l, int i, int j);
    static QList<Clique> splitCliques(const Clique &c, int size);
private:
    QList<CliqueNetwork> networks;

    QHash<Clique, int> occurences;
};

#endif // TOURNAMENT_H
