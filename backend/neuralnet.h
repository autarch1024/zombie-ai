/*
 * Includes the neural network code
 */

#ifndef NEURALNET_H_
#define NEURALNET_H_

#include "matrix.h"
#include <vector>
using namespace std;

class NeuralNet {
private:
	vector<int> layerSizes;
	vector<Matrix> layerWeights;

	const static short int threshold = 0;
	const static int weightRange = 10;
	const static int inputSize = 9;
	const static int outputSize = 4;

	double fRand(double fMin, double fMax);
	vector<double> sigmoid(vector<double> v);

public:
	NeuralNet();
	NeuralNet(vector<int> sizes);
	void randomize();
	Matrix getWeights(int index);
	void setWeights(int index, Matrix m);
	vector<double> getOutput(vector<double> input);
	NeuralNet getChild(NeuralNet parent, float mutOdds);
	vector<int> getLayerSizes();
	void print();

};

#endif /* NEURALNET_H_ */
