#ifndef MATRIX_H_
#define MATRIX_H_

#include <vector>
using namespace std;

class Matrix {
	vector< vector<double> > value;

	public:

		Matrix(int r, int c);

		Matrix(vector<vector<double> > v);

		unsigned int numRows();

		unsigned int numColumns();

		double at(int r, int c);

		void set(double d, int r, int c);

		Matrix operator*(Matrix m);

		vector<double> operator*(vector<double> v);

		vector<double> getRow(int index);

		vector<double> getColumn(int index);

		Matrix sigmoid();

		void print();

		static double dot(vector<double> v1, vector<double> v2);

};



#endif /* MATRIX_H_ */
