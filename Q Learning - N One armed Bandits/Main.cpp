// Reference http://outlace.com/Reinforcement-Learning-Part-1/
// Aim to reproduce code fome above source in an attempt to learn how reinforced learning works

// N Bandit problem - There are n one armed bandits each with a preset pay out limit. The aim is to learn which machine to play to maxamise payout.

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>

struct Dynamic_Sort {
	// Identifies column n as the one to be sorted.
	Dynamic_Sort(int paramA) { this->paramA = paramA; }
	bool operator () (std::vector<double> i, std::vector<double> j) { return i[paramA] < j[paramA]; }

	int paramA;
};

void Print_Vector(std::vector<std::vector<double>> Vec) {
	for (std::vector<std::vector<double>>::size_type i = 0; i < Vec.size(); i++) {
		for (std::vector<double>::size_type j = 0; j < Vec[i].size(); j++) {
			std::cout << Vec[i][j] << "  ";
		}
		std::cout << std::endl;
	}
}

double Get_Reward(double Prob) {
	// Return reward for playing 10 games on a given slot machine which is identified by it predetermined probability.
	// Slot machine probability is set in main function.

	int Reward = 0;
	std::random_device rd;
	std::mt19937 Generate(rd());
	std::uniform_real_distribution<double> Probibility(0, 1);

	for (int i = 0; i < 10; i++) {
		if (Probibility(Generate) < Prob) {
			Reward++;
		}
	}
	return Reward;
}

//// old
//std::vector<double> Get_Best_Bandit(std::vector<std::vector<double>> AV) {
//	std::vector<std::vector<double>>Averages;
//	std::vector<double> Bandit_Ave;
//
//	std::sort(AV.begin(), AV.end());
//
//	int ID = AV[0][0];
//	int count = 0;
//	double sum = 0;
//	for (std::vector<std::vector<double>>::size_type i = 0; i < AV.size(); i++) {
//
//		if (AV[i][0] == ID) {
//			count++;
//			sum += AV[i][1];
//		}
//		else {
//			Bandit_Ave.push_back(ID);
//			Bandit_Ave.push_back(sum / count);
//			Averages.push_back(Bandit_Ave);
//			Bandit_Ave.clear();
//			ID = AV[i][0];
//		}
//	}
//	Bandit_Ave.push_back(ID);
//	Bandit_Ave.push_back(sum / count);
//	Averages.push_back(Bandit_Ave);
//
//	std::sort(Averages.rbegin(), Averages.rend(), Dynamic_Sort(1));
//
//	return Averages[0];
//}
//
std::vector<double> Get_Best_Machine(std::vector<std::vector<double>>AV2) {
	// AV is a 2D vector of the form [machine ID][Number of plays][Average reward]
	// The vector is sorted using the standard C sort function with and addation parameter which takes the column you wish to sort
	// in this case this is column 2 the average reward. it is stored highest to lowest
	// function return the idenity of the machine with the highest average reward.
	std::sort(AV2.rbegin(), AV2.rend(), Dynamic_Sort(2));
	return AV2[0];
}

