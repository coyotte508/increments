#ifndef CLIQUENETWORK_H
#define CLIQUENETWORK_H

#include <QList>
#include <QMap>
#include <QSet>

typedef QList<int> Clique;

inline uint qHash(const QList<int> &key, uint seed = 0)
{
    uint current = 0;
    for (int i : key) {
        current = qHash(QPair<int,int>(current, i), seed);
    }

    return current;
}

namespace cl {
    typedef QPair<int, int> coord;
}

class CliqueNetwork
{
public:
    CliqueNetwork();
    CliqueNetwork(const CliqueNetwork &other);

    void init(int clusters, int l);
    void addClique(const Clique& c);
    void addLink(const cl::coord &src, const cl::coord &dest);
    void addLink(const cl::coord &src, CliqueNetwork *nw, const cl::coord &dest);
    void linkClique(const Clique &c, CliqueNetwork *nw, const Clique &c2);
    Clique randomClique() const;
    Clique activatedClique() const;
    int cliqueSize() const;
    void activateClique(const Clique &c);
    void stimulate(const cl::coord &dest);
    void propagate();
    void stabilize();
    void shutdown();
private:
    typedef QMap<int, QSet<cl::coord>> cluster;
    QSet<Clique> cliques;
    QList<cluster> network;
    QMap<cl::coord, QSet<QPair<CliqueNetwork*, cl::coord>>> extraLinks;
    Clique activated;
    QMap<cl::coord, int> stimulated;
    int l = 0;
    int c = 0;
};

#endif // CLIQUENETWORK_H
