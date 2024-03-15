#include <map>
#include <vector>

using namespace std;

struct Value
{
    int position;
    vector<int> values;

    Value(int position, int value) : position(position)
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
        if (position >= static_cast<int>(values.size()))
        {
            return -1;
        }
        return values[position];
    }

    vector<int> getValues()
    {
        return values;
    }

    void advancePosition()
    {
        position++;
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
            table[key] = Value(0, value);
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
};