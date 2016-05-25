#include "utils.h"

QList<int> randomClique(int c, int l)
{
    QList<int> ret;

    std::uniform_int_distribution<> f(0,l-1);
    for (int i = 0; i < c; i++) {
        ret.push_back(f(e1));
    }

    return ret;
}

QString debug(const QMap<int, int> &histo)
{
    QString ret;
    ret += "{";
    for (int x: histo.keys()) {
        if (ret.size() > 1) {
            ret += ", ";
        }
        ret += "\"" + QString::number(x) + "\"" + ": " + QString::number(histo[x]);
    }
    ret += "}";
    return ret;
}
