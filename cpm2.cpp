#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include <map>
#include <cmath> // For logarithm function

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " <filename> <windowSize> <threshold>" << endl;
        return 1;
    }

    FILE *file = fopen(argv[1], "r"); // Open the file specified by the first command line argument for reading

    if (file == nullptr) {
        cerr << "Error opening file!" << endl;
        return 1; // Exit with error code
    }

    // Define window size and threshold
    size_t windowSize = atoi(argv[2]);
    int threshold = atoi(argv[3]);

    // Initialize window filled with the first character of the text
    char firstChar = fgetc(file);
    vector<char> window(windowSize, firstChar);

    // Initialize alphabet with the first character 
    vector<char> alphabet(1, firstChar);
    vector<char> data(1, firstChar); 

    int c;
    while ((c = fgetc(file)) != EOF) {
        char ch = static_cast<char>(c);
        // Check if the character is not already in the alphabet vector
        if (find(alphabet.begin(), alphabet.end(), ch) == alphabet.end()) {
            alphabet.push_back(ch);
        }
        data.push_back(ch);
    }

    // Output the initialized window
    cout << "Initialized window: ";
    for (char ch : window) {
        cout << ch;
    }
    cout << endl;

    // Output the full alphabet
    cout << "Full alphabet: ";
    for (char ch : alphabet) {
        cout << ch << " ";
    }
    cout << endl;

    cout << "Data: ";
    for (char ch : data) {
        cout << ch;
    }

    // Close the file
    fclose(file);

    // Initialize probability hash table
    double alpha = 0.1; // Smoothing factor
    map<char, double> probability;
    double totalProb = 0.0;
    for (char ch : alphabet) {
        if (ch == window[0]) {
            probability[ch] = 1.0 - alpha * (alphabet.size() - 1);
        } else {
            probability[ch] = alpha;
            totalProb += alpha;
        }
    }

    // Normalize probabilities to ensure they sum up to 1
    totalProb += 1.0 - alpha * (alphabet.size() - 1);
    for (auto &entry : probability) {
        entry.second /= totalProb;
    }

    // Initialize hits and misses hash tables
    map<char, int> hits, misses;
    for (char ch : alphabet) {
        hits[ch] = 0;
        misses[ch] = 0;
    }

    // Process each symbol in the data
    for (size_t i = 0; i < data.size(); ++i) {
        cout << "\n----------------------------------------------" << endl;
        cout << "Processing symbol at index " << i << " of the data... (tryign to predict: "<< data[i] << ")" << endl;
        cout << "Current window: ";
        for (char ch : window) {
            cout << ch;
        }
        cout << endl;

        // Predict next symbol based on probabilities
        char predictedSymbol = '\0'; // Initialize predicted symbol
        double maxProbability = 0.0; // Initialize maximum probability
        for (auto entry : probability) {
            if (entry.second > maxProbability) {
                maxProbability = entry.second;
                predictedSymbol = entry.first;
            }
        }

        // print the probabilities hash table
        cout << "Probabilities: ";
        for (auto entry : probability) {
            cout << entry.first << ": " << entry.second << " ";
        }
        cout << endl;


        cout << "Predicted symbol using probabilities: " << predictedSymbol << endl;

        // Get actual symbol from the data
        char actualSymbol = data[i];
        cout << "Actual symbol from the data: " << actualSymbol << endl;

        // Check if prediction matches the actual symbol
        if (predictedSymbol == actualSymbol) {
            cout << "Prediction matched the actual symbol: " << predictedSymbol << endl;
            hits[actualSymbol]++;
        } else {
            cout << "Prediction did not match the actual symbol. Predicted: " << predictedSymbol << ", Actual: " << actualSymbol << endl;
            misses[actualSymbol]++;
        }

        // Update probability hash table
        probability[actualSymbol] = (hits[actualSymbol] + alpha) / (hits[actualSymbol] + misses[actualSymbol] + 2 * alpha);

        // Normalize probabilities to ensure they sum up to 1
        double sumProbabilities = 0.0;
        for (auto entry : probability) {
            sumProbabilities += entry.second;
        }
        for (auto &entry : probability) {
            entry.second /= sumProbabilities;
        }
        
        // Update window
        window.erase(window.begin());
        window.push_back(actualSymbol);

        cout << "Updated window: ";
        for (char ch : window) {
            cout << ch;
        }
        cout << endl;

        // Check if the model's performance has dropped below the threshold
        if (i >= windowSize && (double)hits[actualSymbol] / (hits[actualSymbol] + misses[actualSymbol]) < threshold) {
            // Search for the last occurrence of the current symbol in the past windowSize symbols
            auto it = search(data.begin(), data.begin() + i, data.begin() + i - windowSize, data.begin() + i);
            // If found, update the window to the start of the last occurrence
            if (it != data.begin() + i) {
                window = vector<char>(it, it + windowSize);
            }
        }
    }

    // Output the probability hash table
    cout << "Probability hash table:" << endl;
    for (auto entry : probability) {
        cout << entry.first << ": " << entry.second << endl;
    }

    // Output the amount of information for each symbol
    cout << "Amount of information for each symbol:" << endl;
    for (auto entry : probability) {
        char symbol = entry.first;
        double info = -log2(entry.second); // Amount of information for the symbol
        cout << symbol << ": " << info << " bits" << endl;
    }

    // Output the hits and misses hash tables
    cout << "Hits and misses hash tables:" << endl;
    for (auto entry : hits) {
        cout << entry.first << ": " << entry.second << " hits, " << misses[entry.first] << " misses" << endl;
    }

    return 0;
}