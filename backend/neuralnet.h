/*
 * Includes the neural network code
 */

#ifndef NEURALNET_H_
#define NEURALNET_H_

#include "matrix.h"
#include <vector>
#include <string>
using namespace std;

class NeuralNet {
private:
	vector<int> layerSizes;
	vector<Matrix> layerWeights;

	const static short int THRESHOLD = 0;
	const static int WEIGHT_RANGE = 10;
	const static int INPUT_SIZE = 9;
	const static int OUTPUT_SIZE = 4;
	const static int MAX_LAYERS = 10;

	double fRand(double fMin, double fMax);
	vector<double> sigmoid(vector<double> v);

public:
	NeuralNet();
	NeuralNet(vector<int> sizes);
	void randomize();
	Matrix getWeights(int index);
	void setWeights(int index, Matrix m);
	vector<double> getOutput(vector<double> input);
	NeuralNet getChild(float mutOdds);
	NeuralNet getChild(NeuralNet parent, float mutOdds);
	vector<int> getLayerSizes();
	void print();
	void printToFile(string s);

};

#endif /* NEURALNET_H_ */
