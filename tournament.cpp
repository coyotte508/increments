#include "tournament.h"

Tournament::Tournament()
{

}

void Tournament::init(int n, int c, int l)
{
    for (int i = 0; i < n; i++) {
        networks.push_back(CliqueNetwork());
        networks.last().init(c, l);
    }
}

void Tournament::convoluate(const QList<Clique> &cliques)
{
    for (int i = 0; i < cliques.size(); i++) {
        for (int j = i+1; j < cliques.size() && j-i < size(); j++) {
            occurences[joinCliques(cliques, i, j)] += j-i+1;
        }
    }
}

void Tournament::learnClique(const Clique &c)
{
    for (CliqueNetwork &nw: networks) {
        nw.addClique(c);
    }
}

CliqueNetwork &Tournament::network(int i)
{
    return networks[i];
}

QList<int> Tournament::joinCliques(const QList<Clique> &l, int i, int j)
{
    QList<int> elements;

    for (int x = i; x <= j; x++) {
        elements.append(l[x]);
    }

    return elements;
}

QList<Clique> Tournament::splitCliques(const Clique &c, int size)
{
    QList<Clique> ret;

    for (int i = 0; i < c.size(); i += size) {
        Clique _c;
        for (int j = i; j < i+size; j++) {
            _c.push_back(c[j]);
        }
        ret.push_back(_c);
    }

    return ret;
}

QList<QList<Clique>> Tournament::topSequences(int n)
{
    QMultiMap<int, QList<Clique>> sequences;

    for (auto key : occurences.keys()) {
        sequences.insertMulti(occurences[key], splitCliques(key, network(0).cliqueSize()));

        if (sequences.size() > n) {
            sequences.erase(sequences.begin());
        }
    }

    return sequences.values();
}

void Tournament::clearCache()
{
    occurences.clear();
}

QList<QPair<CliqueNetwork*, cl::coord>> Tournament::getCoords(const QList<Clique> &cls)
{
    QList<QPair<CliqueNetwork*, cl::coord>> ret;

    for (int i = 0; i < cls.size(); i++) {
        CliqueNetwork *nw = &networks[i];

        for (int j = 0; j < cls[i].size(); j++) {
            ret.push_back(QPair<CliqueNetwork*, cl::coord>(nw, cl::coord(j, cls[i][j])));
        }
    }

    return ret;
}
