/**
 * PRL - Project - Mesh Multiplication
 * Implementation of parallel sorting algorithm called Pipeline Merge Sort.
 * @file            pms.h
 * @version			2.0
 * @author			Daniel Konecny (xkonec75)
 * @organisation	Brno University of Technology - Faculty of Information Technologies
 * @date			03. 05. 2021
 */

#include <vector>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[]);

/**
 * Load input from pre-defined input files and check if it is correct.
 * @param matrix1
 * @param matrix2
 * @param rows
 * @param cols
 * @return 0 if input is correct, non-zero value if input is incorrect.
 */
int load_input(vector <vector<long long int>> *matrix1,
                vector <vector<long long int>> *matrix2,
                int *rows,
                int *cols);

/**
 * Load the file with matrix to a 2-D vector, with the dimension as well.
 * @param file_name
 * @return Dimension and matrix together in one 2-D vector.
 */
vector <vector<long long int>> load_matrix(const char *file_name);

/**
 * Check format of the first line of input file and erase it afterwards.
 * @param matrix
 * @return Value from the first line or -1 in case of incorrect input.
 */
int process_matrix(vector <vector<long long int>> *matrix);

/**
 * Check the dimensions of matrices for given dimension and multiplication conditions.
 * @param matrix1
 * @param matrix2
 * @param rows
 * @param cols
 * @return 0 if dimensions are correct, non-zero value otherwise.
 */
int check_input(vector <vector<long long int>> *matrix1,
                vector <vector<long long int>> *matrix2,
                int rows,
                int cols);

/**
 * Send the dimensions to other processes.
 * @param rows
 * @param cols
 * @param rest Remaining dimension that both matrices share.
 * @param process_count
 */
void distribute_dimensions(int rows, int cols, int rest, int process_count);

/**
 * Shift rows to make matrix valid for horizontal input of the algorithm.
 * @param matrix
 */
void shift_rows(vector <vector<long long int>> *matrix);

/**
 * Transpose matrix.
 * @param input
 * @return Transposed matrix.
 */
vector <vector<long long int>> transpose_matrix(const vector <vector<long long int>> *input);

/**
 * Shift columns to make matrix valid for vertical input of the algorithm.
 * This is done by transposing the matrix, shifting its rows and transposing it again.
 * @param input
 * @return
 */
vector <vector<long long int>> shift_cols(const vector <vector<long long int>> *input);

/**
 * Parses the matrices and sends values to processes in the first row and column.
 * @param matrix1
 * @param matrix2
 * @param rows
 * @param cols
 */
void distribute_matrices(vector <vector<long long int>> matrix1,
                         vector <vector<long long int>> matrix2,
                         int rows,
                         int cols);

/**
 * Save dimensions sent from the loading (last) process.
 * @param rows
 * @param cols
 * @param rest
 * @param last_process_id
 * @param stat
 */
void receive_dimensions(int *rows, int *cols, int *rest, int last_process_id, MPI_Status *stat);

/**
 * Receive two values, compute their multiplication and sum them with previous multiplications.
 * Send the values to following processes, if necessary.
 * After all multiplications and additions are done, result is sent to the process number 0.
 * Each process does this on its own.
 * @param rows
 * @param cols
 * @param rest
 * @param process_id
 * @param stat
 */
void compute_multiplication(int rows, int cols, int rest, int process_id, MPI_Status *stat);

/**
 * Receive all sums from all processes doing the computations.
 * @param rows
 * @param cols
 * @param stat
 * @return Resulting matrix.
 */
vector <vector<long long int>> collect_result(int rows, int cols, MPI_Status *stat);

/**
 * Print matrix in a format defined for output.
 * @param matrix
 */
void print_matrix(vector <vector<long long int>> *matrix);
