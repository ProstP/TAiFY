#include <fstream>
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

void GetFromLineTransient(const std::string& line, std::vector<std::string>& x, std::vector<std::string>& y,
	std::map<StringPair, StringPair>& funcTrans, std::set<StringPair>& results)
{
	size_t pos = 0;
}

void FromMiliToMure()
{
	std::ifstream mili;
	std::ofstream mure;
	OpenFilesWithAutomata("Mili.txt", mili, "Mure.txt", mure);
	//OpenFilesWithAutomata(AskFileName("mili"), mili, AskFileName("mure"), mure);

	std::string line;
	std::getline(mili, line);
	std::vector<std::string> s = ParseLineWithSeparator(line);
	std::vector<std::string> x;
	std::vector<std::string> y;
	std::map<StringPair, StringPair> funcTrans;
	std::set<StringPair> resultsOfTransients;

	while (std::getline(mili, line))
	{
		std::string x = GetFromLineInSymbol(line);
		std::vector<std::string> elts = ParseLineWithSeparator(line);

		for (int i = 0; i < elts.size(); i++)
		{
			std::string state;
			std::string outSymbol;

			size_t posOfOutSymbol = elts[i].find('/');
			state.append(elts[i], 0, elts[i].size() - posOfOutSymbol - 1);
			outSymbol.append(elts[i], posOfOutSymbol + 1, elts[i].length());
			StringPair pair{ state, outSymbol };
			resultsOfTransients.insert(pair);
			funcTrans[StringPair{ s[i], x }] = pair;
		}
	}

	for (auto func : funcTrans)
	{
		std::cout << func.first.first << "!" << func.first.second << "!" << std::endl;
		std::cout << func.second.first << "!" << func.second.second << "!" << std::endl <<std::endl;
	}

	for (auto pair : resultsOfTransients)
	{
		std::cout << pair.first << " " << pair.second << std::endl;
	}
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
		mode = "0";

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
