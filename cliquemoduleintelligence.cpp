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
