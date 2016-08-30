#ifndef COMPARER_H
#define COMPARER_H

#include "cliquenetwork.h"

enum CompResult {
    Equal,
    Inferior,
    Superior,
    Impossible
};

struct Comparison {
    Clique a,b;
    CompResult result;

    bool operator == (const Comparison &other) const {
        return a == other.a && b == other.b && result == other.result;
    }
};

inline uint qHash(const Comparison &key, uint seed = 0)
{
    return qHash(QPair<int, int>(qHash(QPair<Clique,Clique>(key.a, key.b), seed), key.result), seed);
}

class Comparer
{
public:
    Comparer();

    void addComparison(Clique A, Clique B, CompResult Result);
    void computeOrder();
private:
    QList<Clique> order;
    QList<Comparison> comps;

    QHash<Clique, QSet<Comparison>> compsByCl;
};

#endif // COMPARER_H
