#include <unordered_map>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>

using namespace std;

struct Value
{
    int idx = 0;
    vector<char> values;

    Value() {}

    Value(char value)
    {
        values.push_back(value);
    }

    void addValue(char value)
    {
        values.push_back(value);
    }

    int getSize()
    {
        return values.size();
    }

    char getCurrentElement()
    {
        if (idx >= static_cast<int>(values.size()))
        {
            return -1;
        }
        return values[idx];
    }

    vector<char> getValues()
    {
        return values;
    }

    void advancePosition()
    {
        idx++;
    }
};

struct Table
{
    // map char array to Value
    unordered_map<string, Value> table;
    int k;

    Table(int k)
    {
        this->k = k;
    }

    void insert(string key, char value)
    {
        if (table.find(key) == table.end())
        {
            table[key] = Value(value);
        }
        else
        {
            table[key].addValue(value);
        }
    }

    bool contains(string key)
    {
        return table.find(key) != table.end();
    }

    void advancePosition(string key)
    {
        table[key].advancePosition();
    }

    char getCurrentElement(string kmer)
    {
        return table[kmer].getCurrentElement();
    }
};