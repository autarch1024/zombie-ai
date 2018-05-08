/*
 * The actual brains
 */
#include <time.h>
#include "neuralnet.h"
#include <vector>
#include <cstdlib>
#include <cmath>
#include "matrix.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

double NeuralNet::fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}

vector<double> NeuralNet::sigmoid(vector<double> v) {
	vector<double> r(v.size());

	for (unsigned int i = 0; i < v.size(); i++) {
		if (v[i] != 1) r[i] = v[i] / (1 - abs(v[i]));
		else r[i] = v[i] / (1 - abs(v[i] + 0.000001));
	}

	return r;
}

//Sets up the net as completely random (including the number and size of the layers)
NeuralNet::NeuralNet() {
	//Get the number of layers
	srand(clock());
	int numLayers = rand() % MAX_LAYERS + 1;
	layerSizes = *(new vector<int>(numLayers));

	/*Sets up the sizes of the layers such that every layer is <= to the one before it
	 *and >= to the one after it*/
	if (numLayers == 1) {
		//If there is only one layer
		layerSizes[0] = rand() % (INPUT_SIZE - OUTPUT_SIZE) + OUTPUT_SIZE;
	} else {
		//First layer size
		layerSizes[0] = rand() % (INPUT_SIZE - OUTPUT_SIZE + 1) + OUTPUT_SIZE;

		for (unsigned int i = 1; i < layerSizes.size()-1; i++) {
			layerSizes[i] = rand() % (layerSizes[i-1] - OUTPUT_SIZE + 1) + OUTPUT_SIZE;
		}
		//Last layer size
		layerSizes[numLayers - 1] = rand() % (layerSizes[numLayers - 2] - OUTPUT_SIZE + 1) + OUTPUT_SIZE;

	}

	//Sets up the weight matrices
	if (layerSizes.size() > 0) {
		//Add weight matrix between input layer and first hidden layer
		layerWeights.push_back( Matrix(layerSizes[0],INPUT_SIZE) );

		//Add weight matrices between the hidden layers
		for (unsigned int i = 0; i < layerSizes.size() - 1; i++) {
			layerWeights.push_back( Matrix(layerSizes.at(i+1), layerSizes.at(i)) );
		}


		//Add weight matrix between last hidden layer and output layer
		layerWeights.push_back( Matrix(OUTPUT_SIZE, layerSizes[layerSizes.size()-1]) );

	} else {

		//If no hidden layers, assign weight matrix between input and output layers
		layerWeights.push_back( Matrix(OUTPUT_SIZE, INPUT_SIZE) );

	}

	//Randomizes the weight matrices
	randomize();
}

NeuralNet::NeuralNet(vector<int> sizes) {
	//Assign layer sizes
	for (unsigned int i = 0; i < sizes.size(); i++) {
		layerSizes.push_back(sizes[i]);
	}

	if (sizes.size() > 0) {
		//Add weight matrix between input layer and first hidden layer
		layerWeights.push_back( Matrix(sizes[0],INPUT_SIZE) );

		//Add weight matrices between the hidden layers
		for (unsigned int i = 0; i < sizes.size() - 1; i++) {
			layerWeights.push_back( Matrix(sizes.at(i+1), sizes.at(i)) );
		}


		//Add weight matrix between last hidden layer and output layer
		layerWeights.push_back( Matrix(OUTPUT_SIZE, sizes[sizes.size()-1]) );

	} else {

		//If no hidden layers, assign weight matrix between input and output layers
		layerWeights.push_back( Matrix(OUTPUT_SIZE, INPUT_SIZE) );

	}
}

//Fill the neural network with random weights
void NeuralNet::randomize() {
	srand(clock());

	for (unsigned int i = 0; i < layerWeights.size(); i++) {
		//Matrix m = layerWeights[i];

		for (unsigned int r = 0; r < layerWeights[i].numRows(); r++) {
			for (unsigned int c = 0; c < layerWeights[i].numColumns(); c++) {
				layerWeights[i].set(fRand(-WEIGHT_RANGE, WEIGHT_RANGE),r,c);

			}
		}
	}
}

Matrix NeuralNet::getWeights(int index) {
	return layerWeights[index];
}

void NeuralNet::setWeights(int index, Matrix m) {
	if (m.numRows() != layerWeights[index].numRows()) return;
	if (m.numColumns() != layerWeights[index].numColumns()) return;
	layerWeights[index] = m;
}

//Run the given input through the network and return the output
vector<double> NeuralNet::getOutput(vector<double> input) {

	if (input.size() != INPUT_SIZE)
		cout << "Your inputs are a different size than expected: " <<  INPUT_SIZE << " " << input.size() << endl;


	for (unsigned int i = 0; i < layerWeights.size(); i++) {
		Matrix m = layerWeights[i];

		input = sigmoid(m*input);

		for (unsigned int j = 0; j < input.size() && i < layerWeights.size() - 1; j++) {
			if (input[j] < THRESHOLD) input[j] = 0;
		}

	}/*
	cout << "NN output: ";
	for (double d : input) cout << d << " ";
	cout << endl;
	*/
	return input;
}

vector<int> NeuralNet::getLayerSizes() {
	return layerSizes;
}

//Returns a slightly-mutated child of this net
NeuralNet NeuralNet::getChild(float mutOdds) {
	mutOdds = abs(fmod(mutOdds, 1));

	NeuralNet child(layerSizes);

	for (unsigned int i = 0; i < layerWeights.size(); i++) {
		Matrix m = child.getWeights(i);
		for (unsigned int r = 0; r < m.numRows(); r++) {
			for (unsigned int c = 0; c < m.numColumns(); c++) {

				double d1 = layerWeights[i].at(r,c);

				if (fRand(0,1) < mutOdds) m.set(fRand(-WEIGHT_RANGE, WEIGHT_RANGE),r,c);
				else {
					m.set(d1,r,c);;
				}
			}
		}
		child.setWeights(i,m);

	}
	return child;
}

//Get the child network of this and another net
NeuralNet NeuralNet::getChild(NeuralNet parent, float mutOdds) {
	if (getLayerSizes() != parent.getLayerSizes()) return NeuralNet();

	mutOdds = abs(fmod(mutOdds, 1));

	NeuralNet child(layerSizes);

	for (unsigned int i = 0; i < layerWeights.size(); i++) {
		Matrix m = child.getWeights(i);
		for (unsigned int r = 0; r < m.numRows(); r++) {
			for (unsigned int c = 0; c < m.numColumns(); c++) {

				double d1 = layerWeights[i].at(r,c);
				double d2 = parent.getWeights(i).at(r,c);

				if (fRand(0,1) < mutOdds) m.set(fRand(-WEIGHT_RANGE, WEIGHT_RANGE),r,c);
				else {
					float randNum = (float)fRand(0,1);
					if (randNum < 0.5f) m.set(d1,r,c);
					else m.set(d2,r,c);
				}
			}
		}
		child.setWeights(i,m);

	}
	return child;
}

void NeuralNet::print() {

	for (Matrix m : layerWeights) {
		m.print();
		cout << endl;
	}
}

void NeuralNet::printToFile(string s) {
	ofstream file;
	file.open(s);

	for (Matrix m : layerWeights) {
		for (unsigned int r = 0; r < m.numRows(); r++) {
			for (unsigned int c = 0; c < m.numColumns(); c++) {
				file << m.at(r,c) << " ";
			}
			file << endl;
		}
		file << endl;
	}

	file.close();
}

