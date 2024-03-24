#include <cmath>
#include <set>
#include <cstring>
#include <unordered_map>
#include <iostream>

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

    set<char> alphabet;
};

struct CopyModel
{

    string kmer;
    double alpha = 0;
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

    CopyModel(double threshold, int minTries, int alphabetSize, double alpha)
    {
        this->threshold = threshold;
        this->minTries = minTries;
        this->alphabetSize = alphabetSize;
        this->alpha = alpha;
    }

    double calcProb()
    {
        return (nHits + alpha) / (nTries + alpha * 2);
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
    char *data;
    int k;
    Alphabet alphabet;
    unordered_map<char, double> counts;

    FallbackModel(char *&data, int k, Alphabet alphabet)
    {
        this->data = data;
        this->k = k;
        this->alphabet = alphabet;
        for (auto &c : alphabet.alphabet)
        {
            counts[c] = 0;
        }
    }

    void advancePosition(int i)
    {
        if (i > 199)
        {
            counts[data[i - 200]]--;
            counts[data[i]]++;
        }
        else
        {
            counts[data[i]]++;
        }
    }

    double calcBits(int i)
    {
        int size = 200;
        if (i < 199)
        {
            size = i+1;
        }
        
        double bits = 0;
        for (auto &c : counts)
        {
            double p = c.second / size;
            if (p > 0)
            {
                bits -= p * log2(p);
            }
        }
        return bits;
    }
};