#include <functional>
#include <cassert>
#include "cliquemoduleintelligence.h"
#include "converter.h"

//Needs to be below sadly due to qHash redefinition
#include <QDebug>

using namespace cl;

extern Converter convert;

QDebug operator << (QDebug stream, const Transformation &t) {
    stream << t.inputs << " " << t.module->name() << " " << t.outputs;
    return stream;
}

CliqueModuleIntelligence::CliqueModuleIntelligence()
{

}

void CliqueModuleIntelligence::setBaseModel(CliqueModule *mod)
{
    base = CliqueModule(*mod);
}

void CliqueModuleIntelligence::addAuxiliaryModule(CliqueModule *mod)
{
    auxiliaryModules.insert(mod);
}

void CliqueModuleIntelligence::addInputOutput(const input &in, const output &out)
{
    dataset.push_back(inputoutput(in, out));
}

void CliqueModuleIntelligence::resolve()
{
    if (dataset.empty()) {
        qDebug() << "Empty dataset!";
        return;
    }

    for (int i = 0; i < dataset.size(); i++) {
        processDataSet(i);
    }

    qDebug() << "There are " << results.size() << "results.";

    for (const TransformationSet &key : results.keys()) {
        qDebug() << key;
        qDebug() << results[key].size() << " matches";
    }

    //Get the best results possibles
    QSet<int> indexes;

    for (int i = 0; i < dataset.size(); i++) {
        indexes << i;
    }

    QList<TransformationSet> winners;
    int i = 0;
    while (indexes.size() > 0) {
        TransformationSet winner;
        int max = 0;
        i++;

        for (const TransformationSet &key : results.keys()) {
            int count = results[key].intersect(indexes).count();

            if (count > max) {
                max = count;
                winner = key;
            }
        }

        qDebug() << "Winner #" << i << " " << winner << ": " << results[winner].size();

        indexes.subtract(results[winner]);

        for (int remaining : indexes) {
            const auto &in = dataset[remaining].first;
            const auto &out = dataset[remaining].second;
            qDebug() << convert.word(in[0]).toInt() << convert.word(in[1]).toInt() << " -> " << convert.word(out[0]).toInt() << convert.word(out[1]).toInt();
        }

        winners.push_back(winner);
    }

    /* Try to merge multiple "constant functions" into a bigger one */
    mergeTargets(winners);
}

void CliqueModuleIntelligence::mergeTargets(QList<TransformationSet> &winners)
{
    /* Try to rebuild target types */
    while (1) {
        /* We'll try to merge everything into firstTarget */
        int firstTarget = -1;
        int indice = 0;
        CliqueModule *baseModule = nullptr;
        for (int i = 0; i < winners.size(); i++) {
            for (int j = 0; j < winners[i].size(); j++) {
                if (winners[i][j].module->isTarget()) {
                    firstTarget = i, indice = j;
                    baseModule = winners[i][j].module;
                    goto outloop;
                }
            }
        }
        break;
outloop:
        QHash<Clique, Clique> recordedInputs;
        QSet<QPair<int,int>> locations;

        locations.insert(QPair<int,int>(firstTarget, indice));

        auto logInputs = [&](int index, const cl::Transformation &t) {
            auto dataset = results[winners[index]];

            assert(t.inputs.size() == 1);
            assert(t.outputs.size() == 1);

            for (int i: dataset) {
                recordedInputs[this->dataset[i].first[t.inputs[0]]] = this->dataset[i].second[t.outputs[0]];
            }
        };
        logInputs(firstTarget, winners[firstTarget][indice]);

        auto areInputsCompatible = [&] (int index, const cl::Transformation &t) {
            auto dataset = results[winners[index]];

            assert(t.inputs.size() == 1);
            assert(t.outputs.size() == 1);

            for (int i: dataset) {
                auto key = this->dataset[i].first[t.inputs[0]];
                auto val = this->dataset[i].second[t.outputs[1]];

                if (recordedInputs.value(key, val) != val) {
                    return false;
                }
            }
            return true;
        };

        /* Search target targets :) */
        for (int i = firstTarget+1; i < winners.size(); i++) {
            for (int j = 0; j < winners[i].size(); j++) {
                if (!winners[i][j].module->isTarget()) {
                    continue;
                }
                if (!winners[i][j].matchInputsOutputs(winners[firstTarget][indice])) {
                    continue;
                }

                /* Check if targets are compatible */
                if (!areInputsCompatible(i, winners[i][j])) {
                    continue;
                }

                logInputs(i, winners[i][j]);
                locations.insert(QPair<int,int>(i, j));
            }
        }

        /* Merge 'em for real */
        if (locations.values().toSet().size() <= 1) {
            /* All targets pointing to same location, then they should keep being as they are */
            break;
        }

        CliqueModule *disciple = new CliqueModule(*baseModule);

        for (const Clique &c: recordedInputs.keys()) {
            disciple->linkInputOutput(c, recordedInputs[c]);
        }

        //Todo: check it's not already in the disciples?

        auxiliaryModules.insert(disciple);

        /* Update the results by merging now-identical transformation sets */
        for (const QPair<int, int> &location : locations) {
            auto data = results.take(winners[location.first]);
            winners[location.first][location.second].module = disciple;
            results[winners[location.first]].unite(data);
        }
        winners = results.keys();
    }
}

void CliqueModuleIntelligence::processDataSet(int index)
{
    const auto &data = dataset[index];
    const input &in = data.first;
    const output &out = data.second;

    QSet<TransformationSet> sets;

    qDebug() << convert.word(in[0]) << convert.word(in[1]) << " -> " << convert.word(out[0]) << convert.word(out[1]);

    std::function<void(int, const TransformationSet&)> rec
            = [&rec, &sets, index, &out, &in, this] (int j, const TransformationSet &current) {
        if (j >= out.size()) {
            results[current].insert(index);
            qDebug() << current;
            return;
        }

        const auto &target = out[j];
        //qDebug() << target;

        for (CliqueModule *module : auxiliaryModules) {
            for (int i = 0; i < in.size(); i++) {
                //qDebug() << in[i] << module->getOutput(in[i]);
                if (module->getOutput(in[i]) == target) {
                    TransformationSet alt = current;
                    alt.append({{i},{j}, module});
                    rec(j+1, alt);
                }
            }
        }
    };

    rec(0, TransformationSet());
}
