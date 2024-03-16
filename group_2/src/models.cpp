#include <cmath>
#include <set>
#include <cstring>
#include <map>
#include <iostream>

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
        for (char c : alphabet) {
            charMap[c] = 0;
        }
        this->charMap = charMap;
    }

    map<char, double> getCharMap() {
        return charMap;
    }
    
    set<char> alphabet;
    map<char, double> charMap;
};

struct CopyModel {

    char *kmer = NULL;
    char prediction = 0;
    double nHits = 0;
    int nTries = 0;
    int minTries;
    double threshold;

    CopyModel(double threshold, int minTries) {
        this->threshold = threshold;
        this->minTries = minTries;
    }

    double calcProb() {
        return (nHits + ALPHA) / (nTries + ALPHA * 2);
    }

    double calcBits() {
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
        this->prediction = 0;
        this->nHits = 0;
        this->nTries = 0;
    }

    bool match(char *kmer) {
        return this->kmer != NULL && strcmp(this->kmer, kmer) == 0;
    }

    bool isNull() {
        return this->kmer == NULL;
    }

    void predict(char prediction) {
        if (prediction == this->prediction) {
            this->nHits++;
        }
        this->nTries++;
    }

};

struct FallbackModel {
    
    int k;
    Alphabet alphabet;

    FallbackModel(int k, Alphabet alphabet) {
        this->k = k;
        this->alphabet = alphabet;
    }

    double calcBits(char *kmer) {
        // calc relative frequency of each char in the kmer
        int i;
        map<char, double> counts = this->alphabet.getCharMap();
        for (i = 0; i < k; i++) {
            counts[kmer[i]]++;
        }
        // calc probability
        double prob = 0;
        for (auto const& [c, count] : counts) {
            if (count == 0) {
                continue;
            }
            prob += -log2(count / k) * (count / k);
        }
        return prob;
    }
};