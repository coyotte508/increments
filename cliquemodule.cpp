#include "cliquemodule.h"

CliqueModule::CliqueModule()
{

}

void CliqueModule::addInputNetwork(CliqueNetwork *nw)
{
    addNetwork(nw);
    inputs.push_back(nw);
}

void CliqueModule::addOutputNetwork(CliqueNetwork *nw)
{
    addNetwork(nw);
    outputs.push_back(nw);
}

Clique CliqueModule::getOutput(const Clique &input)
{
    /* No safety checks done. Assumes reponsible caller */
    return CliqueNetworkManager::getOutput(inputs.first(), outputs.first(), input);
}

QList<Clique> CliqueModule::getOutputs(const QList<Clique> &inputs)
{
    QList<Clique> ret;

    shutdown();

    for (int i = 0; i < inputs.size(); i++) {
        this->inputs[i]->activateClique(inputs[i]);
    }
    iterate();
    for (auto output : outputs) {
        ret.push_back(output->activatedClique());
    }

    return ret;
}

void CliqueModule::linkInputOutput(const Clique &input, const Clique &output)
{
    inputs.first()->linkClique(input, outputs.first(), output);
}
