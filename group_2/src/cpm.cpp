#include <iostream>
#include <fstream>
#include <cmath>
#include "table.cpp"
#include "models.cpp"
#include "cpm.h"

using namespace std;


int main(int argc, char *argv[])
{

    FILE *inputfile;
    int k;
    double threshold;

    unsigned int totalsize = 0;
    unsigned int fileSize = 0;

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
    threshold = atof(argv[3]);

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
    fread(data, sizeof(char), fileSize, inputfile);
    fclose(inputfile);

    // Compute the alphabet
    int i;
    for (i = 0; i < fileSize; i++)
    {
        alphabet.add(data[i]);
    }

    // Browse the data
    char c;
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
        // check if the kmer is in the table
        // if kmer is not in the table, use fallback and insert the kmer in the table
        // else update the table and use copymodel
    }
    
}
