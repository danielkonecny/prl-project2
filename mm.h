/**
 * PRL - Project - Mesh Multiplication
 * Implementation of parallel sorting algorithm called Pipeline Merge Sort.
 * @file            pms.h
 * @version			1.0
 * @author			Daniel Konecny (xkonec75)
 * @organisation	Brno University of Technology - Faculty of Information Technologies
 * @date			29. 04. 2021
 */

#include <vector>
#include <mpi.h>

using namespace std;

int main (int argc, char *argv[]);

vector <vector<long long int>> load_matrix(const char* file_name);

int process_matrix(vector <vector<long long int>> *matrix);

bool check_matrices(vector <vector<long long int>> matrix1,
                    vector <vector<long long int>> matrix2,
                    int rows,
                    int cols);

void print_matrix(vector <vector<long long int>> matrix);
