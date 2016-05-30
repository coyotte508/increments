#include <QCoreApplication>
#include <iostream>
#include "cliquenetwork.h"
#include "converter.h"
#include "cliquenetworkmanager.h"
#include "cliquemodule.h"
#include "cliquemoduleintelligence.h"
#include "utils.h"

using namespace std;

QList<int> dec(int x, int n) {
    QList<int> ret;

    auto s = QByteArray::number(x);

    while (s.length() < n) {
        s = "0" + s;
    }

    for (int i = 0; i < s.length(); i++) {
        ret.push_back(s[i]-'0');
    }

    return ret;
}

int main(/*int argc, char *argv[]*/)
{
    CliqueNetwork nw;
    nw.init(8,256);

    QList<Clique> numberCliques;

    Converter convert;

    for (int i = 0; i < 10; i++) {
        auto cl = nw.randomClique();
        numberCliques.push_back(cl);
        nw.addClique(cl);
        convert.learnWord(i, cl);
    }

    CliqueModule mod;

    CliqueNetwork *nw1 = new CliqueNetwork(nw);
    CliqueNetwork *nw2 = new CliqueNetwork(nw);
//    CliqueNetwork &nw3 = copies[2];
//    CliqueNetwork &nw4 = copies[3];
//    CliqueNetwork &nw5 = copies[4];

    mod.addNetwork(nw1);
    mod.addNetwork(nw2);

    for (int i = 0; i < 9; i++) {
        mod.linkInputOutput(convert.clique(i), convert.clique(i+1));
    }

    cout << "Link of " << 2 << ": " << convert.word(mod.getOutput(convert.clique(2))).toInt() << endl;

    CliqueModule mod2;
    mod2.addInputNetwork( new CliqueNetwork(nw));
    mod2.addInputNetwork( new CliqueNetwork(nw));
    mod2.addOutputNetwork( new CliqueNetwork(nw));
    mod2.addOutputNetwork( new CliqueNetwork(nw));

    std::uniform_int_distribution<> dist(0, 98);

    CliqueModuleIntelligence intel;
    intel.setBaseModel(&mod2);

    for (int i = 0; i < 50; i++) {
        int rnd = dist(e1);
        QList<int> input = dec(rnd, 2);
        QList<int> output = dec(rnd, 2);
        QList<Clique> inputs = {convert.clique(input[0]), convert.clique(input[1])};
        QList<Clique> outputs = {convert.clique(output[0]), convert.clique(output[1])};

        intel.addInputOutput(inputs, outputs);
    }

    intel.addAuxiliaryModule(&mod);

    intel.resolve();

    return 0;
}
