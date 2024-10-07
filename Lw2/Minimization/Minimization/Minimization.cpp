#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

const char SEPARATOR = ';';

struct Args
{
	std::string inFileName;
	std::string outFileName;
};

std::vector<std::string> ParseLineWithSeparator(const std::string& line)
{
	std::vector<std::string> elts;

	size_t pos = 0;
	while (pos < line.length())
	{
		pos = line.find(SEPARATOR, pos);
		pos++;

		std::string elt;
		while (pos < line.length() && line[pos] != ' ' && line[pos] != SEPARATOR)
		{
			elt.push_back(line[pos]);
			pos++;
		}

		elts.push_back(elt);
	}

	if (elts.size() == 0)
	{
		throw std::invalid_argument("Line is empty or invalid");
	}

	return elts;
}

std::string GetFromLineInSymbol(const std::string line)
{
	size_t pos = 0;
	std::string inSymbol;

	while (pos < line.length() && line[pos] != ' ' && line[pos] != SEPARATOR)
	{
		inSymbol.push_back(line[pos]);

		pos++;
	}

	return inSymbol;
}

void TryOpenFiles(const std::string& inFileName, std::ifstream& inFile, const std::string& outFileName, std::ofstream& outFile)
{
	inFile.open(inFileName);
	if (!inFile.is_open())
	{
		throw std::invalid_argument("Cannot open file " + inFileName);
	}
	outFile.open(outFileName);
	if (!outFile.is_open())
	{
		throw std::invalid_argument("Cannot open file " + outFileName);
	}
}

std::vector<std::set<std::string>> GroupByOutSymbolTransactions(const std::vector<std::string>& states, std::map<std::string, std::vector<std::string>>& outTransactions)
{
	std::vector<std::set<std::string>> groups;
	std::map<std::vector<std::string>, int> uniqueOutSymbols;

	for (const auto& state : states)
	{
		if (uniqueOutSymbols.count(outTransactions[state]) == 0)
		{
			std::set<std::string> set = { state };
			groups.push_back(set);
			uniqueOutSymbols[outTransactions[state]] = groups.size() - 1;
		}
		else
		{
			int index = uniqueOutSymbols[outTransactions[state]];
			groups[index].insert(state);
		}
	}

	return groups;
}

std::vector<std::set<std::string>> GroupByOutSumbol(const std::vector<std::string>& states, std::map<std::string, std::string> funcReturn)
{
	std::vector<std::set<std::string>> groups;
	std::map<std::string, int> uniqueOutSymbols;

	for (const auto& state : states)
	{
		if (uniqueOutSymbols.count(funcReturn[state]) == 0)
		{
			std::set<std::string> set = { state };
			groups.push_back(set);
			uniqueOutSymbols[funcReturn[state]] = groups.size() - 1;
		}
		else
		{
			int index = uniqueOutSymbols[funcReturn[state]];
			groups[index].insert(state);
		}
	}

	return groups;
}

void UpdateTransactionsByGroups(std::map<std::string, std::vector<std::string>>& stateTransactions, const std::vector<std::set<std::string>> groups, std::map<std::string, std::vector<int>>& transactions)
{

	for (const auto& group : groups)
	{
		for (auto& state : group)
		{
			std::vector<std::string> oldTransactions = stateTransactions[state];
			std::vector<int> groupTransactions;

			for (int i = 0; i < oldTransactions.size(); i++)
			{
				for (int j = 0; j < groups.size(); j++)
				{
					if (groups[j].count(oldTransactions[i]) != 0)
					{
						groupTransactions.push_back(j);
						break;
					}
				}
			}
			transactions[state] = groupTransactions;
		}
	}
}

void MinimizateByGroup(std::map<std::string, std::vector<std::string>>& stateTransactions, std::vector<std::set<std::string>>& groups)
{
	bool isMinimizated = true;
	std::map<std::string, std::vector<int>> transactions;

	for (const auto& group : groups)
	{
		if (group.size() != 1)
		{
			isMinimizated = false;
			break;
		}
	}

	if (isMinimizated)
	{
		return;
	}

	UpdateTransactionsByGroups(stateTransactions, groups, transactions);

	while (!isMinimizated)
	{
		std::vector<std::set<std::string>> newGroups;

		for (const auto& group : groups)
		{
			std::map<std::vector<int>, int> uniqueTransaction;
			for (const auto& state : group)
			{
				if (uniqueTransaction.count(transactions[state]) == 0)
				{
					std::set<std::string> set = { state };
					newGroups.push_back(set);
					uniqueTransaction[transactions[state]] = newGroups.size() - 1;
				}
				else
				{
					newGroups[uniqueTransaction[transactions[state]]].insert(state);
				}
			}
		}

		if (groups == newGroups)
		{
			isMinimizated = true;
		}
		else
		{
			groups = newGroups;
			UpdateTransactionsByGroups(stateTransactions, groups, transactions);
		}
	}
}

