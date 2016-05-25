#ifndef CONVERTER_H
#define CONVERTER_H

#include <QList>
#include <QHash>

#include "cliquenetwork.h"

/* Converts text <-> cliques/index */

class Converter
{
public:
    Converter();

    QList<QList<int>> cliques() const;
    QList<QList<int>> cliques (const QString &word, bool forceDec = false); //const;
    QList<QList<int>> cliques (const Clique &cl); //const;
    void associate(const Clique &cl, const QList<Clique> &cls);
    QString word (const Clique &data) const;
    QString word (const QList<Clique> &data, bool sep = false) const;
    int count () const;
    void list() const;
    QString gen(int x) const;
    QString decomposeWord(const QString &word);
    void learnWord(const QString &word);
private:
    QHash<QString, QList<int>> data;
    QHash<QList<int>, QString> rdata;
    QHash<QList<int>, QList<Clique>> rawdata;
};

#endif // CONVERTER_H
