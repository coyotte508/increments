#include <QCoreApplication>
#include <iostream>
#include "cliquenetwork.h"
#include "converter.h"
#include "cliquenetworkmanager.h"
#include "cliquemodule.h"

using namespace std;

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

    return 0;
}
