#include <cassert>
#include "classifier.h"

QList<Classifier::Characteristics> Classifier::classify(const QList<QSet<int> > &_inputs)
{
    auto inputs = _inputs;
    std::reverse(inputs.begin(), inputs.end());

    QList<Characteristics> ret;

    while (inputs.size() > 1) {
        QList<Clique> base = intel->getInput(*inputs.first().begin());
        /* Get similarities in inputs */
        for (int i : inputs.first()) {
            auto in = intel->getInput(i);

            for (int j = 0; j < in.size(); j++) {
                if (in[j] != base[j]) {
                    base[j].clear();
                }
            }
        }

        /* Remove if they are present in other inputs */
        for (int i = 1; i < inputs.size(); i++) {
            for (int j : inputs[i]) {
                auto in = intel->getInput(j);

                for (int k = 0; k < in.size(); k++) {
                    if (in[k] == base[k]) {
                        base[k].clear();
                    }
                }
            }
        }

        /* Make sure we have at least one */
        QMap<int, Clique> characteristics;
        for (int i = 0; i < base.size(); i++) {
            if (!base[i].empty()) {
                characteristics.insert(i, base[i]);
            }
        }

        assert(!characteristics.empty());
        ret.push_back(characteristics);
        inputs.pop_front();
    }

    ret.push_back(Characteristics());

    std::reverse(ret.begin(), ret.end());

    return ret;
}
