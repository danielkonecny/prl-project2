/**
 * PRL - Project - Mesh Multiplication
 * Implementation of parallel sorting algorithm called Pipeline Merge Sort.
 * @file            pms.cpp
 * @version			2.0
 * @author			Daniel Konecny (xkonec75)
 * @organisation	Brno University of Technology - Faculty of Information Technologies
 * @date			03. 05. 2021
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <climits>
#include <mpi.h>

#include "mm.h"

using namespace std;

#define TAG 0
#define TAG_FROM_LEFT 1
#define TAG_FROM_ABOVE 2
#define MAT1FILE "mat1"
#define MAT2FILE "mat2"

int main(int argc, char *argv[]) {
    int process_id;
    int process_count;
    MPI_Status stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    int last_process_id = process_count - 1;

    vector <vector<long long int>> matrix1, matrix2;
    int rows, cols, rest;

    /// Last process starts computing as the last one.
    /// Therefore, it is the best candidate for loading and distributing the data to other processes.
    if (process_id == last_process_id) {
        int is_input_ok = load_input(&matrix1, &matrix2, &rows, &cols);
        MPI_Send(&is_input_ok, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);

        if (is_input_ok == 0) {
            rest = matrix2.size();
            distribute_dimensions(rows, cols, rest, process_count);

            shift_rows(&matrix1);
            matrix2 = shift_cols(&matrix2);

            distribute_matrices(matrix1, matrix2, rows, cols);
        }
    }

    /// End program when incorrect input is encountered.
    if (process_id == 0) {
        int can_continue;
        MPI_Recv(&can_continue, 1, MPI_INT, last_process_id, TAG, MPI_COMM_WORLD, &stat);
        if (can_continue != 0) {
            return 1;
        }
    }

    /// Every process takes its part in computing the multiplication.
    receive_dimensions(&rows, &cols, &rest, last_process_id, &stat);
    compute_multiplication(rows, cols, rest, process_id, &stat);

    if (process_id == 0) {
        vector <vector<long long int>> result_matrix = collect_result(rows, cols, &stat);
        print_matrix(&result_matrix);
    }

    MPI_Finalize();
    return 0;
}

int load_input(vector <vector<long long int>> *matrix1,
               vector <vector<long long int>> *matrix2,
               int *rows,
               int *cols) {
    const char *mat1_file = MAT1FILE;
    const char *mat2_file = MAT2FILE;

    *matrix1 = load_matrix(mat1_file);
    *matrix2 = load_matrix(mat2_file);

    *rows = process_matrix(matrix1);
    *cols = process_matrix(matrix2);

    return check_input(matrix1, matrix2, *rows, *cols);
}

vector <vector<long long int>> load_matrix(const char *file_name) {
    ifstream mat_file(file_name, fstream::in);
    vector <vector<long long int>> matrix;
    string line_string;

    /// Process line by line to a stream.
    while (getline(mat_file, line_string)) {
        vector<long long int> line_vector;

        istringstream line_stream(line_string);

        /// Each stream is divided into numbers and inserted to the output.
        copy(istream_iterator<long long int>(line_stream),
             istream_iterator<long long int>(),
             back_inserter(line_vector));

        matrix.push_back(line_vector);
    }

    mat_file.close();
    return matrix;
}

int process_matrix(vector <vector<long long int>> *matrix) {
    int info = -1;

    if (matrix->front().size() != 1) {
        cerr << "ERROR: Unknown format of the first row (only one integer allowed)." << endl;
    } else {
        info = matrix->front().front();
        /// Erase the initial loaded number (rows/columns) so it is just a matrix now.
        matrix->erase(matrix->begin());
    }

    return info;
}

int check_input(vector <vector<long long int>> *matrix1,
                vector <vector<long long int>> *matrix2,
                int rows,
                int cols) {
    if (rows == -1 || cols == -1) {
        return 1;
    }

    int matrix1_rows = matrix1->size();
    int matrix1_cols = matrix1->front().size();
    int matrix2_rows = matrix2->size();
    int matrix2_cols = matrix2->front().size();

    if (matrix1_cols != matrix2_rows) {
        cerr << "ERROR: Size of matrices does not match for multiplication." << endl;
        return 1;
    }
    if (matrix1_rows != rows) {
        cerr << "ERROR: Size of matrix does not match size from file." << endl;
        return 1;
    }
    if (matrix2_cols != cols) {
        cerr << "ERROR: Size of matrix does not match size from file." << endl;
        return 1;
    }

    return 0;
}

void distribute_dimensions(int rows, int cols, int rest, int process_count) {
    for (int i = 0; i < process_count; i++) {
        MPI_Send(&rows, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        MPI_Send(&cols, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
        MPI_Send(&rest, 1, MPI_INT, i, TAG, MPI_COMM_WORLD);
    }
}

void shift_rows(vector <vector<long long int>> *matrix) {
    int rows = matrix->size();

    for (int i = 0; i < rows; i++) {
        /// Inserts a special constant LLONG_MAX representing padding done by shifting.
        for (int j = 0; j < rows - 1; j++) {
            (*matrix)[i].push_back(LLONG_MAX);
        }
        /// Padding is added to the end, therefore, a rotation has to be applied to place it on correct position.
        rotate((*matrix)[i].rbegin(), (*matrix)[i].rbegin() + rows - i - 1, (*matrix)[i].rend());
    }
}

vector <vector<long long int>> transpose_matrix(const vector <vector<long long int>> *input) {
    vector <vector<long long int>> transposed(input->front().size(), vector<long long int>(input->size()));

    /// Goes over the matrix and copies the values to a new one that is transposed.
    for (vector<long long int>::size_type i = 0; i < input->front().size(); i++) {
        for (vector<long long int>::size_type j = 0; j < input->size(); j++) {
            transposed[i][j] = (*input)[j][i];
        }
    }

    return transposed;
}

vector <vector<long long int>> shift_cols(const vector <vector<long long int>> *input) {
    vector <vector<long long int>> transposed = transpose_matrix(input);
    shift_rows(&transposed);
    return transpose_matrix(&transposed);
}

void distribute_matrices(vector <vector<long long int>> matrix1,
                         vector <vector<long long int>> matrix2,
                         int rows,
                         int cols) {
    int shifts = max(matrix1.front().size(), matrix2.size());

    /// Do all the shifts to send all data to the process mesh.
    for (int shift = 0; shift < shifts; shift++) {
        /// Send only if there are numbers to send from left.
        if (matrix1.front().size() > 0) {
            /// Send all numbers from the last column (except from padding LLONG_MAX) and erase them.
            for (int row = 0; row < rows; row++) {
                long long int element = matrix1[row].back();
                matrix1[row].pop_back();
                if (element != LLONG_MAX) {
                    int process_index = row * cols;
                    MPI_Send(&element, 1, MPI_LONG_LONG_INT, process_index, TAG_FROM_LEFT, MPI_COMM_WORLD);
                }
            }
        }
        /// Send only if there are numbers to send from above.
        if (matrix2.size() > 0) {
            /// Send all numbers from the last row (except from padding LLONG_MAX) and erase it.
            for (int col = cols - 1; col >= 0; col--) {
                long long int element = matrix2.back().back();
                matrix2.back().pop_back();
                if (element != LLONG_MAX) {
                    int process_index = col;
                    MPI_Send(&element, 1, MPI_LONG_LONG_INT, process_index, TAG_FROM_ABOVE, MPI_COMM_WORLD);
                }
            }
            matrix2.pop_back();
        }
    }
}

void receive_dimensions(int *rows, int *cols, int *rest, int last_process_id, MPI_Status *stat) {
    MPI_Recv(rows, 1, MPI_INT, last_process_id, TAG, MPI_COMM_WORLD, stat);
    MPI_Recv(cols, 1, MPI_INT, last_process_id, TAG, MPI_COMM_WORLD, stat);
    MPI_Recv(rest, 1, MPI_INT, last_process_id, TAG, MPI_COMM_WORLD, stat);
}

void compute_multiplication(int rows, int cols, int rest, int process_id, MPI_Status *stat) {
    long long int sum = 0;
    int recv_from1, recv_from2;
    int next_process_in_row = process_id + 1;
    int next_process_in_col = process_id + cols;
    bool send_right = (next_process_in_row / rows) < cols;
    bool send_down = (next_process_in_col / cols) < rows;

    /// Set from which processes receive numbers.
    if (process_id % cols == 0) {
        recv_from1 = rows * cols - 1;
    } else {
        recv_from1 = process_id - 1;
    }
    if (process_id < cols) {
        recv_from2 = rows * cols - 1;
    } else {
        recv_from2 = process_id - cols;
    }

    /// Rest is the matching dimension, therefore, this many multiplications has to be done.
    for (int i = 0; i < rest; i++) {
        long long int num1, num2;

        MPI_Recv(&num1, 1, MPI_LONG_LONG_INT, recv_from1, TAG_FROM_LEFT, MPI_COMM_WORLD, stat);
        MPI_Recv(&num2, 1, MPI_LONG_LONG_INT, recv_from2, TAG_FROM_ABOVE, MPI_COMM_WORLD, stat);
        sum += num1 * num2;

        /// Send numbers further on if necessary.
        if (send_right) {
            MPI_Send(&num1, 1, MPI_LONG_LONG_INT, next_process_in_row, TAG_FROM_LEFT, MPI_COMM_WORLD);
        }
        if (send_down) {
            MPI_Send(&num2, 1, MPI_LONG_LONG_INT, next_process_in_col, TAG_FROM_ABOVE, MPI_COMM_WORLD);
        }
    }
    MPI_Send(&sum, 1, MPI_LONG_LONG_INT, 0, TAG, MPI_COMM_WORLD);
}

vector <vector<long long int>> collect_result(int rows, int cols, MPI_Status *stat) {
    vector <vector<long long int>> result_matrix;

    /// Form a matrix from all resulting sums (line by line, order given by processes).
    for (int row = 0; row < rows; row++) {
        vector<long long int> result_line;
        for (int col = 0; col < cols; col++) {
            long long int result_element;
            int sending_process = cols * row + col;
            MPI_Recv(&result_element, 1, MPI_LONG_LONG_INT, sending_process, TAG, MPI_COMM_WORLD, stat);
            result_line.push_back(result_element);
        }
        result_matrix.push_back(result_line);
    }

    return result_matrix;
}

void print_matrix(vector <vector<long long int>> *matrix) {
    cout << matrix->size() << ":" << matrix->front().size() << endl;
    for (auto row : *matrix) {
        for (auto element : row) {
            cout << element << ' ';
        }
        cout << endl;
    }
}
