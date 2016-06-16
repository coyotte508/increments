#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "cliquemoduleintelligence.h"

class Classifier
{
public:
    typedef QMap<int, Clique> Characteristics;

    Classifier(CliqueModuleIntelligence *cl) : intel(cl) {}

    QList<Characteristics> classify(const QList<QSet<int>> &inputs);
private:
    CliqueModuleIntelligence *intel;
};

#endif // CLASSIFIER_H
