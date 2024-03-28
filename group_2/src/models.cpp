#include <cmath>
#include <set>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <string>

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

struct Reference
{
    string kmer;
    char prediction = 0;
    double nHits = 0;
    int nTries = 0;
    double alpha;
    double threshold;
    int minTries;
    bool activated = true;
    int alphabetSize;
    int goal;
    int k;

    Reference() {}

    Reference(string kmer, double threshold, int minTries, int alphabetSize, double alpha, char prediction, int goal, int k)
    {
        this->kmer = kmer;
        this->threshold = threshold;
        this->minTries = minTries;
        this->alphabetSize = alphabetSize;
        this->alpha = alpha;
        this->prediction = prediction;
        this->goal = goal;
        this->k = k;
    }

    double calcProb()
    {
        return (nHits + alpha) / (nTries + alpha * 2);
    }

    bool thresholdReached()
    {
        return nTries > minTries && nHits / nTries < threshold;
    }

    bool isActive()
    {
        return activated;
    }

    void deactivate()
    {
        this->activated = false;
    }

    double predict(char prediction)
    {
        this->nTries++;
        if (prediction == this->prediction)
        {
            this->nHits++;
            return calcProb();
        }
        return (1 - calcProb()) / (this->alphabetSize - 1);
    }

    bool match(string kmer)
    {
        for (int i = 0; i < k - goal + 1; i++)
        {
            if (this->kmer.find(kmer.substr(i, goal)) != string::npos)
            {
                return true;
            }
        }
        return false;
    }

    bool isNull()
    {
        return this->kmer.empty();
    }
};

struct CopyModel
{
    unsigned long int n;
    vector<Reference> references;
    double alpha = 0;
    int alphabetSize = 0;
    int minTries = 0;
    double threshold = 0;
    int goal = 0;
    int k = 0;

    CopyModel() {}

    CopyModel(double threshold, int minTries, int alphabetSize, double alpha, int n, int goal, int k)
    {
        this->threshold = threshold;
        this->minTries = minTries;
        this->alphabetSize = alphabetSize;
        this->alpha = alpha;
        this->n = n;
        this->goal = goal;
        this->k = k;
    }

    void addReference(string kmer, char prediction)
    {
        this->references.push_back(Reference(kmer, threshold, minTries, alphabetSize, alpha, prediction, goal, k));
    }

    void resetModel()
    {
        this->references.clear();
    }

    bool isNull()
    {
        for (Reference ref : references)
        {
            if (!ref.isNull())
            {
                return false;
            }
        }
        return true;
    }

    bool match(string kmer)
    {
        for (Reference ref : references)
        {
            if (ref.isActive() && ref.match(kmer))
            {
                return true;
            }
        }
        return false;
    }

    bool isActive()
    {
        bool ret = false;
        for (Reference &ref : references)
        {
            if (ref.isActive())
            {
                if (ref.thresholdReached())
                {
                    ref.deactivate();
                    continue;
                }
                ret = true;
            }
        }
        return ret;
    };

    double predict(char prediction, string kmer)
    {
        double prob = 0;
        int size = 0;
        for (Reference &ref : this->references)
        {
            if (ref.isActive() && ref.match(kmer))
            {
                prob += ref.predict(prediction);
                size++;
            }
        }
        return -log2(prob / size);
    }

    bool isFull()
    {
        return this->references.size() >= n;
    }
};

struct FallbackModel
{
    char *data;
    int k;
    int size;
    Alphabet alphabet;
    unordered_map<char, double> counts;

    FallbackModel(char *&data, int k, Alphabet alphabet, int size)
    {
        this->data = data;
        this->k = k;
        this->alphabet = alphabet;
        this->size = size;
        for (auto &c : alphabet.alphabet)
        {
            counts[c] = 0;
        }
    }

    void advancePosition(int i)
    {
        if (i > size - 1)
        {
            counts[data[i - size]]--;
            counts[data[i]]++;
        }
        else
        {
            counts[data[i]]++;
        }
    }

    double calcBits(int i)
    {
        if (i < size - 1)
        {
            size = i + 1;
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