void PrintGroupTransactionToFile(const std::string& state, int i, std::map<std::string, std::vector<std::string>>& stateTransactions, const std::vector<std::set<std::string>>& groups, std::ofstream& outFile)
{
	std::string end = stateTransactions[state][i];

	for (int j = 0; j < groups.size(); j++)
	{
		if (groups[j].count(end) != 0)
		{
			end = "s" + std::to_string(j);
			break;
		}
	}

	outFile << SEPARATOR << end;
}

void MinimizateMili(const std::vector<std::string>& states, std::string line, std::ifstream& inFile, std::ofstream& outFile)
{
	std::vector<std::string> inSymbols;
	std::map<std::string, std::vector<std::string>> stateTransactions;
	std::map<std::string, std::vector<std::string>> outTransactions;

	do
	{
		std::string inSymbol = GetFromLineInSymbol(line);
		inSymbols.push_back(inSymbol);

		auto ends = ParseLineWithSeparator(line);

		for (int i = 0; i < ends.size(); i++)
		{
			size_t posofOutSymbol = ends[i].find('/');
			std::string end;
			std::string outSymbol;

			outSymbol.append(ends[i], posofOutSymbol + 1, ends[i].length());
			end.append(ends[i], 0, ends[i].length() - outSymbol.length() - 1);

			stateTransactions[states[i]].push_back(end);
			outTransactions[states[i]].push_back(outSymbol);
		}

	} while (std::getline(inFile, line));

	auto groups = GroupByOutSymbolTransactions(states, outTransactions);

	MinimizateByGroup(stateTransactions, groups);

	for (int i = 0; i < groups.size(); i++)
	{
		outFile << SEPARATOR << "s" << i;
	}
	outFile << std::endl;

	for (int i = 0; i < inSymbols.size(); i++)
	{
		outFile << inSymbols[i];

		for (auto& group : groups)
		{
			std::string state = *group.begin();
			PrintGroupTransactionToFile(state, i, stateTransactions, groups, outFile);
			outFile << "/" << outTransactions[state][i];
		}

		outFile << std::endl;
	}
}

void MinimizateMure(const std::vector<std::string>& outSymbols, std::string line, std::ifstream& inFile, std::ofstream& outFile)
{
	auto states = ParseLineWithSeparator(line);
	std::map<std::string, std::string> funcReturn;
	for (int i = 0; i < states.size(); i++)
	{
		funcReturn[states[i]] = outSymbols[i];
	}
	std::vector<std::string> inSymbols;
	std::map<std::string, std::vector<std::string>> stateTransactions;
	std::map<std::string, std::vector<std::string>> outTransactions;

	while (std::getline(inFile, line))
	{
		std::string inSymbol = GetFromLineInSymbol(line);
		inSymbols.push_back(inSymbol);

		auto ends = ParseLineWithSeparator(line);

		for (int i = 0; i < ends.size(); i++)
		{
			stateTransactions[states[i]].push_back(ends[i]);
			outTransactions[states[i]].push_back(funcReturn[ends[i]]);
		}
	}

	auto groups = GroupByOutSumbol(states, funcReturn);

	MinimizateByGroup(stateTransactions, groups);

	for (const auto& group : groups)
	{
		std::string outSymbol = funcReturn[*group.begin()];
		outFile << SEPARATOR << outSymbol;
	}
	outFile << std::endl;
	for (int i = 0; i < groups.size(); i++)
	{
		outFile << SEPARATOR << "s" << i;
	}
	outFile << std::endl;

	for (int i = 0; i < inSymbols.size(); i++)
	{
		outFile << inSymbols[i];

		for (auto& group : groups)
		{
			std::string state = *group.begin();
			PrintGroupTransactionToFile(state, i, stateTransactions, groups, outFile);
		}

		outFile << std::endl;
	}
}

void MinimizateAutomata(const std::string& inFileName, const std::string& outFileName)
{
	std::ifstream inFile;
	std::ofstream outFile;
	TryOpenFiles(inFileName, inFile, outFileName, outFile);

	std::string line;

	std::getline(inFile, line);
	auto firstLine = ParseLineWithSeparator(line);

	std::getline(inFile, line);
	std::string inSymbol = GetFromLineInSymbol(line);

	if (inSymbol.empty())
	{
		MinimizateMure(firstLine, line, inFile, outFile);
	}
	else
	{
		MinimizateMili(firstLine, line, inFile, outFile);
	}
}

Args ParseArgs(int argc, char* argv[])
{
	if (argc < 3)
	{
		throw std::invalid_argument("Invalid arguments");
	}

	return Args{ argv[1], argv[2] };
}

int main()
{
	try
	{
		std::string inFileName;
		std::string outFileName;
		std::cin >> inFileName;
		std::cin >> outFileName;
		MinimizateAutomata(inFileName, outFileName);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}
