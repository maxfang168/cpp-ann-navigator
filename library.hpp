#include <iostream>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <thread>
#include <future>

// Functions for text processing
bool isPunctuation(char character) {
    const std::unordered_set<char> punctuation = {'.', ',', '!', '?', ';', ':', '(', ')', '-', '`', '\'', '\"', '[', ']', '{', '}', '<', '>', '/', '\\', '_', '@', '=', '+', '*', '%', '^', '~', '#', '$', '&', '\n', '\t'};
    return punctuation.find(character) != punctuation.end();
}

std::string preprocessText(std::string fileContent) {
    std::string processedFileContent = fileContent;
    std::transform(processedFileContent.begin(), processedFileContent.end(), processedFileContent.begin(), ::tolower);
    processedFileContent.erase(
        std::remove_if(processedFileContent.begin(), processedFileContent.end(), isPunctuation),
        processedFileContent.end());
    return processedFileContent;
}

std::vector<std::string> tokenizeText(std::string processedFileContent, std::vector<std::string>& tokens, int& vocabSize) {
    std::istringstream inputStream(processedFileContent);
    std::string segment;
    while (inputStream >> segment) {
        tokens.push_back(segment);
    }
    std::sort(tokens.begin(), tokens.end());
    tokens.erase(std::unique(tokens.begin(), tokens.end()), tokens.end());
    vocabSize = tokens.size();
    return tokens;
}

std::string readTextFile(std::string fileName) {
    std::ifstream file(fileName);
    std::string fileContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    return fileContent;
}

// Functions for neural network (general)

// Activation functions

double leakyReLU(double neuronOutput) {
    if (neuronOutput < 0) {
        return 0.01 * neuronOutput;
    }
    else {
        return neuronOutput;
    }
}

double sigmoid(double neuronOutput) {
    return 1 / (1 + exp(-neuronOutput));
}

double tanh(double neuronOutput) {
    return (exp(neuronOutput) - exp(-neuronOutput)) / (exp(neuronOutput) + exp(-neuronOutput));
}

// Derivatives of activation functions

double leakyReLUDerivative(double neuronOutput) {
    if (neuronOutput < 0) {
        return 0.01;
    }
    else {
        return 1;
    }
}

double sigmoidDerivative(double neuronOutput) {
    return sigmoid(neuronOutput) * (1 - sigmoid(neuronOutput));
}

double tanhDerivative(double neuronOutput) {
    return 1 - pow(tanh(neuronOutput), 2);
}

// Error functions
std::vector<double> meanSquaredError(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errors;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errors.push_back(pow(actualOutputs[i] - expectedOutputs[i], 2));
    }
    return errors;
}

std::vector<double> crossEntropyError(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errors;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errors.push_back(-expectedOutputs[i] * log(actualOutputs[i]) - (1 - expectedOutputs[i]) * log(1 - actualOutputs[i]));
    }
    return errors;
}

std::vector<double> binaryCrossEntropyError(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errors;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errors.push_back(-expectedOutputs[i] * log(actualOutputs[i]) - (1 - expectedOutputs[i]) * log(1 - actualOutputs[i]));
    }
    return errors;
}

std::vector<double> categoricalCrossEntropyError(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errors;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errors.push_back(-expectedOutputs[i] * log(actualOutputs[i]));
    }
    return errors;
}

// Derivatives of error functions

std::vector<double> meanSquaredErrorDerivative(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errorDerivatives;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errorDerivatives.push_back(actualOutputs[i] - expectedOutputs[i]);
    }
    return errorDerivatives;
}

std::vector<double> crossEntropyErrorDerivative(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errorDerivatives;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errorDerivatives.push_back((actualOutputs[i] - expectedOutputs[i]) / (actualOutputs[i] * (1 - actualOutputs[i])));
    }
    return errorDerivatives;
}

std::vector<double> binaryCrossEntropyErrorDerivative(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errorDerivatives;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errorDerivatives.push_back((actualOutputs[i] - expectedOutputs[i]) / (actualOutputs[i] * (1 - actualOutputs[i])));
    }
    return errorDerivatives;
}

std::vector<double> categoricalCrossEntropyErrorDerivative(std::vector<double> expectedOutputs, std::vector<double> actualOutputs) {
    std::vector<double> errorDerivatives;
    for (int i = 0; i < expectedOutputs.size(); i++) {
        errorDerivatives.push_back(actualOutputs[i] - expectedOutputs[i]);
    }
    return errorDerivatives;
}

// Generate numbers from a normal distribution (multithreaded)
std::vector<double> generateNormalDistribution(int size, double mean, double standardDeviation) {
    std::vector<double> normalDistribution(size);
    std::vector<std::future<void>> futures;
    int numThreads = std::thread::hardware_concurrency();
    int chunkSize = size / numThreads;

    for (int t = 0; t < numThreads; t++) {
        futures.push_back(std::async([t, chunkSize, size, mean, standardDeviation, &normalDistribution, numThreads]() {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? size : start + chunkSize;
            for (int i = start; i < end; i++) {
                double u1 = (double)rand() / RAND_MAX;
                double u2 = (double)rand() / RAND_MAX;
                double z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
                normalDistribution[i] = mean + z * standardDeviation;
            }
        }));
    }

    for (auto& future : futures) {
        future.get();
    }

    return normalDistribution;
}

// Generate x random numbers between range (multithreaded)
std::vector<double> generateRandomNumbers(int size, double lowerBound, double upperBound) {
    std::vector<double> randomNumbers(size);
    std::vector<std::future<void>> futures;
    int numThreads = std::thread::hardware_concurrency();
    int chunkSize = size / numThreads;

    for (int t = 0; t < numThreads; t++) {
        futures.push_back(std::async([t, chunkSize, size, lowerBound, upperBound, &randomNumbers, numThreads]() {
            int start = t * chunkSize;
            int end = (t == numThreads - 1) ? size : start + chunkSize;
            for (int i = start; i < end; i++) {
                randomNumbers[i] = lowerBound + (double)rand() / RAND_MAX * (upperBound - lowerBound);
            }
        }));
    }

    for (auto& future : futures) {
        future.get();
    }

    return randomNumbers;
}

// Matrix operations (multithreaded)
std::vector<std::vector<double>> matrixMultiplication(std::vector<std::vector<double>> matrix1, std::vector<std::vector<double>> matrix2) {
    std::vector<std::vector<double>> resultMatrix(matrix1.size(), std::vector<double>(matrix2[0].size(), 0));
    std::vector<std::future<void>> futures;
    for (int i = 0; i < matrix1.size(); i++) {
        futures.push_back(std::async([i, &matrix1, &matrix2, &resultMatrix]() {
            for (int j = 0; j < matrix2[0].size(); j++) {
                for (int k = 0; k < matrix1[0].size(); k++) {
                    resultMatrix[i][j] += matrix1[i][k] * matrix2[k][j];
                }
            }
        }));
    }
    for (int i = 0; i < futures.size(); i++) {
        futures[i].get();
    }
    return resultMatrix;
}