#include <QCoreApplication>
#include <iostream>
#include "cliquenetwork.h"
#include "converter.h"
#include "cliquenetworkmanager.h"

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

    CliqueNetworkManager mg;

    CliqueNetwork *nw1 = new CliqueNetwork(nw);
    CliqueNetwork *nw2 = new CliqueNetwork(nw);
//    CliqueNetwork &nw3 = copies[2];
//    CliqueNetwork &nw4 = copies[3];
//    CliqueNetwork &nw5 = copies[4];

    mg.addNetwork(*nw1);
    mg.addNetwork(*nw2);

    for (int i = 0; i < 9; i++) {
        nw1->linkClique(convert.clique(i), nw2, convert.clique(i+1));
    }

    cout << "Link of " << 2 << ": " << convert.word(mg.getOutput(*nw1, *nw2, convert.clique(2))).toInt() << endl;

    return 0;
}
