#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <string>
#include <set>
#include <unordered_set>
#include <iterator>
#include <ctime>
#include <mutex>
#include <thread>
#include <map>

unsigned int maxCircuits;

std::mutex mtxGenerator;
unsigned int generatedCircuits = 0;
bool stopAlgorithm = false;
//String with the identifying letters for the gates of the repertoire
std::string binaryGates = "aonOxXlrLR";
std::ofstream fs;
size_t remainingFunctions = 0;


#pragma pack (1)
struct circuit {
	circuit* leftParent;
	circuit* rightParent;
	short size;
	unsigned int eval;
	char gate;
	circuit() {}
	circuit(circuit* leftParent, circuit* rightParent, unsigned int eval, short size, char gate) {
		this->leftParent = leftParent;
		this->rightParent = rightParent;
		this->eval = eval;
		this->size = size;
		this->gate = gate;
	}
};


// Map of circuit-size to circuits yet to be marked
std::map<short, std::vector<circuit*>> wait;

// Map of computed functions to vector of circuits that compute them (from left to right increasing in size)
std::unordered_map<unsigned int, std::vector<circuit*>> stock;

// Gate evaluation function.
unsigned int evaluation(circuit* circ1, circuit* circ2, char gate) {
	// AND
	if (gate == 'a') return circ1->eval & circ2->eval;
	// OR
	else if (gate == 'o') return circ1->eval | circ2->eval;
	// NOT
	else if (gate == 'N') return ~circ1->eval;
	// NAND
	else if (gate == 'n') return ~(circ1->eval & circ2->eval);
	// NOR
	else if (gate == 'O') return  ~(circ1->eval | circ2->eval);
	// XOR
	else if (gate == 'x') return circ1->eval ^ circ2->eval;
	// XNOR
	else if (gate == 'X') return  ~(circ1->eval ^ circ2->eval);
	// LONLY
	else if (gate == 'l') return  circ1->eval & (~circ2->eval);
	// RONLY
	else if (gate == 'r') return  (~circ1->eval) & circ2->eval;
	// NLONLY
	else if (gate == 'L') return  ~(circ1->eval & (~circ2->eval));
	// NRONLY
	else if (gate == 'R') return  ~((~circ1->eval) & circ2->eval);
}

