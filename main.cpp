#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <fstream>
#include <chrono>

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

Matrix generateFixedMatrix(int rows, int cols) {
    Matrix matrix(rows, std::vector<int>(cols));
    int value = 1;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = value++;
        }
    }
    return matrix;
}

/**
 * function to measure the time taken for
 * matrix multiplication and write to file
 * @param matrix1 First matrix
 * @param matrix2 Second matrix
 * @param outputFile File to hold results of execution
*/
void measureAndWritePerformance(const Matrix& matrix1, const Matrix& matrix2, std::ofstream& outputFile) {
    // std::vector<int> threadList = {1, 2, 4, 8, 10, 12, 14, 16, 18, 20, 22, };
    
    for (int numThreads = 2; numThreads < 31; numThreads += 2) {
        // Create result matrix
        Matrix resultMatrix(matrix1.size(), std::vector<int>(matrix2[0].size()));

        // measure time taken for multiplication of matrix based on number of threads
        auto start = std::chrono::high_resolution_clock::now();
        multiplyMatrices(matrix1, matrix2, resultMatrix, numThreads);
        auto stop = std::chrono::high_resolution_clock::now();

        int totalMatrixSize = (matrix1.size() * matrix1[0].size()) + (matrix2.size() * matrix2[0].size());

        // Calculate the time taken in a suitable unit
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        outputFile << totalMatrixSize << "," << numThreads << "," << duration.count() << std::endl;
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

// Set1 matrix
std::vector<Matrix> set1 = {
    {{1, 2, 3, 1, 2, 3},    
     {4, 5, 6, 4, 5, 6}},

    {{7, 8, 7, 8, 7, 8, 7, 8},       
     {9, 10, 9, 10, 9, 10, 9, 10},
     {11, 12, 11, 12, 11, 12, 11, 12}},

    {{13, 14, 15, 13, 14, 15, 13, 14, 15, 13, 14, 15}, 
     {16, 17, 18, 16, 17, 18, 16, 17, 18, 16, 17, 18}},

    {{19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20},     
     {21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22},
     {23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24}},

    {{25, 26, 27, 25, 26, 27, 25, 26, 27, 25, 26, 27, 25, 26, 27, 25, 26, 27}, 
     {28, 29, 30, 28, 29, 30, 28, 29, 30, 28, 29, 30 ,28, 29, 30, 28, 29, 30}},

{{19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20},     
 {21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22},
 {23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24}},

{{19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20, 19, 20},     
 {21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22, 21, 22},
 {23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24, 23, 24}}

};

// Set2 matrix
std::vector<Matrix> set2 = {
    {{1, 2},      
     {3, 4},
     {5, 6},
     {1, 2},      
     {3, 4},
     {5, 6}},

    {{7, 8, 9},   
     {10, 11, 12},
     {7, 8, 9},   
     {10, 11, 12},
     {7, 8, 9},   
     {10, 11, 12},
     {7, 8, 9},   
     {10, 11, 12}},

    {{13},        
     {14},
     {15},
     {13},        
     {14},
     {15},
     {13},        
     {14},
     {15},
     {13},        
     {14},
     {15}},

    {{16, 17},    
     {18, 19},
     {16, 17},    
     {18, 19},
     {16, 17},    
     {18, 19},
     {16, 17},    
     {18, 19},
     {16, 17},    
     {18, 19},
     {16, 17},    
     {18, 19},
     {16, 17},    
    {18, 19}},
    //  {16, 17},    
    //  {18, 19},
    //  {16, 17},    
    //  {18, 19},
    //  {16, 17},    
    //  {18, 19}},

    {{20, 21, 22},
     {23, 24, 25},
     {26, 27, 28},
     {20, 21, 22},
     {23, 24, 25},
     {26, 27, 28},
     {20, 21, 22},
     {23, 24, 25},
     {26, 27, 28},
     {20, 21, 22},
     {23, 24, 25},
     {26, 27, 28},
     {20, 21, 22},
     {23, 24, 25},
     {26, 27, 28},
     {20, 21, 22},
     {23, 24, 25},
     {26, 27, 28}},

{
 {1, 2, 3, 4},
 {5, 6, 7, 8},
 {9, 10, 11, 12},
 {13, 14, 15, 16},
 {17, 18, 19, 20},
 {21, 22, 23, 24},
 {25, 26, 27, 28},
 {29, 30, 31, 32},
 {33, 34, 35, 36},
 {37, 38, 39, 40},
 {41, 42, 43, 44},
 {45, 46, 47, 48},
 {49, 50, 51, 52},
 {53, 54, 55, 56},
 {57, 58, 59, 60},
 {61, 62, 63, 64},
 {65, 66, 67, 68},
 {69, 70, 71, 72},
 {73, 74, 75, 76},
 {77, 78, 79, 80}
},

{
 {1, 2, 3, 4, 5},
 {6, 7, 8, 9, 10},
 {11, 12, 13, 14, 15},
 {16, 17, 18, 19, 20},
 {21, 22, 23, 24, 25},
 {26, 27, 28, 29, 30},
 {31, 32, 33, 34, 35},
 {36, 37, 38, 39, 40},
 {41, 42, 43, 44, 45},
 {46, 47, 48, 49, 50},
 {51, 52, 53, 54, 55},
 {56, 57, 58, 59, 60},
 {61, 62, 63, 64, 65},
 {66, 67, 68, 69, 70},
 {71, 72, 73, 74, 75},
 {76, 77, 78, 79, 80},
 {81, 82, 83, 84, 85},
 {86, 87, 88, 89, 90},
 {91, 92, 93, 94, 95},
 {96, 97, 98, 99, 100},
 {101, 102, 103, 104, 105},
 {106, 107, 108, 109, 110},
 {111, 112, 113, 114, 115},
 {116, 117, 118, 119, 120},
 {121, 122, 123, 124, 125},
 {126, 127, 128, 129, 130}}


};

    // create file object and write execution to file
    std::ofstream outputFile("execution_times.csv", std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return 1;
    }

    outputFile << "Total matrix size, Thread Count, Execution Time (microseconds)" << std::endl;

    for(int i=0; i < 7; i++){
        measureAndWritePerformance(set1[i], set2[i], outputFile);
    }

    outputFile.close();

    return 0;
}
