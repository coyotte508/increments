#ifndef CLIQUEMODULEINTELLIGENCE_H
#define CLIQUEMODULEINTELLIGENCE_H

#include <QList>

namespace cl {
    struct Transformation;
}

uint qHash(const cl::Transformation &key, uint seed = 0);

#include <QHash>
#include "cliquemodule.h"

QDebug operator << (QDebug stream, const cl::Transformation &t);

inline uint qHash(const cl::Transformation &key, uint seed)
{
    uint current = 0;
    current = qHash(QPair<QList<int>,QList<int>>(key.inputs, key.outputs), seed);
    current = qHash(QPair<int, CliqueModule*>(current, key.module));

    return current;
}

class CliqueModuleIntelligence {
public:

    typedef QList<Clique> input;
    typedef QList<Clique> output;
    typedef QList<input> inputlist;
    typedef QList<output> outputlist;
    typedef QPair<input, output> inputoutput;
    typedef QPair<inputlist, outputlist> inputoutputlist;

    CliqueModuleIntelligence();
    ~CliqueModuleIntelligence();

    void setBaseModel(CliqueModule *mod);
    void addAuxiliaryModule(CliqueModule *mod);
    void addInputOutput(const inputlist &in, const outputlist &out);
    void addInputOutput(const input &in, const output &out);

    void resolve();
    void collate();
    void clearData();

    QList<clword> test(const QList<clword> &input);

    CliqueModule *popModule();
    QList<CliqueModule*> getNewModules();

    const input& getInput(int index) const;
protected:
    void processDataSet(int index);
    void mergeTargets(QList<cl::TransformationSet> &winners);
    void updateResults();
    void trimResults(const QList<cl::TransformationSet> &winners); //trim results to only what's necessary
    QList<cl::TransformationSet> winners();
private:
    QList<inputoutputlist> dataset;
    CliqueModule base;

    QSet<CliqueModule *> auxiliaryModules;
    QList<CliqueModule *> newModules;

    QHash<cl::TransformationSet, QSet<int>> results;
};


#endif
