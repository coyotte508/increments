#include <functional>
#include "cliquemoduleintelligence.h"
#include "converter.h"
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
