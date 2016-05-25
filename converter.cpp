#include <QList>
#include <QDebug>
#include "utils.h"
#include "converter.h"

Converter::Converter()
{

}

QList<QList<int>> Converter::cliques() const
{
    return data.values();
}

QList<QList<int>> Converter::cliques(const Clique &cl)
{
    return rawdata.value(cl);
}

void Converter::learnWord(const QVariant &word, Clique clique)
{
    if (!data.contains(word)) {
        if (clique.length() == 0) {
            clique = randomClique(8,256);
        }
        data.insert(word, clique);
        rdata.insert(clique, word);
    }
}

void Converter::associate(const Clique &cl, const QList<Clique> &cls)
{
//    QString w = word(cls);
//    data.insert(w, cl);
//    rdata.insert(cl, w);
    rawdata.insert(cl, cls);
}

QVariant Converter::word(const Clique &data) const
{
    return rdata.value(data);
}

Clique Converter::clique(const QVariant &word) const
{
    return data.value(word);
}

int Converter::count() const
{
    return rdata.count();
}

void Converter::list() const
{
    for (const QVariant &s : data.keys()) {
        qDebug() << s;
    }
}
