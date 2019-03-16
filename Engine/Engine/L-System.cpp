
#include "L-System.h"

LSystem::LSystem(std::string axiom) : m_axiom(axiom), m_rules()
{
	srand(time(nullptr));
}

std::string LSystem::RunSystem(int iterations)
{
	std::string curWord = m_axiom;
	std::string next = "";

	std::unordered_map<int, std::string> rewriteMap;

	for (int i = 0; i < iterations; ++i)
	{
		for (char c : curWord)
		{
			bool replaced = false;

			for (ProductionRule r : m_rules)
			{
				if (ApplyRule(r.probability))
				{
					if (c == r.predecessor)
					{
						next.append(r.successor);
						replaced = true;
						break;
					}
				}
			}
			if (!replaced)
				next.push_back(c);
		}

		curWord = next;
		next.clear();
	}
	return curWord;
}

bool LSystem::AddRule(char pre, std::string suc, float prob)
{
	for (ProductionRule r : m_rules)
	{
		if (r.predecessor == pre)
		{
			if (prob >= 1.0f || suc == r.successor)
				return false;
		}
	}
			

	m_rules.push_back(ProductionRule(pre, suc, prob));
	return true;
}

bool LSystem::AddRule(char pre, std::string suc)
{
	return AddRule(pre, suc, 1.0f);
}

bool LSystem::ApplyRule(float p)
{
	double r = (double)rand() / RAND_MAX;
	return r <= p;
}

void LSystem::SetAxiom(std::string axiom)
{
	m_axiom = axiom;
}