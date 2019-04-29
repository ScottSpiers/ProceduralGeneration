#pragma once

#include <unordered_map>
#include <vector>
#include <time.h>

class LSystem
{
private:
	struct ProductionRule
	{
		//could change this to two constructors with defaults
		ProductionRule(char pre, std::string suc) : ProductionRule(pre, suc, 1.0f) {};
		ProductionRule(char pre, std::string suc, float prob) : predecessor(pre), successor(suc), probability(prob) {};
		/*ProductionRule(std::string pre, std::string suc) : ProductionRule(pre, suc, 1.0f, 1) {};
		ProductionRule(std::string pre, std::string suc, float prob) : ProductionRule(pre, suc, prob, 1) {};
		ProductionRule(std::string pre, std::string suc, int nParams) : ProductionRule(pre, suc, 1.0f, nParams) {};
		ProductionRule(std::string pre, std::string suc, float prob, int nParams) : predecessor(pre), successor(suc), probability(prob), numParams(nParams) {};*/

		char predecessor;
		std::string successor;
		float probability;
		int numParams;
	};

public:
	LSystem(std::string = "F" , bool = false);

	std::string RunSystem(int);
	bool AddRule(char, std::string);
	bool AddRule(char, std::string, float p);
	/*bool AddRule(std::string, std::string);
	bool AddRule(std::string, std::string, float);
	bool AddRule(std::string, std::string, int);
	bool AddRule(std::string, std::string, float, int);*/
	

	void SetAxiom(std::string);

private:
	/*int ParseParameters(std::string);
	void ApplyArithmetic(std::string&);*/
	bool ApplyRule(float);

private:
	std::string m_axiom;
	std::vector<ProductionRule> m_rules;

	//std::unordered_map<char, int> m_varMap;

	bool m_isParametric;
};