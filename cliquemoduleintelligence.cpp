#include <functional>
#include <cassert>
#include "cliquemoduleintelligence.h"
#include "testmodule.h"
#include "converter.h"
#include "classifier.h"
#include "utils.h"

//Needs to be below sadly due to qHash redefinition
#include <QDebug>
#include <deque>

using namespace cl;

extern Converter convert;

CliqueModule* cl::TransformationSet::createModule() const
{
    CliqueModule *module = new CliqueModule(toString());
    QMap<int, CliqueNetwork*> inputs;
    QMap<int, CliqueNetwork*> outputs;

    for (const Transformation& t: *this) {
        for (int i = 0; i < t.inputs.size(); i++) {
            int key = t.inputs[i];
            CliqueNetwork *ori = t.module->getInputNetwork(i);
            if (!inputs.contains(key)) {
                inputs[key] = new CliqueNetwork();
            }
            inputs[key]->copyAdd(*ori);
        }
        for (int i = 0; i < t.outputs.size(); i++) {
            int key = t.outputs[i];
            CliqueNetwork *ori = t.module->getOutputNetwork(i);
            if (!outputs.contains(key)) {
                outputs[key] = new CliqueNetwork();
            }
            outputs[key]->copyAdd(*ori);
        }
    }

    /* One or more inputs may be ignored */
    QList<CliqueNetwork *> linputs;
    for (int k: inputs.keys()) {
        while (linputs.size() != k) {
            linputs.push_back(new CliqueNetwork());
        }
    }

    //assert(inputs.key(inputs.first()) == 0 && inputs.key(inputs.last()) == inputs.size() -1);
    assert(outputs.key(outputs.first()) == 0 && outputs.key(outputs.last()) == outputs.size() -1);

    for (CliqueNetwork *c : inputs.values()) {
        module->addInputNetwork(c);
    }
    for (CliqueNetwork *c : outputs.values()) {
        module->addOutputNetwork(c);
    }

    for (const Transformation &t: *this) {
        module->addModule(t.module, t.inputs, t.outputs);
    }

    return module;
}

QList<Clique> cl::TransformationSet::transform(const QList<Clique> &in)
{
    QMap<int, Clique> results;

    for (const cl::Transformation &tr : *this) {
        QList<Clique> inputs;
        for (int i : tr.inputs) {
            inputs.push_back(in[i]);
        }
        auto outs = tr.module->getOutputs(inputs);
        for (int j = 0; j < tr.outputs.size(); j++) {
            results[tr.outputs[j]] = outs[j];
        }
    }

    return results.values();
}

QString cl::TransformationSet::toString() const
{
    QString ret = "";

    for (const cl::Transformation &tr: *this) {
        for (int i : tr.inputs) {
            ret += QString::number(i);
        }

        ret += " -> " + tr.module->name() + " -> ";

        for (int i : tr.outputs) {
            ret += QString::number(i);
        }
        ret += " ";
    }

    return ret;
}

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

//    for (const TransformationSet &key : results.keys()) {
//        qDebug() << key;
//        qDebug() << results[key].size() << " matches";
//    }

    //Get the best results possibles

    auto winners = this->winners();

    /* Try to merge multiple "constant functions" into a bigger one */
    trimResults(winners);
    mergeTargets(winners);
}

void CliqueModuleIntelligence::collate()
{
    qDebug() << "Collating...";

    /* Todo: find way of collating different winners by testing the input and going in the right direction */
    updateResults(); //in case of merged targets, rerun the results with new modules

    auto winners = this->winners();
    trimResults(winners);//necessary?

    /* Todo: find a network that can test which one */
    QList<QSet<int> > inputs;
    for (const auto &winner : winners) {
        inputs.push_back(results[winner]);
    }

    if (winners.size() == 1 && winners[0].size() == 1) {
        return; //abosutely nothing to do
    }

    QList<CliqueModule *> modules;
    /* First merge winners into one module */
    for (const auto &winner : winners) {
        CliqueModule *module = winner.createModule();
        addAuxiliaryModule(module);
        modules.push_back(module);
        newModules.push_back(module);
    }

    /* No need to collate inputs if only one solution */
    if (inputs.size() == 1) {
        return;
    }

    //We now have our sets of inputs, and corresponding modules
    //How to create a test function that separates them well?

    /* Check similarities between the least-known group */
    Classifier cl(this);
    auto protos = cl.classify(inputs);

    TestModule *test = new TestModule("test"+QString::number(auxiliaryModules.size()));
    test->setCharacteristics(protos, modules);

    addAuxiliaryModule(test);
    newModules.push_back(test);

    //debug
    for (int i = 0; i < dataset.size(); i++) {
        qDebug() << toInt(convert.words(getInput(i))) << toInt(convert.words(test->getOutputs(getInput(i))));
    }
}

QList<Clique> CliqueModuleIntelligence::test(const QList<Clique> &input)
{
    auto module = newModules.back();

    qDebug() << module->name();

    return module->getOutputs(input);
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

    QSet<TransformationSet> all = results.keys().toSet();

    int i = 0;
    while (indexes.size() > 0 && all.size() > 0) {
        TransformationSet winner;
        int max = 0;
        int maxSize = 0; //the best transformation is the simplest one, i.e. the one with the least elements.
        i++;

        for (const TransformationSet &key : all) {
            int count = results[key].intersect(indexes).count();

            if (count > max || (count == max && key.count() < maxSize)) {
                max = count;
                maxSize = key.count();
                winner = key;
            }
        }

        assert(max > 0);

        qDebug() << "Winner #" << i << " " << winner << ": " << results[winner].size();

        indexes.subtract(results[winner]);
        all.remove(winner);

        for (int remaining : indexes) {
            const auto &in = dataset[remaining].first;
            const auto &out = dataset[remaining].second;
            qDebug() << toInt(convert.words(in)) << " -> " << toInt(convert.words(out)) << " -- " << toInt(convert.words(winner.transform(in)));
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

 const CliqueModuleIntelligence::input & CliqueModuleIntelligence::getInput (int index) const
 {
     return dataset[index].first;
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
    std::deque<int> outputsRemaining;
    for (int i = 0; i < out.size(); i++) {
        outputsRemaining.push_back(i);
    }

    //qDebug() << convert.words(in) << " -> " << convert.words(out);

    std::function<void(std::deque<int>, const TransformationSet&)> rec
            = [&rec, &sets, index, &out, &in, this] (std::deque<int> rem, const TransformationSet &current) {
        if (rem.size() == 0) {
            results[current].insert(index);
            //qDebug() << current;
            return;
        }

        std::sort(rem.begin(), rem.end());
        int firstOutput = rem.front();
        rem.erase(rem.begin());

        for (CliqueModule *module : auxiliaryModules) {
            QList<Transformation> transformations = module->getCombinationInputs(in, out, firstOutput, rem);

            for (const auto &transformation : transformations) {
                TransformationSet alt = current;
                alt.push_back(transformation);

                auto set = rem;

                //remove used outputs from the destinatoin
                for (int output : transformation.outputs) {
                    auto it = std::find(set.begin(), set.end(), output);

                    assert(it != set.end() || output == firstOutput);

                    if (it != set.end()) {
                        set.erase(it);
                    }
                }

                rec(set, alt);
            }
        }
    };

    rec(outputsRemaining, TransformationSet());
}
