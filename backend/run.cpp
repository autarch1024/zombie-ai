#include <time.h>
#include <iostream>
#include <vector>
#include "neuralnet.h"
#include "gm.h"
using namespace std;

const int NUM_CHAMPS = 3;
const int NUM_NETS = 10;
const int NUM_GENS = 10;
const double MUTATION_CHANCE = 0.1;
const double WORLD_HEIGHT = 1000;
const double WORLD_WIDTH = 1000;


struct Contestant {
	NeuralNet net;
	long int score;
};

bool operator>(Contestant c1, Contestant c2) {
	return c1.score > c2.score;
}

bool operator<(Contestant c1, Contestant c2) {
	return c1.score < c2.score;
}

bool operator==(Contestant c1, Contestant c2) {
	return c1.score == c2.score;
}

bool operator>=(Contestant c1, Contestant c2) {
	return c1.score >= c2.score;
}

bool operator<=(Contestant c1, Contestant c2) {
	return c1.score <= c2.score;
}


//Insertion sort the contestants
vector<Contestant> sortContestants(vector<Contestant> contestants) {
	vector<Contestant> r;

	if (contestants.size() > 0) {
		r.push_back(contestants[0]);
		contestants.erase(contestants.begin());
	}

	while (!contestants.empty()) {
		bool added = false;
		for (unsigned int i = 0; i < r.size(); i++) {
			if (r[i] < contestants[0]) {
				added = true;
				r.insert(r.begin()+i, contestants[0]);
				contestants.erase(contestants.begin());
				break;
			}
		}

		if (!added) {
			r.push_back(contestants[0]);
			contestants.erase(contestants.begin());
		}
	}

	return r;
}

vector<NeuralNet> runGeneration(vector<NeuralNet> champs, int sanityCheck) {
	vector<NeuralNet> nets;

	//Get all the nets
	for (unsigned int i = 0; i < champs.size(); i++) {

		nets.push_back(champs[i]);

		for (unsigned int j = i; j < champs.size(); j++) {

			nets.push_back(champs[i].getChild(champs[j], MUTATION_CHANCE));

		}
	}

	while (nets.size() < NUM_NETS) {
		NeuralNet nn(champs[0].getLayerSizes());
		nn.randomize();
		nets.push_back(nn);
	}

	//Run the games
	long int time = clock();
	vector<Contestant> challengers;

	for (NeuralNet nn : nets) {
		GameMaster gm(nn);

		Contestant c;
		c.net = nn;
		cout << "Running game..." << endl;
		c.score = gm.run(sanityCheck, time);
		challengers.push_back(c);
	}
	cout << "All games complete" << endl;
	challengers = sortContestants(challengers);
	cout << "Contestants sorted: ";
	for (Contestant c : challengers) cout << c.score << " ";
	cout << endl;

	//Return the nets
	vector<NeuralNet> retNets;
	for (unsigned int i = 0; i < NUM_CHAMPS && i < challengers.size(); i++)
		retNets.push_back(challengers[i].net);

	return retNets;

}

int main() {
	vector<int> sizes;
	for (int i = 8; i > 4; i--) sizes.push_back(i);

	vector<NeuralNet> v;
	NeuralNet nn(sizes);
	nn.randomize();
	v.push_back(nn);

	for (int i = 1; i <= NUM_GENS; i++) {
		cout << "Running generation " << i << endl;
		v = runGeneration(v, 3000);
	}
	//*/

}


