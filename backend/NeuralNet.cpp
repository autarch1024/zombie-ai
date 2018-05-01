/*
 * The actual brains
 */
#include <time.h>
#include "neuralnet.h"
#include <vector>
#include <cstdlib>
#include <cmath>
#include "matrix.h"
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
		r[i] = v[i] / (1 - abs(v[i]));
	}

	return r;
}

NeuralNet::NeuralNet() {
	NeuralNet(vector<int>());
}

NeuralNet::NeuralNet(vector<int> sizes) {
	//Assign layer sizes
	for (unsigned int i = 0; i < sizes.size(); i++) {
		layerSizes.push_back(sizes[i]);
	}

	if (sizes.size() > 0) {
		//Add weight matrix between input layer and first hidden layer
		layerWeights.push_back( Matrix(sizes[0],inputSize) );

		//Add weight matrices between the hidden layers
		for (unsigned int i = 0; i < sizes.size() - 1; i++) {
			layerWeights.push_back( Matrix(sizes.at(i+1), sizes.at(i)) );
		}


		//Add weight matrix between last hidden layer and output layer
		layerWeights.push_back( Matrix(outputSize, sizes[sizes.size()-1]) );

	} else {

		//If no hidden layers, assign weight matrix between input and output layers
		layerWeights.push_back( Matrix(outputSize, inputSize) );

	}
}

//Fill the neural network with random weights
void NeuralNet::randomize() {
	srand(clock());

	for (unsigned int i = 0; i < layerWeights.size(); i++) {
		//Matrix m = layerWeights[i];

		for (unsigned int r = 0; r < layerWeights[i].numRows(); r++) {
			for (unsigned int c = 0; c < layerWeights[i].numColumns(); c++) {
				layerWeights[i].set(fRand(-weightRange,weightRange),r,c);

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

	if (input.size() != inputSize)
		cout << "Your inputs are a different size than expected: " <<  inputSize << " " << input.size() << endl;


	for (unsigned int i = 0; i < layerWeights.size(); i++) {
		Matrix m = layerWeights[i];

		input = sigmoid(m*input);

		for (unsigned int i = 0; i < input.size(); i++) {
			if (input[i] < threshold) input[i] = 0;
		}

	}
	return input;
}

vector<int> NeuralNet::getLayerSizes() {
	return layerSizes;
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

				if (fRand(0,1) < mutOdds) m.set(fRand(-weightRange, weightRange),r,c);
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

