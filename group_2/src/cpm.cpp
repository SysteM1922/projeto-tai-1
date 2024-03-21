#include <iostream>
#include <chrono>
#include <fstream>
#include <cmath>
#include "table.cpp"
#include "models.cpp"
#include "cpm.h"

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{

    FILE *inputfile;
    int k;
    double threshold;
    int minTries;

    double totalBits = 0;
    int fileSize = 0;

    Table table;
    Alphabet alphabet;

    if (argc < 4)
    {
        cerr << "Usage: cpm <input_file> <k> <threshold>" << endl;
        return 1;
    }

    inputfile = fopen(argv[1], "r");
    if (inputfile == NULL)
    {
        cerr << "Error opening input file." << endl;
        return 1;
    }

    k = atoi(argv[2]);
    // threshold is in format <minTries>/<threshold>
    sscanf(argv[3], "%d/%lf", &minTries, &threshold);

    // start the timer
    auto start = high_resolution_clock::now();

    // Get the size of the file
    fseek(inputfile, 0, SEEK_END);
    fileSize = ftell(inputfile);
    fseek(inputfile, 0, SEEK_SET);

    // Read the file into memory
    char *data = (char *)malloc(fileSize * sizeof(char));
    if (data == NULL)
    {
        cerr << "Error allocating memory for input file." << endl;
        return 1;
    }
    if (static_cast<int>(fread(data, sizeof(char), fileSize, inputfile)) != fileSize)
    {
        cerr << "Error reading input file." << endl;
        return 1;
    }
    fclose(inputfile);

    // Compute the alphabet
    int i;
    for (i = 0; i < fileSize; i++)
    {
        alphabet.add(data[i]);
    }
    alphabet.loadCharMap();

    FallbackModel fallbackModel(k, alphabet);
    
    // for 1 CopyModel
    CopyModel copyModel = CopyModel(threshold, minTries, alphabet.size());

    char kmer[k];
    // fill kmer with th first character
    for (i = 0; i < k; i++)
    {
        kmer[i] = data[0];
    }

    int j;
    for (i = 1; i < fileSize; i++)
    {
        for (j = 0; j < k - 1; j++)
        {
            kmer[j] = kmer[j + 1];
        }
        kmer[k - 1] = data[i - 1];

        if (table.contains(kmer))
        {
            if (copyModel.match(kmer))
            {
                if (copyModel.thresholdReached())
                {
                    copyModel.resetModel();
                    table.advancePosition(kmer);
                }
                else
                {
                    totalBits += copyModel.predict(data[i]);
                
                    table.insert(kmer, i);
                    continue;
                }
            }
            else if (copyModel.isNull())
            {
                copyModel.addKmer(kmer, data[table.getPosition(kmer)]);
                totalBits += copyModel.predict(data[i]);
                table.insert(kmer, i);
                continue;
            }
        }
        totalBits += fallbackModel.calcBits(kmer);
        table.insert(kmer, i);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    
    cout << "File: " << argv[1] << endl;
    cout << "k: " << k << endl;
    cout << "Threshold: " << threshold << endl;
    cout << "MinTries: " << minTries << endl;
    cout << "File size (Bases): " << fileSize << endl;
    cout << "Alphabet size: " << alphabet.size() << endl;
    cout << "Total bytes (B): " << (int)(totalBits / 8) << endl;
    cout << "Bits per base: " << (double)totalBits / fileSize << endl;
    cout << "Time (s): " << duration.count() / 1000000.0 << endl;

    free(data);

    return 0;
}
