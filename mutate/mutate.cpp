#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h> // for getopt
#include <unordered_set>
#include <cstdlib> // for rand() and srand()
#include <ctime>   // for time()

using namespace std;

// Function to perform mutation on a character based on probability and distinct letters
char mutateCharacter(char ch, double probability, const unordered_set<char>& letters) {
    // Generate a random value between 0 and 1
    double randomValue = (double)rand() / RAND_MAX;

    // If the random value is greater than the probability, replace the current character
    if (randomValue < probability) {
        // Select a random letter from the set of unique letters
        int randomIndex = rand() % letters.size();
        auto it = letters.begin();
        advance(it, randomIndex);
        return *it;
    }

    return ch; // If no mutation, return the original character
}

int main(int argc, char* argv[]) {
    string inputFileName;
    string outputFileName;
    double probability = 0;

    int opt;
    while ((opt = getopt(argc, argv, "i:o:p:")) != -1) {
        switch (opt) {
            case 'i':
                inputFileName = optarg;
                break;
            case 'o':
                outputFileName = optarg;
                break;
            case 'p':
                probability = stod(optarg);
                break;
            default:
                cerr << "Usage: " << argv[0] << " -i inputFileName -o outputFileName -p probability\n";
                return 1;
        }
    }

    // Check if input file, output file, and probability are provided
    if (inputFileName.empty() || outputFileName.empty() || probability == 0) {
        cerr << "Usage: " << argv[0] << " -i input_file -o output_file -p probability\n";
        return 1;
    }

    // Check if the probability is in the valid range (0 to 1)
    if (probability < 0 || probability > 1) {
        cerr << "Probability must be between 0 and 1.\n";
        return 1;
    }

    // Open the input file
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: cannot open " << inputFileName << endl;
        return 1;
    }

    // Unordered set to store the unique letters in the input file
    unordered_set<char> letters;

    // Read the input file character by character and populate the set of unique letters
    char ch;
    while (inputFile.get(ch)) {
        letters.insert(ch);
    }

    // Reset the file pointer to the beginning of the file
    inputFile.clear(); // Clear any error flags
    inputFile.seekg(0, ios::beg); // Move file pointer to the beginning

    // Seed the random number generator with current time
    srand(time(nullptr));

    // Open the output file
    ofstream outputFile(outputFileName);
    if (!outputFile) {
        cerr << "Error: cannot create " << outputFileName << endl;
        return 1;
    }

    // Read the input file character by character and perform mutation
    while (inputFile.get(ch)) {
        // Mutate the character
        ch = mutateCharacter(ch, probability, letters);
        
        // Write the mutated character to the output file
        outputFile << ch;
    }

    // Close the input and output files
    inputFile.close();
    outputFile.close();

    return 0;
}
