#include <QCoreApplication>
#include <iostream>
#include "cliquenetwork.h"

using namespace std;

int x(int *&y) {
    y = new int(1);
    return 0;
}

int main(/*int argc, char *argv[]*/)
{
    int *y;
    x(y);
    cout << *y << endl;
    return 0;

    CliqueNetwork nw;
    nw.init(8,256);

    QList<Clique> numberCliques;

    for (int i = 0; i < 10; i++) {
        auto cl = nw.randomClique();
        numberCliques.push_back(cl);
        nw.addClique(cl);
    }

    QList<CliqueNetwork> copies;

    for (int i = 0; i < 50; i++) {
        copies.push_back(nw);
    }

    return 0;
}
