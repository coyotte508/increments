#ifndef CLIQUEMODULEINTELLIGENCE_H
#define CLIQUEMODULEINTELLIGENCE_H

#include <QList>

#include "cliquemodule.h"

class CliqueModuleIntelligence {
public:
    typedef QList<Clique> input;
    typedef QList<Clique> output;
    typedef QPair<input, output> inputoutput;

    CliqueModuleIntelligence();

    void setBaseModel(CliqueModule *mod);
    void addAuxiliaryModule(CliqueModule *mod);
    void addInputOutput(const input &in, const output &out);

    void resolve();
private:
    QList<inputoutput> dataset;
    CliqueModule base;

    QSet<CliqueModule *> auxiliaryModules;
};

#endif
