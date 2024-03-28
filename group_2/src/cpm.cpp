#include <iostream>
#include <chrono>
#include <getopt.h>
#include <cstdlib>
#include <unordered_map>

#include "table.cpp"
#include "models.cpp"

using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[])
{

    FILE *inputfile = NULL;
    int k = 0;
    double threshold;
    int minTries;
    int n = 1;
    double alpha = 0;
    int fallbackWindowSize = 0;

    double totalBits = 0;
    int fileSize = 0;

    Alphabet alphabet;

    if (argc < 13)
    {
        cerr << "Usage: " << argv[0] << " -f <input_file> -k <kmer_size> -t <minTries/threshold> -n <nCopyModels> -s <fallback_window_size -a <alpha>" << endl;
        return 1;
    }

    int opt;
    while ((opt = getopt(argc, argv, "f:k:t:n:s:a:")) != -1)
    {
        switch (opt)
        {
        case 'f':
            inputfile = fopen(optarg, "r");
            if (inputfile == NULL)
            {
                cerr << "Error opening input file." << endl;
                return 1;
            }
            break;
        case 'k':
            k = atoi(optarg);
            break;
        case 't':
            sscanf(optarg, "%d/%lf", &minTries, &threshold);
            break;
        case 'n':
            n = atoi(optarg);
            break;
        case 'a':
            alpha = atof(optarg);
            break;
        case 's':
            fallbackWindowSize = atoi(optarg);
            break;
        default:
            cerr << "Usage: " << argv[0] << " -f <input_file> -k <kmer_size> -t <minTries/threshold> -n <nCopyModels> -s <fallback_window_size -a <alpha>" << endl;
            return 1;
        }
    }

    Table table = Table(k);

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

    FallbackModel fallbackModel(data, k, alphabet, fallbackWindowSize);

    CopyModel copyModel = CopyModel(threshold, minTries, alphabet.size(), alpha, n);

    string kmer(k, data[0]);

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
                if (!copyModel.isActive())
                {
                    copyModel.resetModel();
                    table.advancePosition(kmer);
                }
                else
                {
                    totalBits += copyModel.predict(data[i]);

                    table.insert(kmer, data[i]);
                    fallbackModel.advancePosition(i - 1);

                    if (!copyModel.isFull())
                    {
                        copyModel.addReference(table.getCurrentElement(kmer));
                    }
                    continue;
                }
            }
            else if (copyModel.isNull() || !copyModel.isActive())
            {
                copyModel.newReferences(kmer, table.getCurrentElement(kmer));
                totalBits += copyModel.predict(data[i]);
                table.insert(kmer, data[i]);
                fallbackModel.advancePosition(i - 1);
                continue;
            }
        }
        fallbackModel.advancePosition(i - 1);
        totalBits += fallbackModel.calcBits(data[i]);
        table.insert(kmer, data[i]);
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Copy Models: " << n << endl;
    cout << "File: " << argv[2] << endl;
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