int main() {
	// Set up random number generation
	std::random_device rd;
	std::mt19937 Generate(rd());
	std::uniform_real_distribution<double> Probibility(0, 1);
	std::uniform_int_distribution<int> Machine(0, 9);

	// Inital settings
	int n = 10; // Number of Machines
	int Best_Arm = 0; // Inital best machine to play.
	double Eps = 0.2; // defines how likly we just select the current best arm.
	double Best_Average = 0; // Current payout for the 'best' machine.
	std::vector<double> Slot_Machine; // Stores probabilites of each machine

	// Set inital probibilites for each slot machine
	for (int i = 0; i < n; i++) {
		Slot_Machine.push_back(Probibility(Generate));
	}

	// Vector to record actions taken - initalised to 0. [Machine ID][Number of plays][Average reward]
	std::vector<std::vector<double>> AV2;
	for (int a = 0; a < n; a++) {
		std::vector<double> Row;
		Row.push_back(a);
		Row.push_back(0);
		Row.push_back(0);
		AV2.push_back(Row);
	}

	std::cout << "\n Ver 1.0\n\n";
	std::ofstream Out("Output.txt");

	// Play a total of 10000 games.
	for (int i = 0; i < 10000; i++) {
		double X = Probibility(Generate);	// Generates a random probability.
											// If this is less than Eps the current best machine is played else a machine is choosen at random.
		std::vector<double> ThisAV;			// Stores current action
		if (X < Eps) {
			ThisAV.push_back(Best_Arm);		// Machince ID
			ThisAV.push_back(Get_Reward(Slot_Machine[Best_Arm])); // Reward for this play
			AV2[Best_Arm][1] = AV2[Best_Arm][1]++;
			// Incremital update to current averages. this prevent the need for storing every action taken.
			AV2[Best_Arm][2] = AV2[Best_Arm][2] + ((ThisAV[1] - AV2[Best_Arm][2]) / AV2[Best_Arm][1]);
			std::vector<double> Best_Machine = Get_Best_Machine(AV2); // Get the current best machine
			Best_Arm = Best_Machine[0];	// Get current best machine
			Best_Average = Best_Machine[2]; // Get average reward for playing the best machine.
		}
		else {
			Best_Arm = Machine(Generate);
			ThisAV.push_back(Best_Arm);
			ThisAV.push_back(Get_Reward(Slot_Machine[Best_Arm]));
			AV2[Best_Arm][1] = AV2[Best_Arm][1]++;
			AV2[Best_Arm][2] = AV2[Best_Arm][2] + ((ThisAV[1] - AV2[Best_Arm][2]) / AV2[Best_Arm][1]);
			std::vector<double> Best_Bandit = Get_Best_Machine(AV2);
			Best_Arm = Best_Bandit[0];
			Best_Average = Best_Bandit[2];
		}

		// Print current results.
		if (i % 1000 == 0) { std::cout << "Best Bandit: " << Best_Arm << "   Average Reward: " << Best_Average << std::endl; }
		Out << std::setw(5) << i << "  " << Best_Arm << "  " << Best_Average << std::endl;
	}

	Out << std::endl << std::endl;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < 3; j++) {
			Out << std::setw(5) << AV2[i][j] << "  ";
		}
		Out << std::endl;
	}

	// Print out the orignal win rate for each machine.
	std::cout << "\n Actaul win rates\n\n";
	for (int i = 0; i < 10; i++) {
		std::cout << "Bandit " << i << " win percentage: " << Slot_Machine[i] << std::endl;
		Out << std::setw(5) << i << "  " << Slot_Machine[i] << std::endl;
	}
	Out.close();

	// Version 3
	std::vector<std::vector<double>> AV3;
	for (int a = 0; a < n; a++) {
		std::vector<double> Row;
		Row.push_back(a);
		Row.push_back(0);
		Row.push_back(0);
		AV3.push_back(Row);
	}

	std::ofstream Out2("Out2.txt");
	// ver 3 softmax
	std::cout << "\n Ver 3.0\n\n";

	std::vector<double>Weight_Array;
	for (int i = 0; i < n; i++) { Weight_Array.push_back(1); } // initalise array to 0 for each machine.
	std::discrete_distribution<int> Weights(Weight_Array.begin(), Weight_Array.end());
	for (int i = 0; i < 10000; i++) {
		Best_Arm = Weights(Generate);
		std::vector<double> ThisAV;

		ThisAV.push_back(Best_Arm);
		ThisAV.push_back(Get_Reward(Slot_Machine[Best_Arm]));
		AV3[Best_Arm][1] = AV3[Best_Arm][1]++;
		AV3[Best_Arm][2] = AV3[Best_Arm][2] + ((ThisAV[1] - AV3[Best_Arm][2]) / AV3[Best_Arm][1]);

		for (int j = 0; j < n; j++) { Weight_Array[j] = AV3[j][2] / i; } // convert ave to % for weighted random no.

		std::vector<double> Best_Bandit = Get_Best_Machine(AV3);
		Best_Arm = Best_Bandit[0];
		Best_Average = Best_Bandit[2];

		if (i % 1000 == 0) { std::cout << "Best Bandit: " << Best_Arm << "   Average Reward: " << Best_Average << std::endl; }
		Out2 << std::setw(5) << i << "  " << Best_Arm << "  " << Best_Average << std::endl;
	}

	// Print out the orignal win rate for each machine.
	std::cout << "\n Actaul win rates\n\n";
	for (int i = 0; i < 10; i++) {
		std::cout << "Bandit " << i << " win percentage: " << Slot_Machine[i] << std::endl;
		Out2 << std::setw(5) << i << "  " << Slot_Machine[i] << std::endl;
	}
	Out2.close();

	return 0;
}