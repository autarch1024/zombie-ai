#include <time.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "neuralnet.h"
#include "gm.h"
using namespace std;

const int NUM_CHAMPS = 3;			//Number of nets that carry on from the previous gen
const int NUM_NETS = 12;			//Number of nets to test in a generation
const int NUM_GENS = 1000;			//Number of generations to run
const int NUM_SEEDS = 10; 			//Number of unique level seeds to be used (<= 0 means infinite)
const double MUTATION_CHANCE = 0.3; //Chance a given weight in a child net will change
const double WORLD_HEIGHT = 1000;
const double WORLD_WIDTH = 1000;

int currentGen = 0;
ofstream file;

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
	for (unsigned int i = 0; i < champs.size() && nets.size() < NUM_NETS; i++) {

		nets.push_back(champs[i]);

		for (unsigned int j = i; j < champs.size() && nets.size() < NUM_NETS; j++) {

			nets.push_back(champs[i].getChild(MUTATION_CHANCE));

		}
	}

	while (nets.size() < NUM_NETS) {
		NeuralNet nn;
		nets.push_back(nn);
	}

	//Run the games
	long int time = clock();
	if (NUM_SEEDS > 0) time %= NUM_SEEDS;
	vector<Contestant> challengers;

	for (NeuralNet nn : nets) {
		GameMaster gm(nn);

		Contestant c;
		c.net = nn;
		//cout << "Running game..." << endl;
		c.score = gm.run(sanityCheck, time);
		challengers.push_back(c);
	}
	//cout << "All games complete" << endl;
	challengers = sortContestants(challengers);

	cout << "Top scores: ";
	for (unsigned int i = 0; i < NUM_CHAMPS && i < challengers.size(); i++)
		cout << challengers[i].score << " ";
	cout << endl;


	//if (currentGen % 100 == 0 || currentGen == NUM_GENS - 1) {
		file << currentGen << "," << challengers[0].score << endl;
		//cout << "Printed score to file" << endl;
	//}

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
	NeuralNet nn;
	v.push_back(nn);

	file.open("scores4.txt");

	for (currentGen = 0; currentGen <= NUM_GENS; currentGen++) {
		cout << "Running generation " << currentGen << endl;
		v = runGeneration(v, 3000);
	}

	file.close();

	cout << endl;
	v[0].print();
	if (!v.empty()) v[0].printToFile("net.ai");
	//*/

}