// Prints the circuit as a binary tree
void printBT(std::ostream& o, const std::string& prefix, circuit const* circ, bool isRight)
{
	o << prefix;

	o << "|--";

	// We print the corresponding cable or gate
	if (circ->leftParent == nullptr && circ->rightParent == nullptr) {
		if (circ->eval == 2863311530) o << "X0\n";
		else if (circ->eval == 3435973836) o << "X1\n";
		else if (circ->eval == 4042322160) o << "X2\n";
		else if (circ->eval == 4278255360) o << "X3\n";
		else if (circ->eval == 4294901760) o << "X4\n";
	}
	// AND gate
	else if (circ->gate == 'a') {
		o << "AND\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// OR gate
	else if (circ->gate == 'o') {
		o << "OR\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// NOT gate
	else if (circ->gate == 'N') {
		o << "NOT\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// NAND gate
	else if (circ->gate == 'n') {
		o << "NAND\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// NOR gate
	else if (circ->gate == 'O') {
		o << "NOR\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// XOR gate
	else if (circ->gate == 'x') {
		o << "XOR\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// XNOR gate
	else if (circ->gate == 'X') {
		o << "XNOR\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// LONLY gate
	else if (circ->gate == 'l') {
		o << "LONLY\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// RONLY gate
	else if (circ->gate == 'r') {
		o << "RONLY\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// NLONLY gate
	else if (circ->gate == 'L') {
		o << "NLONLY\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}
	// NRONLY gate
	else if (circ->gate == 'R') {
		o << "NRONLY\n";
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->rightParent, true);
		printBT(o, prefix + (isRight ? "|   " : "    "), circ->leftParent, false);
	}

}

void bin(std::ostream& o, unsigned int n)
{
	unsigned int num = n;
	for (int i = 0; i < 32; ++i) {
		o << (num & 1);
		num = num >> 1;
	}
}

void showCircuitDetails(std::ostream& o, circuit* const& circ) {
	o << "Size: " << circ->size;
	o << "\n";
	printBT(o, "", circ, false);
	o << " Evaluation: ";
	bin(o, circ->eval);
	o << "\n\n";
}

void recursivePreorder(std::ostream& o, circuit* const& circ) {
	if (circ->leftParent == nullptr && circ->rightParent == nullptr) {
		if (circ->eval == 2863311530) o << '0';
		else if (circ->eval == 3435973836) o << '1';
		else if (circ->eval == 4042322160) o << '2';
		else if (circ->eval == 4278255360) o << '3';
		else if (circ->eval == 4294901760) o << '4';
	}
	else if (circ->gate == 'N') {
		o << circ->gate;
		recursivePreorder(o, circ->leftParent);
	}
	else {
		o << circ->gate;
		recursivePreorder(o, circ->leftParent);
		recursivePreorder(o, circ->rightParent);
	}
}


std::ostream& operator<<(std::ostream& o, circuit* circ) {
	recursivePreorder(o, circ);
	o << " " << circ->size << " " << circ->eval << '\n';
	return o;
}


void pushInSet(std::set<circuit*>& set, circuit* circ) {
	if (circ != nullptr) {
		set.insert(circ);
		pushInSet(set, circ->leftParent);
		pushInSet(set, circ->rightParent);
	}
}

void merge(circuit* circuit1, circuit* circuit2, int& size) {

	std::set<circuit*> set1, set2;
	pushInSet(set1, circuit1);
	pushInSet(set2, circuit2);

	std::set<circuit*> intersect;
	std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersect, intersect.begin()));

	for (circuit* circ : intersect) {
		if (circ->size > 0) size--;
	}

}

// Initialization function that includes all 0-sized circuits with their evaluation
void initialize() {
	wait[0] = std::vector<circuit*>();

	circuit* circ = new circuit(nullptr, nullptr, 2863311530, 0, 'C');
	wait[0].push_back(circ);

	circ = new circuit(nullptr, nullptr, 3435973836, 0, 'C');
	wait[0].push_back(circ);

	circ = new circuit(nullptr, nullptr, 4042322160, 0, 'C');
	wait[0].push_back(circ);

	circ = new circuit(nullptr, nullptr, 4278255360, 0, 'C');
	wait[0].push_back(circ);

	circ = new circuit(nullptr, nullptr, 4294901760, 0, 'C');
	wait[0].push_back(circ);

}



void taskGenerateCircuits(short n, size_t start, size_t end) {
	for (char gate : binaryGates) {
		for (size_t i = start; i < end; ++i) {
			circuit* circuit1 = wait[n][i];

			// Merges the circuit with all others of thge same size with whom it has not already been merged
			for (size_t j = i + 1; j < wait[n].size(); ++j) {
				circuit* circuit2 = wait[n][j];
				int size = circuit1->size + circuit2->size + 1;
				merge(circuit1, circuit2, size);
				mtxGenerator.lock();
				if (!wait.count(size)) wait[size] = std::vector<circuit*>();
				wait[size].push_back(new circuit(circuit1, circuit2, evaluation(circuit1,circuit2, gate), size, gate));
				generatedCircuits++;
				if (stopAlgorithm) {
					mtxGenerator.unlock();
					return;
				}
				else if (generatedCircuits == maxCircuits) {
					stopAlgorithm = true;
					mtxGenerator.unlock();
					return;
				}
				else if (generatedCircuits % 5000000 == 0) {
					std::cout << generatedCircuits / 1000000 << '\n';
					mtxGenerator.unlock();
				}
				else mtxGenerator.unlock();
			}

			// Merges the circuit with all circuits of smaller size
			for (auto par2 : stock) {
				for (circuit* circuit2 : par2.second) {
					if (circuit1 == circuit2) break;
					int size = circuit1->size + circuit2->size + 1;
					merge(circuit1, circuit2, size);
					mtxGenerator.lock();
					if (!wait.count(size)) wait[size] = std::vector<circuit*>();
					wait[size].push_back(new circuit(circuit1, circuit2, evaluation(circuit1, circuit2, gate), size, gate));
					generatedCircuits++;
					if (stopAlgorithm) {
						mtxGenerator.unlock();
						return;
					}
					else if (generatedCircuits == maxCircuits) {
						stopAlgorithm = true;
						mtxGenerator.unlock();
						return;
					}
					else if (generatedCircuits % 5000000 == 0) {
						std::cout << generatedCircuits / 1000000 << '\n';
						mtxGenerator.unlock();
					}
					else mtxGenerator.unlock();
				}
			}
		}
	}
	// NOT gate (unary gate)
	for (circuit* circuit1 : wait[n]) {
		int size = circuit1->size + 1;
		mtxGenerator.lock();
		if (!wait.count(size)) wait[size] = std::vector<circuit*>();
		wait[size].push_back(new circuit(circuit1, nullptr, evaluation(circuit1, nullptr, 'N'), size, 'N'));
		generatedCircuits++;
		if (stopAlgorithm) {
			mtxGenerator.unlock();
			return;
		}
		else if (generatedCircuits == maxCircuits) {
			stopAlgorithm = true;
			mtxGenerator.unlock();
			return;
		}
		else if (generatedCircuits % 5000000 == 0) {
			std::cout << generatedCircuits / 1000000 << '\n';
			mtxGenerator.unlock();
		}
		else mtxGenerator.unlock();
	}
}

void generateCircuits(short n) {
	if (!wait.count(n)) return;
	size_t numProcessors = std::thread::hardware_concurrency();
	size_t processSize = (wait[n].size() / numProcessors);
	std::vector<std::thread> threads(numProcessors);
	for (size_t i = 0; i < numProcessors; ++i) {
		if (i < numProcessors - 1) threads[i] = std::thread(taskGenerateCircuits, n, i * processSize, (i + 1) * processSize);
		else threads[i] = std::thread(taskGenerateCircuits, n, i * processSize, wait[n].size());
	}

	for (size_t i = 0; i < numProcessors; ++i) {
		threads[i].join();
	}

}

void noteCircuits(short n) {
	if (!wait.count(n)) return;
	for (circuit* circ : wait[n]) {
		unsigned int eval = circ->eval;
		if (!stock.count(eval)) {
			stock[eval] = std::vector<circuit*>();
			fs << circ;
		}
		stock[eval].push_back(circ);
	}
	std::cout << "Size " << n << " marked\n";

}

inline void takeOutWaitingCircuits(short n) {
	wait[n].clear();
	wait.erase(n);
}


void taskMarkRemaining(std::vector<circuit*> const& v, int start, int end) {
	for (int i = start; i < end; ++i) {
		circuit* circ = v[i];
		mtxGenerator.lock();
		if (!stock.count(circ->eval)) {
			stock[circ->eval] = std::vector<circuit*>();
			remainingFunctions++;
			fs << circ;
		}
		mtxGenerator.unlock();
		delete circ;
	}
}


void markRemaining() {
	std::vector<std::pair<int, std::vector<circuit*>>> remaining(wait.size());
	std::transform(wait.begin(), wait.end(), remaining.begin(), [](auto pair) {return std::move(pair); });
	wait.clear();
	sort(remaining.begin(), remaining.end(), [](auto pair1, auto pair2) {return pair1.first < pair2.first; });
	for (auto par : remaining) {

		int numProcessors = std::thread::hardware_concurrency();
		int processSize = par.second.size() / numProcessors;
		std::vector<std::thread> threads(numProcessors);
		for (int i = 0; i < numProcessors; ++i) {
			if (i < numProcessors - 1) threads[i] = std::thread(taskMarkRemaining, par.second, i * processSize, (i + 1) * processSize);
			else threads[i] = std::thread(taskMarkRemaining, par.second, i * processSize, par.second.size());
		}
		for (int i = 0; i < numProcessors; ++i) {
			threads[i].join();
		}
		std::cout << "Size " << par.first << " marked\n";
	}
}



inline void printRemainingCount() {
	for (auto par : wait) {
		std::cout << par.second.size() << " circuits of size " << par.first << " yet to be processed\n";
	}
}

inline void freeStock() {
	for (auto par : stock) {
		for (circuit* circ : par.second) {
			delete circ;
		}
		par.second.clear();
	}
	stock.clear();
}

int main() {
	std::cout << "Number of circuits to generate:\n";
	std::cin >> maxCircuits;
	unsigned t0, t1;
	t0 = clock();
	std::string fileName;
	std::cout << "Output file name:\n";
	std::cin >> fileName;
	fs = std::ofstream(fileName);
	initialize();
	int size = 0;
	while (!stopAlgorithm) {
		generateCircuits(size);
		noteCircuits(size);
		takeOutWaitingCircuits(size);
		size++;
	}
	std::cout << "Exhaustively generated circuits up to size: " << size - 1 << '\n';
	printRemainingCount();
	std::cout << "\n";
	std::cout << "Processing... \n";
	markRemaining();
	std::cout << "Total of different functions computed: " << stock.size() + remainingFunctions << '\n';
	fs << "Total of different functions computed: " << stock.size() + remainingFunctions << '\n';
	freeStock();
	t1 = clock();
	double time = (double(t1 - t0) / CLOCKS_PER_SEC);
	fs << "Execution time: " << time << "\n";
	fs.close();
	return 0;
}