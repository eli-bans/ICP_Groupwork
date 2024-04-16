#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <random> // to generate random numbers
#include <fstream> // For ofstream - outputting from runtime to external file
#include <chrono>

// Define matrix type for simplicity
using Matrix = std::vector<std::vector<int>>;

// Function to multiply a range of rows of matrix1 with matrix2 and store the result in resultMatrix
void multiplyRows(const Matrix& matrix1, const Matrix& matrix2, Matrix& resultMatrix, int startRow, int endRow, std::mutex& mtx) {
    int cols1 = matrix1[0].size();
    int cols2 = matrix2[0].size();
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < cols2; ++j) {
            int sum = 0;
            for (int k = 0; k < cols1; ++k) {
                sum += matrix1[i][k] * matrix2[k][j];
            }
            mtx.lock(); // Acquire lock before writing to resultMatrix
            resultMatrix[i][j] = sum;
            mtx.unlock(); // Release lock after writing
        }
    }
}

// Function to perform matrix multiplication using threads
void multiplyMatrices(const Matrix& matrix1, const Matrix& matrix2, Matrix& resultMatrix, int numThreads) {
    int rows1 = matrix1.size();
    int cols1 = matrix1[0].size();
    int cols2 = matrix2[0].size();

    // Check if matrix multiplication is possible
    if (cols1 != matrix2.size()) {
        throw std::invalid_argument("Matrix dimensions are invalid for multiplication");
    }

    int rowsPerThread = rows1 / numThreads;
    std::vector<std::thread> threads;
    std::mutex mtx; // Mutex for synchronization

    // Create threads to perform matrix multiplication
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? rows1 : (i + 1) * rowsPerThread;
        threads.emplace_back(multiplyRows, std::ref(matrix1), std::ref(matrix2), std::ref(resultMatrix), startRow, endRow, std::ref(mtx));
    }

    // Join threads to wait for completion
    for (auto& thread : threads) {
        thread.join();
    }
}

// Generate random sized matrix
Matrix generateMatrix() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disSquare(0, 1); // range for random pick between square or no square matrix
    std::uniform_int_distribution<> disSize(2, 10); // range for random pick for matrix size.

    // Randomly decide whether to generate a square or non-square matrix
    bool squareMatrix = disSquare(gen) == 1;

    int numRows, numCols;
    if (squareMatrix) {
        int size = disSize(gen); // randomly generate
        numRows = size;
        numCols = size;
    } else {
        numRows = disSize(gen);
        numCols = disSize(gen);
    }

    Matrix matrix(numRows, std::vector<int>(numCols));

    // populate matrix with constant number since values don't matter
    int value = 1;
    for (int i = 0; i < numRows; ++i) {
        for (int j = 0; j < numCols; ++j) {
            matrix[i][j] = value++;
        }
    }

    return matrix;
}

// Measure time taken for computation and write to csv file
void measureAndWritePerformance(int numThreads, ofstream& outputFile) {
    // Generate matrices
    Matrix matrix1 = generateMatrix();
    Matrix matrix2 = generateMatrix();
    Matrix resultMatrix(matrix1.size(), vector<int>(matrix2[0].size()));

    // Measure time taken for computation
    auto start = std::high_resolution_clock::now();
    multiplyMatrices(matrix1, matrix2, resultMatrix, numThreads);
    auto stop = std::high_resolution_clock::now();
    auto duration = std::duration_cast<milliseconds>(stop - start);

    int totalElements = matrix1.size() * matrix1[0].size() + matrix2.size() * matrix2[0].size();

    // Output benchmark results to file
    outputFile << totalElements << "," << numThreads << "," << duration.count() << endl;
}

// Run test functions and write results to benchmark file
void runBenchmark(int maxThreads) {
    try {
        // open results file
        std::ofstream outputFile("benchmark_results.csv");
        if (!outputFile.is_open()) {
            throw std::runtime_error("Error: Could not open output file.");
        }

        // First row with headers
        outputFile << "Matrix Size,Thread Count,Execution Time (ms)" << std::endl;

        // perform matrix multiplication with different threads 
        for (int i = 0; i <= maxThreads; ++i) {
            measureAndWritePerformance(i, outputFile);
        }

        // Close output file
        outputFile.close();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw; // Re-throw exception for outer try cath if one is available.
    }
}

// Main function
int main() {
    // Example matrices
    Matrix matrix1 = {{1, 2},
                      {3, 4},
                      {5, 6}};

    Matrix matrix2 = {{7, 8, 9},
                      {10, 11, 12}};


    // Specify number of threads
    int numThreads = 2; // Change this to desired number of threads

    // Result matrix
    Matrix resultMatrix(matrix1.size(), std::vector<int>(matrix2[0].size()));

    try {
        // Perform matrix multiplication using threads
        multiplyMatrices(matrix1, matrix2, resultMatrix, numThreads);

        // Display result matrix
        std::cout << "Result Matrix:" << std::endl;
        for (const auto& row : resultMatrix) {
            for (int elem : row) {
                std::cout << elem << " ";
            }
            std::cout << std::endl;
        }
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    runBenchmark(3);

    return 0;
}
