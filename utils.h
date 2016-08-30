#ifndef UTILS_H
#define UTILS_H

#include <utility>
#include <random>
#include <QList>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVariant>
#include <list>

uint qHash(const QVariant &v);

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

std::default_random_engine &rng();

QList<int> randomClique(int c, int l);

QList<QString> toInt(const QList<QVariant> &l);

QString debug(const QMap<int, int> &histo);

template<class T>
QList<T> comb(T cont, int K)
{
    QList<T> ret;

    std::string bitmask(K, 1); // K leading 1's
    bitmask.resize(cont.size(), 0); // N-K trailing 0's

    // print integers and permute bitmask
    do {
        T v;

        for (int i = 0; i < int(cont.size()); ++i) // [0..N-1] integers
        {
            if (bitmask[i]) {
                v.push_back(cont[i]);
            }
        }
        ret.push_back(v);
    } while (std::prev_permutation(bitmask.begin(), bitmask.end()));

    return ret;
}

template <class T>
T left(T cont, int count)
{
    T ret;

    for (auto x : cont) {
        if (!count--) {
            break;
        }
        ret.push_back(x);
    }

    return ret;
}

template <class T>
T mid(T cont, int pos)
{
    T ret;

    while (pos < cont.size()) {
        ret.push_back(cont[pos++]);
    }

    return ret;
}

template<class T>
QList<T> prepareList(int count, T sample)
{
    QList<T> rest;

    while (count-- > 0) {
        rest.push_back(sample);
    }

    return rest;
}

template <class T>
QList<QList<T>> subIndexes(QList<QList<T>> list, QList<int> indexes)
{
    QList<QList<T>> ret;

    for (auto l : list) {
        QList<T> part;
        for (int x : indexes) {
            part.push_back(l[x]);
        }
        ret.push_back(part);
    }

    return ret;
}

#endif // UTILS_H
