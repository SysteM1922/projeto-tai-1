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

    // If the probability is greater than the random value, replace the current character
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

    // get size of the file
    ifstream inputFile(inputFileName);
    if (!inputFile) {
        cerr << "Error: Could not open input file.\n";
        return 1;
    }
    
    //fileSize is the size of the file
    inputFile.seekg(0, ios::end);
    int fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    // read the file into memory
    char *fileData =  (char*)malloc(fileSize * sizeof(char));
    if (fileData == NULL) {
        cerr << "Error: Could not allocate memory for file data.\n";
        return 1;
    }

    if (!inputFile.read(fileData, fileSize)) {
        cerr << "Error: Could not read input file.\n";
        return 1;
    }
    inputFile.close();

    //set to store the unique letters in the input file
    unordered_set<char> letters;

    //populate the set of unique letters
    for (int i = 0; i < fileSize; i++) {
        letters.insert(fileData[i]);
    }

    //seed the random number generator
    srand(time(nullptr));

    // Open the output file
    ofstream outputFile(outputFileName);
    if (!outputFile) {
        cerr << "Error: cannot create " << outputFileName << endl;
        return 1;
    }

    // Perform mutation on each character and write to the output file
    for (int i = 0; i < fileSize; i++) {
        outputFile << mutateCharacter(fileData[i], probability, letters);
    }

    // Close the output file
    outputFile.close();
    
    return 0;
}
