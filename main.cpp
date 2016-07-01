#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include <cassert>
#include <sstream>
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

Converter convert;

string testCommands = string("0123456789\n")
        + "1\n1\n"
        + "adds 0123456789\n"
        + "learn\nnext\n2\n2\n"
        + "add 01 02\nadd 32 33\nadd 29 30\nadd 49 50\nadd 68 69\nadd 13 14\n"
        + "learn\nnext\n3\n3\n"
        + "add 001 002\nadd 023 024\nadd 136 137\nadd 399 400\nadd 259 260\nadd 699 700\nadd 456 457";
stringstream stream(testCommands);

std::istream & gin() {
    if (stream && !stream.eof()) {
        return stream;
    } else {
        return cin;
    }
}

int main(/*int argc, char *argv[]*/)
{
    qInstallMessageHandler(myMessageOutput);
    CliqueNetwork nw;
    nw.init(8,256);

    QList<Clique> numberCliques;

    string alphabet;

    cout << "Type whole alphabet: ";
    gin() >> alphabet;

    for (char c: alphabet) {
        auto cl = nw.randomClique();
        numberCliques.push_back(cl);
        nw.addClique(cl);
        convert.learnWord(QString(1, c), cl);
    }

    CliqueModule mod;
    mod.setOwnership(true);

    CliqueNetwork *nw1 = new CliqueNetwork(nw);
    CliqueNetwork *nw2 = new CliqueNetwork(nw);

    mod.addInputNetwork(nw1);
    mod.addOutputNetwork(nw2);

    CliqueModule identity(mod);
    identity.setName("identity");
    identity.buildIdentity();

    CliqueModuleIntelligence intel;
    intel.addAuxiliaryModule(&identity);

    for (char c: alphabet) {
        CliqueModule *target = new CliqueModule(mod);
        target->setName("target" + QString(QChar(c)));
        target->buildTarget(convert.clique(QString(1, c)));
        intel.addAuxiliaryModule(target);
    }

    while (1) {
        cout << "Input Size: " ;
        int inSize;
        gin() >> inSize;

        cout << "Ouput Size: ";
        int outSize;
        gin() >> outSize;

        cout << "In, Out: " << inSize << ", " << outSize << endl;

        CliqueModule base;
        base.setOwnership(true);

        for (int i = 0; i < inSize; i++) {
            base.addInputNetwork( new CliqueNetwork(nw) );
        }
        for (int i = 0; i < outSize; i++) {
            base.addOutputNetwork( new CliqueNetwork(nw) );
        }

        intel.clearData();
        intel.setBaseModel(&base);

        cout << "- add xx yy: to add input / output" << endl;
        cout << "- learn: to assimilate results" << endl;
        cout << "- test xx: to get result" << endl;
        cout << "- next: to start learning again" << endl;


        while (1) {
            string s;
            gin() >> s;

            if (s == "add") {
                string in, out;

                gin() >> in >> out;

                assert(in.size() == inSize);
                assert(out.size() == outSize);

                QList<Clique> inC, outC;
                for (char c: in) {
                    inC << convert.clique(QString(1, c));
                }
                for (char c: out) {
                    outC << convert.clique(QString(1, c));
                }
                intel.addInputOutput(inC, outC);
            } else if (s == "adds") {
                assert(inSize == outSize && inSize == 1);

                string data;
                gin() >> data;

                for (int i = 0; i +1 < data.size(); i++) {
                    intel.addInputOutput(QList<Clique>() << convert.clique(QString(1, data[i])), QList<Clique>() << convert.clique(QString(1, data[i+1])));
                }
            } else if (s == "learn") {
                intel.resolve();
                intel.collate();
            } else if (s == "test") {
                string test;
                gin() >> test;

                assert(test.size() == inSize);

                qDebug() << "testing " << QString::fromStdString(test);
                QList<Clique> inC;
                for (char c: test) {
                    inC << convert.clique(QString(1, c));
                }
                qDebug() << toInt(convert.words(intel.test(inC)));
            } else if (s == "next") {

                break;
            } else {
                cout << "Unknown command: " << s << endl;
                return 1;
            }
        }
    }

    return 0;
}
