#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>
#include <ctime> // Include the <ctime> header for time functions


std::string mutate(const std::string &sequence, const std::map<char, double> &probs)
{
    std::string mutated_sequence = "";
    for (char l : sequence)
    {
        if (std::rand() / (RAND_MAX + 1.0) > probs.at(l))
        {
            std::vector<char> keys;
            for (const auto &pair : probs)
            {
                if (pair.first != l)
                {
                    keys.push_back(pair.first);
                }
            }
            mutated_sequence += keys[std::rand() % keys.size()];
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
    std::map<char, double> probs = {{'A', 0.25}, {'C', 0.25}, {'G', 0.25}, {'T', 0.25}};

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Read the input sequence
    std::ifstream infile("data.txt");
    std::string sequence;
    infile >> sequence;
    infile.close();

    // Convert to uppercase
    for (char &c : sequence)
    {
        c = std::toupper(c);
    }

    std::cout << "Original sequence: " << sequence << std::endl;

    for (char l : sequence)
    {
        if (probs.find(l) == probs.end())
        {
            probs[l] = 0.0;
        }
    }

    // Mutate the sequence
    std::string mutated_sequence = mutate(sequence, probs);

    std::cout << "Mutated sequence: " << mutated_sequence << std::endl;

    return 0;
}
