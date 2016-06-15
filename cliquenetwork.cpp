#include <QVector>


#include "utils.h"
#include "cliquenetwork.h"

using namespace cl;

CliqueNetwork::CliqueNetwork()
{

}

CliqueNetwork::CliqueNetwork(const CliqueNetwork &other)
{
    this->l = other.l;
    this->c = other.c;
    this->cliques = other.cliques;
    this->network = other.network;
    /* Don't copy temporary data or links to exterior networks */
}

void CliqueNetwork::init(int clusters, int l)
{
    network.clear();
    cliques.clear();
    for (int i = 0; i < clusters; i++) {
        network.push_back(cluster());
    }

    this->l = l;
    this->c = clusters;
}

void CliqueNetwork::copyAdd(const CliqueNetwork &other)
{
    if (!l) {
        init(other.c, other.l);
    }

    cliques.unite(other.cliques);

    for (int i = 0; i < other.network.size(); i++) {
        for (int j : other.network[i].keys()) {
            network[i][j].unite(other.network[i][j]);
        }
    }
}

void CliqueNetwork::addClique(const Clique &c)
{
    for (int i = 0; i < c.size(); i++) {
        for (int j = 0; j < c.size(); j++) {
            addLink(coord(i, c[i]), coord(j, c[j]));
        }
    }
    cliques.insert(c);
}

void CliqueNetwork::removeClique(const Clique &c, bool update)
{
    if (!cliques.contains(c)) {
        return;
    }

    cliques.remove(c);

    if (update) {
        recliques();
    }
}

void CliqueNetwork::recliques()
{
    for (int i = 0; i < network.size(); i++) {
        network[i] = cluster();
    }

    for (const auto &cl : cliques) {
        addClique(cl);
    }
}

bool CliqueNetwork::isFullyConnectedTo(const Clique &input, CliqueNetwork *dest, const Clique &output) const
{
    for (int i = 0; i < input.size(); i++) {
        for (int j = 0; j < output.size(); j++) {
            if (!extraLinks.value(cl::coord(i, input[i])).contains(destlink(dest, cl::coord(j, output[j])))) {
                return false;
            }
        }
    }

    return true;
}

void CliqueNetwork::addLink(const coord &src, const coord &dest)
{
    network[src.first][src.second].insert(dest);
}

void CliqueNetwork::addLink(const coord &src, CliqueNetwork *nw, const coord &dest)
{
    extraLinks[src].insert(QPair<CliqueNetwork*, cl::coord>(nw, dest));
}

void CliqueNetwork::buildIdentity(CliqueNetwork *other)
{
    for (const auto &cl : cliques) {
        linkClique(cl, other, cl);
    }
}

void CliqueNetwork::linkClique(const Clique &c, CliqueNetwork *nw, const Clique &c2)
{
    for (int i = 0; i < c.length(); i++) {
        cl::coord ori(i, c[i]);

        for (int j = 0; j < c.length(); j++) {
            addLink(ori, nw, cl::coord(j, c2[i]));
        }
    }
}

Clique CliqueNetwork::randomClique() const
{
    Clique ret;

    std::uniform_int_distribution<> f(0,l-1);
    for (int i = 0; i < c; i++) {
        ret.push_back(f(rng()));
    }

    return ret;
}

Clique CliqueNetwork::activatedClique() const
{
    return activated;
}

int CliqueNetwork::cliqueSize() const
{
    return c;
}

void CliqueNetwork::activateClique(const Clique &c)
{
    activated = c;
}

void CliqueNetwork::stimulate(const coord &dest)
{
    stimulated[dest] += 1;
}

void CliqueNetwork::propagate()
{
    //intra links
    for (int i = 0; i < activated.size(); i++) {
        for(const cl::coord &dest : network[i].value(activated[i])) {
            stimulated[dest] += 1;
        }
    }
    //extra links
    for (int i = 0; i < activated.size(); i++) {
        for (const auto &pair: extraLinks.value(cl::coord(i, activated[i]))) {
            pair.first->stimulate(pair.second);
        }
    }
}

void CliqueNetwork::stabilize()
{
    //WTA
    QMap<int, int> maxCl;

    // find max stimulation for each cluster
    for(auto it = stimulated.begin(); it != stimulated.end(); ++it) {
        if (maxCl[it.key().first] < it.value()) {
            maxCl[it.key().first] = it.value();
        }
    }

    //clear activated
    activated = QVector<int>(c, 0).toList();

    //If neuron has same stimulation as max, activate it
    for (auto it = stimulated.begin(); it != stimulated.end(); ++it) {
        if (maxCl[it.key().first] == it.value()) {
            activated[it.key().first] = it.key().second;
        }
    }

    stimulated.clear();
}

void CliqueNetwork::shutdown()
{
    stimulated.clear();
    activated.clear();
}
