#include <cmath>
#include <set>
#include <cstring>
#include <unordered_map>
#include <iostream>

#include "cpm.h"

using namespace std;

struct Alphabet
{

    void add(char c)
    {
        alphabet.insert(c);
    }

    int size()
    {
        return alphabet.size();
    }

    void loadCharMap()
    {
        for (char c : alphabet)
        {
            charMap[c] = 0;
        }
        this->charMap = charMap;
    }

    unordered_map<char, double> getCharMap()
    {
        return charMap;
    }

    set<char> alphabet;
    unordered_map<char, double> charMap;
};

struct CopyModel
{

    string kmer;
    char prediction = 0;
    double nHits = 0;
    int nTries = 0;
    int minTries;
    double threshold;
    int alphabetSize;

    CopyModel(){
        this->threshold = 0;
        this->minTries = 0;
        this->alphabetSize = 0;
    }

    CopyModel(double threshold, int minTries, int alphabetSize)
    {
        this->threshold = threshold;
        this->minTries = minTries;
        this->alphabetSize = alphabetSize;
    }

    double calcProb()
    {
        return (nHits + ALPHA) / (nTries + ALPHA * 2);
    }

    double calcBits()
    {
        return -log2(calcProb());
    }

    bool thresholdReached()
    {
        return nTries > minTries && nHits / nTries < threshold;
    }

    void addKmer(string kmer, char prediction)
    {
        this->kmer = kmer;
        this->prediction = prediction;
        this->nHits = 0;
        this->nTries = 0;
    }

    void resetModel()
    {
        this->kmer.clear();
        this->prediction = 0;
        this->nHits = 0;
        this->nTries = 0;
    }

    bool match(string kmer)
    {
        return !this->kmer.empty() && this->kmer == kmer;
    }

    bool isNull()
    {
        return this->kmer.empty();
    }

    double predict(char prediction)
    {
        if (prediction == this->prediction)
        {
            this->nHits++;
            this->nTries++;
            return calcBits();
        }
        else
        {
            this->nTries++;
            return -log2((1-calcProb()) / (alphabetSize-1));
        }
    }
};

struct FallbackModel
{

    int k;
    Alphabet alphabet;

    FallbackModel(int k, Alphabet alphabet)
    {
        this->k = k;
        this->alphabet = alphabet;
    }

    double calcBits(string kmer)
    {
        // calc relative frequency of each char in the kmer
        int i;
        unordered_map<char, double> counts = this->alphabet.getCharMap();
        for (i = 0; i < k; i++)
        {
            counts[kmer[i]]++;
        }
        // calc probability
        double prob = 0;
        for (auto const &[c, count] : counts)
        {
            if (count == 0)
            {
                continue;
            }
            prob += -log2(count / k) * (count / k);
        }
        return prob;
    }
};