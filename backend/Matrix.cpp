/*
 * Used for the linear algebra bits of the neural network
 */

#include <vector>
#include <cmath>
#include <iostream>
#include "matrix.h"
using namespace std;

Matrix::Matrix(int r, int c) {
	for (int i = 0; i < r; i++) {
		value.push_back(vector<double>(c));
	}
}

Matrix::Matrix(vector<vector<double> > v) {
	value = v;
}

unsigned int Matrix::numRows() {
	return value.size();
}

unsigned int Matrix::numColumns() {
	return value.at(0).size();
}

double Matrix::at(int r, int c) {
	double d = value.at(r).at(c);
	return d;
}

void Matrix::set(double d, int r, int c) {
	value[r][c] = d;
}

Matrix Matrix::operator*(Matrix m) {
	if (m.numRows() != numColumns()) return Matrix(0,0);

	vector<vector <double> > v;

	for (unsigned int r = 0; r < numRows(); r++) {
		vector<double> row;
		for (unsigned int c = 0; c < m.numColumns(); c++) {
			row.push_back(dot(getRow(r), m.getColumn(c)));
		}
		v.push_back(row);
	}

	return Matrix(v);
}

vector<double> Matrix::operator*(vector<double> v) {
	//if (v.size() != numColumns()) return *(new vector<double>());

	vector<double> returnVector;

	for (unsigned int r = 0; r < numRows(); r++) {
		returnVector.push_back(dot(getRow(r), v));
	}

	return returnVector;
}

//Returns the given row as a vector
vector<double> Matrix::getRow(int index) {
	vector<double> r;

	for (unsigned int i = 0; i < value[index].size(); i++) {
		r.push_back(value[index][i]);
	}

	return r;
}

//Returns the given column as a vector
vector<double> Matrix::getColumn(int index) {
	vector<double> r;

	for (unsigned int i = 0; i < value.size(); i++) {
		r.push_back(value[i][index]);
	}

	return r;
}

//Returns a sigmoided version of this matrix
Matrix Matrix::sigmoid() {
	Matrix matt(numRows(), numColumns());

	for (unsigned int r = 0; r < numRows(); r++) {
		for (unsigned int c = 0; c < numColumns(); c++) {

			matt.set((matt.at(r,c))/(1-abs(matt.at(r,c))),r,c);

		}
	}

	return matt;
}

void Matrix::print() {
	for (unsigned int r = 0; r < numRows(); r++) {

		for (unsigned int c = 0; c < numColumns(); c++) {

			cout << at(r,c) << " ";

		}
		cout << endl;
	}
}


//Dot product of two vectors
double Matrix::dot(vector<double> v1, vector<double> v2) {
	double sum = 0;

	for (unsigned int i = 0; i < v1.size(); i++) {
		sum += v1.at(i) * v2.at(i);
	}

	return sum;
}
