#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

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


// Main function
int main() {
    // Example matrices
    Matrix matrix1 = {{1, 2, 3},
                      {4, 5, 6},
                      {7, 8, 9}};
    Matrix matrix2 = {{9, 8, 7},
                      {6, 5, 4},
                      {3, 2, 1}};

    // Determine number of threads
    int numThreads;
    std::cout << "Enter number of threads: ";
    std::cin >> numThreads;

    // Result matrix
    Matrix resultMatrix(matrix1.size(), std::vector<int>(matrix2[0].size()));

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

    return 0;
}
