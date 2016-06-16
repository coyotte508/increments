#ifndef CLIQUENETWORK_H
#define CLIQUENETWORK_H

#include <QList>
#include <QMap>
#include <QSet>

typedef QList<int> Clique;

template <class T>
inline uint qHash(const QList<T> &key, uint seed = 0)
{
    uint current = 0;
    for (const T & val : key) {
        current = qHash(QPair<int,T>(current, val), seed);
    }

    return current;
}

namespace cl {
    typedef QPair<int, int> coord;
}

class CliqueNetwork
{
    typedef QPair<CliqueNetwork*, cl::coord> destlink;
public:
    CliqueNetwork();
    CliqueNetwork(const CliqueNetwork &other);

    void init(int clusters, int l);
    void copyAdd(const CliqueNetwork &other);
    void clear(); // only remove local cliques
    void addClique(const Clique& c);
    void removeClique(const Clique &c, bool update=true);
    void addLink(const cl::coord &src, const cl::coord &dest);
    void addLink(const cl::coord &src, CliqueNetwork *nw, const cl::coord &dest);
    void buildIdentity(CliqueNetwork *other);
    void linkClique(const Clique &c, CliqueNetwork *nw, const Clique &c2);
    Clique randomClique() const;
    Clique activatedClique() const;
    int cliqueSize() const;
    void activateClique(const Clique &c);
    void stimulate(const cl::coord &dest);
    void propagate();
    void stabilize();
    void shutdown();
    void recliques();

    bool isEmpty() const;
    bool isFullyConnectedTo(const Clique &input, CliqueNetwork *dest, const Clique &output) const;

    QSet<Clique> allCliques() const {return cliques;}
private:
    typedef QMap<int, QSet<cl::coord>> cluster;
    QSet<Clique> cliques;
    QList<cluster> network;
    QMap<cl::coord, QSet<destlink>> extraLinks;
    Clique activated;
    QMap<cl::coord, int> stimulated;
    int l = 0;
    int c = 0;
};

#endif // CLIQUENETWORK_H
