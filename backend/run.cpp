#include <time.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <limits>
#include "neuralnet.h"
#include "gm.h"
using namespace std;

const int NUM_CHAMPS = 3;			//Number of nets that carry on from the previous gen
const int NUM_NETS = 12;			//Number of nets to test in a generation
int NUM_GENS = 100;				//Number of generations to run
int PRINT_CHECKPOINT = 0;
const int NUM_SEEDS = 10; 			//Number of unique level seeds to be used (<= 0 means infinite)
const double MUTATION_CHANCE = 0.3; //Chance a given weight in a child net will change
const double WORLD_HEIGHT = 1000;
const double WORLD_WIDTH = 1000;
bool silent = false;

int currentGen = 0;
ofstream scoreFile;
string gameFile = "";
string netFile = "";

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

	if (!silent) {
		cout << "Top scores: ";
		for (unsigned int i = 0; i < NUM_CHAMPS && i < challengers.size(); i++)
			cout << challengers[i].score << " ";
		cout << endl;
	}


	//Return the nets
	vector<NeuralNet> retNets;
	for (unsigned int i = 0; i < NUM_CHAMPS && i < challengers.size(); i++)
		retNets.push_back(challengers[i].net);

	return retNets;

}

void parseTerminalInput(int argc, char *argv[]) {

	try {
		string s = argv[1];
		NUM_GENS = stoi(s);
	} catch (const exception&) {
		//Prints the help text
		cout << "Syntax:" << endl;
		cout << "zombieai [NUM_GENS] : runs the genetic algorithm for [NUM_GENS] generations and outputs the resulting neural network in net.ai" << endl << endl;
		cout << "Arguments:" << endl;
		cout << "	help : displays this message" << endl;
		cout << "	-s : runs the program in silent mode" << endl;
		cout << "	-w [FILE] : outputs the neural net to [FILE]" << endl;
		cout << "	-p [FILE]: prints a game with the final neural network to [FILE] (Default gamelog.txt)" << endl;
		cout << "	-c [NUM]: periodically prints a game every [NUM] generations" << endl;

		exit(0);
	}

	for (int i = 2; i < argc; i++) {

		string s = argv[i];

		//Runs the program in silent mode
		if (s == "-s") silent = true;

		//Writes the winning net to the given file
		else if (s == "-w" && i + 1 < argc) {
			if (i + 1 < argc && argv[i+1] != "-w" && argv[i+1] != "-c" && argv[i+1] != "-p" && argv[i+1] != "-s") {
				i++;
				netFile = argv[i];
			}
		}
		//Prints a game with the winning neural net to
		else if (s == "-p") {
			if (i + 1 < argc && argv[i+1] != "-w" && argv[i+1] != "-c" && argv[i+1] != "-p" && argv[i+1] != "-s") {
				i++;
				gameFile = (argv[i]);
			} else {
				gameFile = ("gamelog.txt");
			}
		}
		//Prints a game periodically
		else if (s == "-c" && i + 1 < argc) {
			i++;
			PRINT_CHECKPOINT = std::stoi(argv[i]);
		}
	}

}

int main(int argc, char *argv[]) {

	parseTerminalInput(argc, argv);

	vector<int> sizes;
	for (int i = 8; i > 4; i--) sizes.push_back(i);

	vector<NeuralNet> v;
	NeuralNet nn;
	v.push_back(nn);

	for (currentGen = 0; currentGen <= NUM_GENS; currentGen++) {
		if (!silent) cout << "Running generation " << currentGen << endl;
		v = runGeneration(v, 3000);

		if (PRINT_CHECKPOINT != 0 && currentGen % PRINT_CHECKPOINT == 0) {

			//ofstream file;

			stringstream ss;
			ss << "gen" << currentGen << ".txt";
			string fileName;
			ss >> fileName;

			//file.open(fileName);

			GameMaster gm(v[0]);
			gm.run(fileName, numeric_limits<int>::max(), clock());
		}
	}

	scoreFile.close();

	cout << endl;
	if (!silent) v[0].print();
	if (!v.empty() && netFile != "") v[0].printToFile(netFile);
	else if (!v.empty()) v[0].printToFile("net.ai");

	if (gameFile != "") {
		if (!silent) cout << "Printing game..." << endl;
		GameMaster gm(v[0]);
		gm.run(gameFile, numeric_limits<int>::max(), clock());
	}
	//*/

}


