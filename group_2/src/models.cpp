#include <cmath>
#include <set>

#include "cpm.h"

using namespace std;

struct Alphabet {

    void add(char c) {
        alphabet.insert(c);
    }
    
    set<char> alphabet;
};

struct RepeatModel {
    RepeatModel(int pos, double alpha) : pos(pos) {}

    double calcProb() {
        return (nHits + ALPHA) / (nTries + ALPHA * 2);
    }

    int calcBits() {
        return -log2(calcProb());
    }

    int pos;
    int nHits = 0;
    int nTries = 0;
};