#include <cmath>
#include <set>
#include <cstring>
#include <map>

#include "cpm.h"

using namespace std;

struct Alphabet {

    void add(char c) {
        alphabet.insert(c);
    }

    int size() {
        return alphabet.size();
    }

    void loadCharMap() {
        map<char, int> charMap;
        for (char c : alphabet) {
            charMap[c] = 0;
        }
        this->charMap = charMap;
    }

    map<char, int> getCharMap() {
        return charMap;
    }
    
    set<char> alphabet;
    map<char, int> charMap;
};

struct CopyModel {

    double calcProb() {
        return (nHits + ALPHA) / (nTries + ALPHA * 2);
    }

    int calcBits() {
        return -log2(calcProb());
    }

    bool thresholdReached() {
        return nTries > minTries && nHits / nTries < threshold;
    }

    void addKmer(char *kmer, char prediction) {
        this->kmer = kmer;
        this->prediction = prediction;
        this->nHits = 0;
        this->nTries = 0;
    }

    void resetModel() {
        this->kmer = NULL;
        this->prediction = NULL;
        this->nHits = 0;
        this->nTries = 0;
    }

    bool match(char *kmer, char prediction) {
        if (this->kmer == NULL) {
            addKmer(kmer, prediction);
            return true;
        }
        return strcmp(this->kmer, kmer) == 0;
    }

    void predict(char prediction) {
        if (prediction == this->prediction) {
            nHits++;
        }
        nTries++;
    }

    char *kmer;
    char prediction;
    int nHits = 0;
    int nTries = 0;
    int minTries;
    int threshold;
};

struct FallbackModel {

    FallbackModel(int k, Alphabet alphabet) {
        this->k = k;
        this->alphabet = alphabet;
    }

    double calcBits(char *kmer) {
        // calc relative frequency of each char in the kmer
        int i;
        map<char, int> counts = this->alphabet.getCharMap();
        for (i = 0; i < k; i++) {
            counts[kmer[i]]++;
        }
        // calc probability
        double prob = 0;
        for (auto const& [c, count] : counts) {
            prob += -log2(count / k) * (count / k);
        }
        return prob;
    }

    int k;
    Alphabet alphabet;
};