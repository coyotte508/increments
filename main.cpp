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

    QList<CliqueNetwork> copies;

    for (int i = 0; i < 50; i++) {
        copies.push_back(nw);
    }

    CliqueNetworkManager mg;

    CliqueNetwork &nw1 = copies[0];
    CliqueNetwork &nw2 = copies[1];
//    CliqueNetwork &nw3 = copies[2];
//    CliqueNetwork &nw4 = copies[3];
//    CliqueNetwork &nw5 = copies[4];

    mg.addNetwork(nw1);
    mg.addNetwork(nw2);

    for (int i = 0; i < 10; i++) {
        nw1.linkClique(convert.word(i), &nw2, convert.word((i+1)%10));
    }

    return 0;
}
