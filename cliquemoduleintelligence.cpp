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

    updateResults();

    qDebug() << "There are " << results.size() << "results.";

    for (const TransformationSet &key : results.keys()) {
        qDebug() << key;
        qDebug() << results[key].size() << " matches";
    }

    //Get the best results possibles
    auto winners = this->winners();

    /* Try to merge multiple "constant functions" into a bigger one */
    trimResults(winners);
    mergeTargets(winners);
}

void CliqueModuleIntelligence::collate()
{
    /* Todo: find way of collating different winners by testing the input and going in the right direction */
    updateResults(); //in case of merged targets, rerun the results with new modules

    auto winners = this->winners();
    trimResults(winners);//necessary?

    /* Todo: find a network that can test which one */
    QList<QSet<int> > inputs;
    for (const auto &winner : winners) {
        inputs.push_back(results[winner]);
    }

    //We now have our sets of inputs.
    //How to create a test function that separates them well?
}

void CliqueModuleIntelligence::updateResults()
{
    results.clear();

    for (int i = 0; i < dataset.size(); i++) {
        processDataSet(i);
    }
}

void CliqueModuleIntelligence::trimResults(const QList<cl::TransformationSet> &winners)
{
    QSet<int> indexes;

    for (int i = 0; i < dataset.size(); i++) {
        indexes << i;
    }

    for (const auto winner: winners) {
        results[winner].intersect(indexes);
        indexes.subtract(results[winner]);
    }
}

QList<cl::TransformationSet> CliqueModuleIntelligence::winners()
{
    QSet<int> indexes;

    for (int i = 0; i < dataset.size(); i++) {
        indexes << i;
    }

    QList<TransformationSet> winners;
    auto results = this->results;

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
            qDebug() << convert.words(in) << " -> " << convert.words(out);
        }

        winners.push_back(winner);
    }

    return winners;
}

void CliqueModuleIntelligence::clearData()
{
    dataset.clear();
    results.clear();
}

CliqueModule *CliqueModuleIntelligence::popModule()
{
    if (newModules.empty()) {
        return nullptr;
    }

    return newModules.takeFirst();
}

QList<CliqueModule*> CliqueModuleIntelligence::getNewModules()
{
    QList<CliqueModule*> ret;
    ret.swap(newModules);

    return ret;
}

void CliqueModuleIntelligence::mergeTargets(QList<TransformationSet> &winners)
{
    qDebug() << "merging target";

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
            auto winner = winners[index];
            auto dataset = results[winner];

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
                auto val = this->dataset[i].second[t.outputs[0]];

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
        if (recordedInputs.values().toSet().size() <= 1) {
            /* All targets pointing to same location, then they should keep being as they are */
            break;
        }

        CliqueModule *disciple = new CliqueModule(*baseModule);
        disciple->setName(QString("internal%1").arg(auxiliaryModules.size()));

        for (const Clique &c: recordedInputs.keys()) {
            disciple->linkInputOutput(c, recordedInputs[c]);
        }

        //Todo: check it's not already in the disciples?
        qDebug() << "adding new module";

        auxiliaryModules.insert(disciple);
        newModules.push_back(disciple);

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

    qDebug() << convert.words(in) << " -> " << convert.words(out);

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
