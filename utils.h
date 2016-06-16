#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <QList>
#include <QString>
#include <QMap>
#include <QHash>
#include <QVariant>

uint qHash(const QVariant &v);

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

std::default_random_engine &rng();

QList<int> randomClique(int c, int l);

QList<int> toInt(const QList<QVariant> &l);

QString debug(const QMap<int, int> &histo);

#endif // UTILS_H
