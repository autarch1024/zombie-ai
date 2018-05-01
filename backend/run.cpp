#include <time.h>
#include <iostream>
#include <vector>
#include "neuralnet.h"
#include "gm.h"
using namespace std;

const int NUM_CHAMPS = 3;
const int NUM_NETS = 10;
const double MUTATION_CHANCE = 0.1;
const double WORLD_HEIGHT = 1000;
const double WORLD_WIDTH = 1000;


struct Contestant {
	NeuralNet net;
	long int score;
};

//Insertion sort the contestants
vector<Contestant> sortContestants(vector<Contestant> contestants) {
	vector<Contestant> r;

	if (contestants.size() > 0) r.push_back(contestants[0]);

	for (Contestant c : contestants) {

		bool added = false;

		for (vector<Contestant>::iterator i = r.begin(); i != r.end(); i++) {

			if (c.score > (*i).score) {
				r.insert(i,c);
				added = true;
			}

		}

		if (!added) r.push_back(c);
	}

	return r;
}

vector<NeuralNet> runGeneration(vector<NeuralNet> champs, int sanityCheck) {
	vector<NeuralNet> nets;

	//cout << "Running gen" << endl;

	//Get all the nets
	for (unsigned int i = 0; i < champs.size(); i++) {

		nets.push_back(champs[i]);

		for (unsigned int j = i; j < champs.size(); j++) {

			nets.push_back(champs[i].getChild(champs[j], MUTATION_CHANCE));

		}
	}
	//cout << "Champs added" << endl;

	while (nets.size() < NUM_NETS) {
		NeuralNet nn(champs[0].getLayerSizes());
		nn.randomize();
		nets.push_back(nn);
	}

	//cout << "Random units added" << endl;

	//Run the games
	long int time = clock();
	vector<Contestant> challengers;

	for (NeuralNet nn : nets) {
		//cout << "Getting game ready..." << endl;
		GameMaster gm(nn);

		Contestant c;
		c.net = nn;
		cout << "Running game..." << endl;
		c.score = gm.run(sanityCheck, time);
		cout << c.score << endl;
		challengers.push_back(c);
	}

	sortContestants(challengers);

	//Return the nets
	vector<NeuralNet> retNets;
	for (Contestant c : challengers) retNets.push_back(c.net);

	//cout << "Gen done" << endl;
	return retNets;

}

int main() {
	/*
	vector<int*> v;
	for (int i = 0; i < 9; i++) {
		int var = i;
		v.push_back(&var);
	}

	for (int i = 0; i < v.size(); i++) {
		cout << *(v[i]) << " ";
	}
	cout << endl;

	v.erase(v.begin() + 4);

	for (int* i : v) cout << *i << " ";
	*/

	vector<int> sizes;
	for (int i = 8; i > 4; i--) sizes.push_back(i);

	vector<NeuralNet> v;
	NeuralNet nn(sizes);
	nn.randomize();
	v.push_back(nn);

	long int time = clock();
	runGeneration(v, 300);
	cout << clock() - time;
	//*/

}


