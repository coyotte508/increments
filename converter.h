#ifndef CONVERTER_H
#define CONVERTER_H

#include <QList>
#include <QHash>
#include <QVariant>

#include "cliquenetwork.h"

/* Converts text <-> cliques/index */

class Converter
{
public:
    typedef QList<Clique> clword;
    typedef QList<clword> clwords;
    Converter();

    QList<QList<int>> cliques() const;
    QList<QList<int>> cliques (const Clique &cl); //const;
    void associate(const Clique &cl, const clword &cls);
    QVariant word (const Clique &data) const;
    QVariantList words (const clword &data) const;
    QVariantList words (const clwords &data) const;
    Clique clique(const QVariant &word) const;
    int count () const;
    void list() const;
    void learnWord(const QVariant &word, Clique cl = Clique());
private:
    QHash<QVariant, QList<int>> data;
    QHash<QList<int>, QVariant> rdata;
    QHash<QList<int>, QList<Clique>> rawdata;
};

#endif // CONVERTER_H
