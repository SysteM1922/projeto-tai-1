#include <cmath>
#include <set>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include <vector>

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
    char prediction = 0;
    double nHits = 0;
    int nTries = 0;
    double alpha = 0;
    double threshold = 0;
    int minTries = 0;
    bool activated = true;
    int alphabetSize = 0;

    Reference() {}

    Reference(double threshold, int minTries, int alphabetSize, double alpha, int k, char prediction)
    {
        this->threshold = threshold;
        this->minTries = minTries;
        this->alphabetSize = alphabetSize;
        this->alpha = alpha;
        this->prediction = prediction;
    }

    double calcProb()
    {
        return (nHits + alpha) / (nTries + alpha * 2);
    }

    bool thresholdReached()
    {
        return nTries > minTries && nHits / nTries < threshold;
    }

    bool isActivate()
    {
        return activated;
    }

    void deactivate()
    {
        activated = false;
    }

    double predict(char prediction)
    {
        if (prediction == this->prediction)
        {
            this->nHits++;
            this->nTries++;
            return calcProb();
        }
        else
        {
            this->nTries++;
            return (1 - calcProb()) / (this->alphabetSize - 1);
        }
    }
};

struct CopyModel
{
    string kmer;
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
        this->references = vector<Reference>();
        this->n = n;
        this->goal = goal;
        this->k = k;
    }

    void newReferences(string kmer, char prediction)
    {
        this->kmer = kmer;
        addReference(prediction);
    }

    void addReference(char prediction)
    {
        this->references.push_back(Reference(threshold, minTries, alphabetSize, alpha, kmer.size(), prediction));
    }

    void resetModel()
    {
        this->kmer.clear();
        this->references.clear();
    }

    bool match(string kmer)
    {
        int match = 0;
        unsigned long int i, j, l;
        unsigned long int goal = this->goal;
        for (i = 0; i < k - goal; i++)
        {
            for (j = i; j < k - goal; j++)
            {
                if (kmer[i] == this->kmer[j])
                {
                    match++;
                    for (l = 1; l < goal; l++)
                    {
                        if (kmer[i + l] != this->kmer[j + l])
                        {
                            break;
                        }
                        match++;
                        if (match == this->goal)
                        {
                            return true;
                        }
                    }
                }
                else
                {
                    match = 0;
                }
            }
            match = 0;
        }
        return false;
    }

    bool isNull()
    {
        return this->kmer.empty();
    }

    bool isActive()
    {
        bool ret = false;
        for (auto ref : references)
        {
            if (ref.isActivate() && ref.thresholdReached())
            {
                ref.deactivate();
                continue;
            }
            ret = true;
        }
        return ret;
    };

    double predict(char prediction)
    {
        double prob = 0;
        for (auto ref : references)
        {
            if (ref.isActivate())
            {
                prob += ref.predict(prediction);
            }
        }
        return -log2(prob / references.size());
    }

    bool isFull()
    {
        return references.size() == n;
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