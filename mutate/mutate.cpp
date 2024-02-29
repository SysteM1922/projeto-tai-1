#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>
#include <ctime> // Include the <ctime> header for time functions

//define namespace std 
using namespace std;

string mutate(const string &sequence, const map<char, double> &probs)
{
    string mutated_sequence = "";
    for (char l : sequence)
    {
        if (rand() / (RAND_MAX + 1.0) < probs.at(l))
        {
            vector<char> keys;
            for (const auto &pair : probs)
            {
                if (pair.first != l)
                {
                    keys.push_back(pair.first);
                }
            }
            mutated_sequence += keys[rand() % keys.size()];
        }
        else
        {
            mutated_sequence += l;
        }
    }
    return mutated_sequence;
}

int main()
{
    map<char, double> probs = {{'A', 0.25}, {'C', 0.25}, {'G', 0.25}, {'T', 0.25}};

    srand(static_cast<unsigned int>(time(nullptr)));

    // Read the input sequence
    ifstream infile("data.txt");
    string sequence;
    infile >> sequence;
    infile.close();

    // Convert to uppercase
    for (char &c : sequence)
    {
        c = toupper(c);
    }

    cout << "Original sequence: " << sequence << endl;

    for (char l : sequence)
    {
        if (probs.find(l) == probs.end())
        {
            probs[l] = 0.0;
        }
    }

    // Mutate the sequence
    string mutated_sequence = mutate(sequence, probs);

    cout << "Mutated sequence: " << mutated_sequence << endl;

    return 0;
}
