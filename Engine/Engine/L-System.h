#pragma once

#include <unordered_map>
#include <vector>
#include <time.h>

class LSystem
{
private:
	struct ProductionRule
	{
		ProductionRule(char pre, std::string suc) : ProductionRule(pre, suc, 1.0f) {};
		ProductionRule(char pre, std::string suc, float prob) : predecessor(pre), successor(suc), probability(prob) {};

		char predecessor;
		std::string successor;
		float probability;
	};

public:
	LSystem(std::string);

	std::string RunSystem(int);
	bool AddRule(char, std::string);
	bool AddRule(char, std::string, float p);
	
	void SetAxiom(std::string);

private:
	bool ApplyRule(float);

private:
	std::string m_axiom;
	std::vector<ProductionRule> m_rules;
};