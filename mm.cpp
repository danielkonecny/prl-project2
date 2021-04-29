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
#include <fstream>
#include <sstream>
#include <iterator>
#include <mpi.h>

#include "mm.h"

using namespace std;

#define MAT1FILE "mat1"
#define MAT2FILE "mat2"

int main(int argc, char *argv[]) {
    int process_id;
    MPI_Status stat;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &process_id);

    vector <vector<long long int>> matrix1;
    vector <vector<long long int>> matrix2;
    int rows;
    int cols;

    if (process_id == 0) {
        const char *mat1_file = MAT1FILE;
        const char *mat2_file = MAT2FILE;

        matrix1 = load_matrix(mat1_file);
        matrix2 = load_matrix(mat2_file);

        rows = process_matrix(&matrix1);
        cols = process_matrix(&matrix2);

        if (rows == -1 || cols == -1) {
            return 1;
        }
        if (!check_matrices(matrix1, matrix2, rows, cols)) {
            return 1;
        }

        cout << "MATRIX 1 - rows = " << rows << endl;
        print_matrix(matrix1);
        cout << "MATRIX 2 - cols = " << cols << endl;
        print_matrix(matrix2);
    } else {

    }

    MPI_Finalize();
    return 0;
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
        cerr << "ERROR: First row bad format." << endl;
    } else {
        info = matrix->front().front();
        matrix->erase(matrix->begin());
    }

    return info;
}

bool check_matrices(vector <vector<long long int>> matrix1,
                    vector <vector<long long int>> matrix2,
                    int rows,
                    int cols) {
    int matrix1_rows = matrix1.size();
    int matrix1_cols = matrix1.front().size();
    int matrix2_rows = matrix2.size();
    int matrix2_cols = matrix2.front().size();

    if (matrix1_cols != matrix2_rows) {
        cerr << "ERROR: Size of matrices does not match for multiplication." << endl;
        return false;
    }

    if(matrix1_rows != rows) {
        cerr << "ERROR: Size of matrix does not match size from file." << endl;
        return false;
    }

    if(matrix2_cols != cols) {
        cerr << "ERROR: Size of matrix does not match size from file." << endl;
        return false;
    }

    return true;
}

void print_matrix(vector <vector<long long int>> matrix) {
    for (auto row : matrix) {
        for (auto element : row) {
            cout << element << ' ';
        }
        cout << endl;
    }
}
