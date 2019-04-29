// TreeBuilder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "L-System.h";
#include "LTree.h"
#include <iostream>
#include <string>

int main()
{
	LSystem m_lsystem;
	LTree m_tree;
	bool cont = true;

    std::cout << "Welcome to the Tree Builder!\n\n";

	while (cont)
	{
		std::cout << "Please enter the L-Sytems axiom: ";

		std::string axiom;
		std::getline(std::cin, axiom);
		std::cout << std::endl << std::endl;
		m_lsystem.SetAxiom(axiom);

		std::cout << "Please enter the number of rules for the system: ";
		int numRules;
		std::cin >> numRules;
		std::cout << std::endl << std::endl;


		for (int i = 0; i < numRules; ++i)
		{
			char pre;
			std::cout << "Please enter rule #" << i +1 << "'s predecessor (single character): ";
			std::cin >> pre;
			std::cout << std::endl << std::endl;
			std::cin.clear();
			std::cin.ignore(256, '\n');

			std::string suc;
			std::cout << "Please enter rule #" << i+1 << "'s successor: ";
			std::getline(std::cin, suc);
			std::cout << std::endl << std::endl;

			m_lsystem.AddRule(pre, suc);
		}

		float stepSize;
		std::cout << "Please enter the length of tree segments (can be real number): ";
		std::cin >> stepSize;
		std::cout << std::endl << std::endl;

		float angle;
		std::cout << "Please enter the angle delta (can be real number): ";
		std::cin >> angle;
		std::cout << std::endl << std::endl;

		float angleDelta = (angle * XM_PI) / 180;

		float radius;
		std::cout << "Please enter the start radius of the tree (can be real number): ";
		std::cin >> radius;
		std::cout << std::endl << std::endl;
		m_tree.SetRadius(radius);

		int iterations;
		std::cout << "Please enter the number of times to run the LSystem (whole number): ";
		std::cin >> iterations;
		std::cin.clear();
		std::cin.ignore(256, '\n');
		std::cout << std::endl << std::endl;

		std::string filepath;
		std::cout << "Please enter the path you would like to save to (including filename): ";
		std::getline(std::cin, filepath);
		std::cout << std::endl << std::endl;

		std::cout << "Building Tree\n\n";
		std::string out = m_lsystem.RunSystem(iterations);
		Model* m_model = m_tree.InterpretSystem(out, stepSize, angleDelta);

		std::cout << "Writing to File...\n\n";
		m_model->WriteToFile(filepath + ".txt");

		char ans;
		std::cout << "Would you like to continue?: (y/n)";
		std::cin >> ans;
		std::cout << std::endl << std::endl;

		if (ans != 'y')
			cont = false;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
