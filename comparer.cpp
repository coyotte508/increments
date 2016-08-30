#include "comparer.h"

Comparer::Comparer()
{

}

void Comparer::addComparison(Clique A, Clique B, CompResult Result)
{
    Comparison cmp = {A,B,Result};
    comps.push_back(cmp);
    compsByCl[A].insert(cmp);
    compsByCl[B].insert(cmp);
}
