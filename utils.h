#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <QList>
#include <QString>
#include <QMap>

// Seed with a real random value, if available
static std::random_device r;

// Choose a random mean between 1 and 6
static std::default_random_engine e1(r());

QList<int> randomClique(int c, int l);

QString debug(const QMap<int, int> &histo);

#endif // UTILS_H
