#include <map>
#include <vector>

using namespace std;

struct Value
{
    unsigned int position;
    vector<unsigned int> values;

    Value(unsigned int position, unsigned int value)
    {
        this->position = position;
        this->values.push_back(value);
    }

    void addValue(unsigned int value)
    {
        values.push_back(value);
    }
    
    int getSize()
    {
        return values.size();
    }

    unsigned int getPosition()
    {
        if (position >= values.size())
        {
            return -1;
        }
        return values[position];
    }

    vector<unsigned int> getValues()
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
    map<char*, Value> table;

    void insert(char* key, unsigned int value)
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

    bool contains(char* key)
    {
        return table.find(key) != table.end();
    }

    void advancePosition(char* key)
    {
        table[key].advancePosition();
    }

};