#include <map>
#include <vector>

using namespace std;

struct Value
{
    int idx = 0;
    vector<int> values;

    Value() {}

    Value(int value)
    {
        values.push_back(value);
    }

    void addValue(int value)
    {
        values.push_back(value);
    }

    int getSize()
    {
        return values.size();
    }

    int getPosition()
    {
        if (idx >= static_cast<int>(values.size()))
        {
            return -1;
        }
        return values[idx];
    }

    vector<int> getValues()
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
    map<char *, Value> table;

    void insert(char *key, int value)
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

    bool contains(char *key)
    {
        return table.find(key) != table.end();
    }

    void advancePosition(char *key)
    {
        table[key].advancePosition();
    }

    int getPosition(char *key)
    {
        return table[key].getPosition();
    }
};