#ifndef CLTRANSFORMATIONS
#define CLTRANSFORMATIONS

#include <QList>
#include <QPair>
#include <QSet>
#include "cliquenetwork.h"

class CliqueModule;

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

    class TransformationSet : public QList<Transformation> {
    public:
        CliqueModule *createModule() const;
        QList<Clique> transform(const QList<Clique> &in);

        QString toString() const;
    };
    typedef QPair<QSet<int>, QSet<int>> Results;
}

#endif // CLTRANSFORMATIONS

