#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>

using namespace std;

// Structure to store counts for each symbol
struct SymbolData
{
    int hits;
    int misses;
};

class CopyModel
{
public:
    CopyModel(int k, double threshold) : k(k), threshold(threshold) {}

    // Update model counts for a symbol
    void update(int symbol)
    {
        auto it = counts.find(symbol);
        if (it == counts.end())
        {
            counts[symbol] = {0, 0};
        }
        if (symbol == predicted_symbol)
        {
            counts[symbol].hits++;
        }
        else
        {
            counts[symbol].misses++;
            reposition_pointer();
        }
    }

    // Calculate probability of a symbol
    double get_probability(int symbol)
    {
        auto it = counts.find(symbol);
        if (it == counts.end())
        {
            return 0.5; // Apply smoothing for unseen symbols
        }
        int total = it->second.hits + it->second.misses;
        return (double)(it->second.hits + alpha) / (total + 2 * alpha);
    }

    // Estimate information needed to represent a symbol
    double get_information(int symbol)
    {
        return -log2(get_probability(symbol));
    }

    // Reposition pointer if performance falls below threshold
    void reposition_pointer()
    {
        string search_string;
        for (int i = k - 1; i >= 0; i--)
        {
            search_string += (char)predicted_symbol;
        }
        int new_position = find_previous_occurrence(search_string);
        if (new_position != -1)
        {
            predicted_symbol = encoded_data[new_position];
        }
    }

    // Find the previous occurrence of a string in encoded data
    int find_previous_occurrence(const string &search_string)
    {
        for (int i = encoded_data.size() - k; i >= 0; i--)
        {
            bool match = true;
            for (int j = 0; j < k; j++)
            {
                if (encoded_data[i + j] != search_string[j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                return i;
            }
        }
        return -1;
    }

    // Member variables
    int k;                                 // Lookback window size
    double threshold;                      // Performance threshold
    double alpha = 1.0;                    // Smoothing parameter
    int predicted_symbol = -1;             // Currently predicted symbol
    vector<int> encoded_data;              // Encoded data
    unordered_map<int, SymbolData> counts; // Symbol counts
};

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Usage: cpm <input_file> <k> <threshold> <output_file>" << endl;
        return 1;
    }

    int k = stoi(argv[2]);
    double threshold = stod(argv[3]);

    // Read input file
    ifstream input(argv[1], ios::binary);
    if (!input.is_open())
    {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    CopyModel model(k, threshold);
    int symbol;
    while (input >> noskipws >> symbol)
    {
        model.update(symbol);
        model.encoded_data.push_back(symbol);
    }
    input.close();

    // Calculate total information and average information per symbol
    double total_information = 0.0;
    for (int symbol : model.encoded_data)
    {
        total_information += model.get_information(symbol);
    }
    double average_information = total_information / model.encoded_data.size();

    // Write compressed data (optional, implementation not provided)
    // ...

    // Print results
    cout << "Estimated total bits: " << total_information << endl;
    cout << "Average bits per symbol: " << average_information << endl;

    return 0;
}
