#include "cliquemoduleintelligence.h"

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
    //magic
}
