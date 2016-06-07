#ifndef CLIQUEMODULEINTELLIGENCE_H
#define CLIQUEMODULEINTELLIGENCE_H

#include <QList>

namespace cl {
    struct Transformation;
}

uint qHash(const cl::Transformation &key, uint seed = 0);

#include <QHash>
#include "cliquemodule.h"

namespace cl {
    struct Transformation {
        /* Allows for some reordering / selection of inputs / outputs */
        QList<int> inputs;
        QList<int> outputs;
        CliqueModule *module;

        bool operator == (const Transformation &other) const {
            return inputs == other.inputs && outputs == other.outputs && module == other.module;
        }

        bool matchInputsOutputs(const Transformation &other) const {
            return inputs == other.inputs && outputs == other.outputs;
        }
    };

    typedef QList<Transformation> TransformationSet;
    typedef QPair<QSet<int>, QSet<int>> Results;
}

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
    typedef QPair<input, output> inputoutput;

    CliqueModuleIntelligence();

    void setBaseModel(CliqueModule *mod);
    void addAuxiliaryModule(CliqueModule *mod);
    void addInputOutput(const input &in, const output &out);
    void mergeTargets( QList<cl::TransformationSet> &winners);
    void resolve();
    void collate();
protected:
    void processDataSet(int index);

private:
    QList<inputoutput> dataset;
    CliqueModule base;

    QSet<CliqueModule *> auxiliaryModules;

    QHash<cl::TransformationSet, QSet<int>> results;
};


#endif
