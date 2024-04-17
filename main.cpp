#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>

// Define matrix type for simplicity
using Matrix = std::vector<std::vector<int>>;

/**
 * Function to multiply rows of two matrices and store the result in another matrix
 * @param matrix1 First matrix
 * @param matrix2 Second matrix
 * @param resultMatrix Result matrix
 * @param startRow Start row index
 * @param endRow End row index
 * @return None
 */
void multiplyRows(const Matrix& matrix1, const Matrix& matrix2, Matrix& resultMatrix, int startRow, int endRow) {
    int cols1 = matrix1[0].size(); // Number of columns in matrix1
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < resultMatrix[i].size(); ++j) { // Iterate over columns of resultMatrix
            int sum = 0;
            for (int k = 0; k < cols1; ++k) {
                sum += matrix1[i][k] * matrix2[k][j];
            }
            resultMatrix[i][j] = sum;
        }
    }
}



/**
 * Function to multiply two matrices using threads
 * @param matrix1  First matrix
 * @param matrix2  Second matrix
 * @param resultMatrix  Result matrix
 * @param numThreads  Number of threads to use
 */
void multiplyMatrices(const Matrix& matrix1, const Matrix& matrix2, Matrix& resultMatrix, int numThreads) {
    int rows1 = matrix1.size();
    int cols1 = matrix1[0].size();
    int cols2 = matrix2[0].size();

    /**
     * Check if matrix dimensions are valid for multiplication
     * Number of columns in matrix1 should be equal to number of rows in matrix2
     */
    if (cols1 != matrix2.size()) {
        throw std::invalid_argument("Matrix dimensions are invalid for multiplication");
    }

   /**
    * If number of threads is 0, perform matrix multiplication sequentially
    */
    if (numThreads == 0) {
        for (int i = 0; i < rows1; ++i) {
            multiplyRows(matrix1, matrix2, resultMatrix, i, i + 1); // Call multiplyRows for each row
        }
        return; // No need to create threads
    }

    int rowsPerThread = rows1 / numThreads;
    std::vector<std::thread> threads;

   /**
    * Create threads to multiply rows of matrices
    * Each thread will multiply a subset of rows
    */
    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? rows1 : (i + 1) * rowsPerThread;
        threads.emplace_back(multiplyRows, std::ref(matrix1), std::ref(matrix2), std::ref(resultMatrix), startRow, endRow);
    }

   /**
    * Wait for all threads to finish execution
    */
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * Main function to test matrix multiplication
 * @param None
 * @return  0
 */
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

    return 0;
}
