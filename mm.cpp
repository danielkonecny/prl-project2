/**
 * PRL - Project - Mesh Multiplication
 * Implementation of parallel sorting algorithm called Pipeline Merge Sort.
 * @file            pms.cpp
 * @version			1.0
 * @author			Daniel Konecny (xkonec75)
 * @organisation	Brno University of Technology - Faculty of Information Technologies
 * @date			29. 04. 2021
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iterator>
#include <mpi.h>

#include "mm.h"

using namespace std;

#define TAG 0
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

    vector <vector<long long int>> matrix1;
    vector <vector<long long int>> matrix2;
    int rows;
    int cols;
    int rest;

    if (process_id == last_process_id) {
        load_input(&matrix1, &matrix2, &rows, &cols);

        int is_input_ok = check_input(&matrix1, &matrix2, rows, cols);
        MPI_Send(&is_input_ok, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD);

        shift_rows(&matrix1);
        matrix2 = shift_cols(&matrix2);

        if (is_input_ok == 0) {
            cout << "MATRIX 1 - rows = " << rows << endl;
            print_matrix(&matrix1);

            cout << "MATRIX 2 - cols = " << cols << endl;
            print_matrix(&matrix2);
        }

        distribute_matrices(&matrix1, &matrix2);

    } else if (process_id == 0) {
        int can_continue;
        MPI_Recv(&can_continue, 1, MPI_INT, last_process_id, TAG, MPI_COMM_WORLD, &stat);

        if (can_continue != 0) {
            return 1;
        }
    }


    MPI_Finalize();
    return 0;
}

void load_input(vector <vector<long long int>> *matrix1,
                vector <vector<long long int>> *matrix2,
                int *rows,
                int *cols) {
    const char *mat1_file = MAT1FILE;
    const char *mat2_file = MAT2FILE;

    *matrix1 = load_matrix(mat1_file);
    *matrix2 = load_matrix(mat2_file);

    *rows = process_matrix(matrix1);
    *cols = process_matrix(matrix2);
}

vector <vector<long long int>> load_matrix(const char *file_name) {
    ifstream mat_file(file_name, fstream::in);
    vector <vector<long long int>> matrix;
    string line_string;

    while (getline(mat_file, line_string)) {
        vector<long long int> line_vector;

        istringstream line_stream(line_string);

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

void shift_rows(vector <vector<long long int>> *matrix) {
    int rows = matrix->size();

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < rows - 1; j++) {
            (*matrix)[i].push_back(0);
        }
        rotate((*matrix)[i].rbegin(), (*matrix)[i].rbegin() + rows - i - 1, (*matrix)[i].rend());
    }
}

vector <vector<long long int>> shift_cols(const vector <vector<long long int>> *input) {
    vector <vector<long long int>> transposed((*input)[0].size(), vector<long long int>(input->size()));

    for (vector<long long int>::size_type i = 0; i < (*input)[0].size(); i++) {
        for (vector<long long int>::size_type j = 0; j < input->size(); j++) {
            transposed[i][j] = (*input)[j][i];
        }
    }

    shift_rows(&transposed);

    vector <vector<long long int>> shifted(transposed[0].size(), vector<long long int>(transposed.size()));

    for (vector<long long int>::size_type i = 0; i < transposed[0].size(); i++) {
        for (vector<long long int>::size_type j = 0; j < transposed.size(); j++) {
            shifted[i][j] = transposed[j][i];
        }
    }

    return shifted;
}

void print_matrix(vector <vector<long long int>> *matrix) {
    for (auto row : *matrix) {
        for (auto element : row) {
            cout << element << ' ';
        }
        cout << endl;
    }
}

void distribute_matrices(vector <vector<long long int>> *matrix1,
                         vector <vector<long long int>> *matrix2) {
    int matrix1_rows = matrix1->size();
    int matrix1_cols = matrix1->front().size();
    int matrix2_rows = matrix2->size();
    int matrix2_cols = matrix2->front().size();

    for (int i = 0; i < matrix1_rows + matrix2_cols; i++) {
        cout << "LOOP " << i << endl;
        cout << "-matrix 1" << endl;
        for (int row = 0; row < matrix1_rows; row++) {
            for (int col = matrix1_cols + matrix2_cols - 2 - i;
                 col > matrix1_cols - 2 - i;
                 col--) {
                if (col < 0 || col > matrix1_cols - 1) {
                    continue;
                }
                int element = (*matrix1)[row][col];
                if (element != 0) {
                    cout << "--sent " << element << " to cpu=" << matrix2_cols * row + col + i + 1 - matrix1_cols
                         << " row=" << row << " col=" << col << endl;
                }
            }
        }
        cout << "-matrix 2" << endl;
        for (int col = 0; col < matrix2_cols; col++) {
            for (int row = matrix2_rows + matrix1_rows - 2 - i;
                 row > matrix2_rows - 2 - i;
                 row--) {
                if (row < 0 || row > matrix2_rows - 1) {
                    continue;
                }
                int element = (*matrix2)[row][col];
                if (element != 0) {
                    cout << "--sent " << element << " to cpu=" << matrix2_cols * (row - i) + col - matrix2_rows
                         << " row=" << row << " col=" << col << endl;
                }
            }
        }
    }
}
