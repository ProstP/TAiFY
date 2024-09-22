﻿#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

const char SEPARATOR = ';';

struct StringPair
{
	std::string first;
	std::string second;

	bool operator==(const StringPair& other) const
	{
		return this->first == other.first && this->second == other.second;
	}
	bool operator<(const StringPair& other) const
	{
		if (this->first < other.first)
		{
			return true;
		}
		if (this->first > other.first)
		{
			return false;
		}
		if (this->second < other.second)
		{
			return true;
		}
		return false;
	}
	bool operator>(const StringPair& other) const
	{
		if (this->first > other.first)
		{
			return true;
		}
		if (this->first < other.first)
		{
			return false;
		}
		if (this->second > other.second)
		{
			return true;
		}
		return false;
	}
};

struct MiliData
{
	std::vector<std::string> states;
	std::vector<std::string> inSymbols;
	std::map<StringPair, StringPair> funcTrans;
	std::set<StringPair> endsOfTrances;
};

std::string AskFileName(const std::string& type)
{
	std::cout << "Enter name of file with " << type << " automata" << std::endl;
	std::string fileName;
	std::cin >> fileName;
	return fileName;
}

void OpenFilesWithAutomata(const std::string& inFileName, std::ifstream& inFile, const std::string& outFileName, std::ofstream& outFile)
{
	inFile.open(inFileName);
	if (!inFile.is_open())
	{
		throw std::invalid_argument("Unknwon filename " + inFileName);
	}
	outFile.open(outFileName);
	if (!outFile.is_open())
	{
		throw std::invalid_argument("Unknwon filename " + outFileName);
	}
}

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

	if (inSymbol.empty())
	{
		throw std::invalid_argument("In symbol is empty");
	}

	return inSymbol;
}

MiliData GetDataAboutMiliAutomata(std::istream& in)
{
	MiliData data;

	std::string line;
	std::getline(in, line);
	data.states = ParseLineWithSeparator(line);

	while (std::getline(in, line))
	{
		std::string x = GetFromLineInSymbol(line);
		data.inSymbols.push_back(x);
		std::vector<std::string> elts = ParseLineWithSeparator(line);

		for (int i = 0; i < elts.size(); i++)
		{
			std::string state;
			std::string outSymbol;

			size_t posOfOutSymbol = elts[i].find('/');
			state.append(elts[i], 0, elts[i].size() - posOfOutSymbol - 1);
			outSymbol.append(elts[i], posOfOutSymbol + 1, elts[i].length());
			StringPair pair{ state, outSymbol };
			data.endsOfTrances.insert(pair);
			data.funcTrans[StringPair{ data.states[i], x }] = pair;
		}
	}

	return data;
}

std::map<StringPair, std::string> CreateMureNodesAndWriteToFile(std::set<StringPair> endOfTrances, std::ostream& out)
{
	std::map<StringPair, std::string> nodes;
	auto node = endOfTrances.begin();
	for (int i = 0; i < endOfTrances.size(); i++)
	{
		nodes[*node] = "q" + std::to_string(i);
		out << SEPARATOR << nodes[*node];
		node++;
	}

	return nodes;
}

void WriteMureTransitionToFile(MiliData data, std::map<StringPair, std::string> mureNodes, std::ostream& out)
{
	for (int i = 0; i < data.inSymbols.size(); i++)
	{
		out << data.inSymbols[i];

		for (auto state : data.endsOfTrances)
		{
			out << SEPARATOR << mureNodes[data.funcTrans[StringPair{ state.first, data.inSymbols[i] }]];
		}

		out << std::endl;
	}
}

void FromMiliToMure()
{
	std::ifstream mili;
	std::ofstream mure;
	OpenFilesWithAutomata(AskFileName("mili"), mili, AskFileName("mure"), mure);

	MiliData data = GetDataAboutMiliAutomata(mili);

	for (auto pair : data.endsOfTrances)
	{
		mure << SEPARATOR << pair.second;
	}
	mure << std::endl;
	auto q = CreateMureNodesAndWriteToFile(data.endsOfTrances, mure);
	mure << std::endl;
	WriteMureTransitionToFile(data, q, mure);
}

void FromMureToMili()
{
}

int main()
{
	std::cout << "Select mode:" << std::endl
			  << "[0] From mili to mure" << std::endl
			  << "[1] From mure to mili" << std::endl;

	while (true)
	{
		std::string mode;
		//std::cin >> mode;
		mode = "1";

		try
		{
			if (mode == "0")
			{
				FromMiliToMure();
				break;
			}
			else if (mode == "1")
			{
				FromMureToMili();
				break;
			}
			else
			{
				std::cout << "Uknown mode" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}

	return 0;
}
