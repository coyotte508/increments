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

QList<QList<int>> Converter::cliques(const QString &word, bool forceDec)// const
{
    QList<Clique> ret;

    for (const QChar &c : word) {
        learnWord(QString(c));
    }

    QMap<int, QList<Clique>> bestScore;
    bestScore[0];//insert element at 0

    auto bs = [&](int x) {
        if (!bestScore.contains(x)) {
            return 1000000;
        } else {
            return bestScore[x].size();
        }
    };

    for (int i = 1; i <= word.size(); i++) {
        for (int j = 0; j < i; j++) {
            if (j == 0 && i == word.size() && forceDec) {
                continue;
            }
            if (data.contains(word.mid(j, i-j)) && bs(i) > bs(j) + 1) {
                bestScore[i] = bestScore[j];
                bestScore[i].push_back(data[word.mid(j, i-j)]);
            }
        }
    }

    return bestScore[word.size()];
}

QList<QList<int>> Converter::cliques(const Clique &cl)
{
    return rawdata.value(cl);
}

void Converter::learnWord(const QString &word)
{
    if (!data.contains(word)) {
        auto clique = randomClique(8,256);
        data.insert(word, clique);
        rdata.insert(clique, word);
    }
}

QString Converter::decomposeWord(const QString &word)
{
    auto l = cliques(word, true);
    QStringList ret;

    for (auto c : l) {
        ret.push_back(this->word(c));
    }
    return ret.join('|');
}

void Converter::associate(const Clique &cl, const QList<Clique> &cls)
{
    QString w = word(cls);
    data.insert(w, cl);
    rdata.insert(cl, w);
    rawdata.insert(cl, cls);
}

QString Converter::word(const Clique &data) const
{
    return rdata.value(data, "_");
}

QString Converter::word(const QList<Clique> &l, bool sep) const
{
    QString ret;

    for (const Clique &c : l) {
        if (sep && ret.size() >  0) {
            ret += "|";
        }
        if (rdata.contains(c)) {
            ret += rdata[c];
        } else {
            ret += "_";
        }
    }

    return ret;
}

QString Converter::gen(int x) const
{
    QList<QString> l = rdata.values();

    std::uniform_int_distribution<> dist(0, l.size()-1);

    QString ret;

    for (int i = 0; i < 4; i++) {
        ret += l[dist(e1)];
    }

    return ret;
}

int Converter::count() const
{
    return rdata.count();
}

void Converter::list() const
{
    for (QString s : data.keys()) {
        qDebug() << s;
    }
}
