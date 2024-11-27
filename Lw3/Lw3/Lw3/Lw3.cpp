#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

struct Alphabet
{
	std::string start;
	std::vector<std::string> states;
	std::vector<std::vector<std::string>> transactions;
};

struct NKA
{
	std::string start;
	std::string end;
	std::map<std::string, std::map<std::string, std::set<std::string>>> transactions;
	std::set<std::string> inSymbols;
};

struct DKA
{
	std::map<std::string, std::map<std::string, std::string>> transactions;
	std::set<std::string> inSymbols;
};

void AskFileName(std::string& input, std::string& output)
{
	std::cout << "Enter input file name: ";
	std::cin >> input;
	std::cout << "Enter output file name: ";
	std::cin >> output;
}

Alphabet ReadAlphabet(const std::string& inFileName)
{
	std::ifstream inFile(inFileName);
	if (!inFile.is_open())
	{
		throw std::invalid_argument("Cannot open input file");
	}

	std::string line;

	Alphabet alphabet;

	while (std::getline(inFile, line))
	{
		std::string state;
		std::string transition;
		std::vector<std::string> transitions;

		bool isState = true;

		for (int i = 0; i < line.length(); i++)
		{
			if (std::isspace(line[i]) || line[i] == '-')
			{
				continue;
			}

			if (line[i] == '>' && line[i - 1] == '-')
			{
				isState = false;
				continue;
			}

			if (line[i] == '|')
			{
				transitions.push_back(transition);
				transition = "";

				continue;
			}

			if (isState)
			{
				state += line[i];
			}
			else
			{
				transition += line[i];
			}
		}

		if (!transition.empty())
		{
			transitions.push_back(transition);
		}

		alphabet.states.push_back(state);
		alphabet.transactions.push_back(transitions);
	}

	inFile.close();
	alphabet.start = alphabet.states[0];
	return alphabet;
}

NKA AlphabetToNKA(Alphabet& alphabet)
{
	NKA nka;
	bool isRight = true;

	nka.transactions["End"] = std::map<std::string, std::set<std::string>>();

	for (size_t i = 0; i < alphabet.states.size(); i++)
	{
		for (std::string& transaction : alphabet.transactions[i])
		{
			std::size_t pos = std::string::npos;
			std::string end;

			for (std::string& state : alphabet.states)
			{
				pos = transaction.find(state);
				if (pos != std::string::npos)
				{
					end = state;
					break;
				}
			}
			std::string symbol;

			if (pos == std::string::npos)
			{
				end = "End";
				symbol = transaction;
			}
			else
			{
				if (pos != 0)
				{
					isRight = true;
					symbol = transaction.substr(0, pos);
				}
				else
				{
					isRight = false;
					symbol = transaction.substr(end.length());
				}
			}

			if (isRight)
			{
				nka.transactions[alphabet.states[i]][symbol].insert(end);
			}
			else
			{
				nka.transactions[end][symbol].insert(alphabet.states[i]);
			}

			nka.inSymbols.insert(symbol);
		}
	}

	if (isRight)
	{
		nka.start = alphabet.start;
		nka.end = "End";
	}
	else
	{
		nka.start = "End";
		nka.end = alphabet.start;
	}

	return nka;
}

DKA NKAToDKA(NKA& nka)
{
	DKA dka;
	dka.inSymbols = nka.inSymbols;
	std::vector<std::set<std::string>> newStates = { { nka.start } };
	std::vector<std::map<std::string, int>> newTransactions;

	int current = 0;
	bool isCreate = true;
	while (isCreate)
	{
		std::map<std::string, std::set<std::string>> transactions;
		for (auto state : newStates[current])
		{
			for (auto& transaction : nka.transactions[state])
			{
				transactions[transaction.first].insert(transaction.second.begin(), transaction.second.end());
			}
		}

		std::map<std::string, int> newTransaction;
		for (auto transaction : transactions)
		{

			int index;
			auto it = std::find(newStates.begin(), newStates.end(), transaction.second);
			if (it == newStates.end())
			{
				newStates.push_back(transaction.second);
				index = newStates.size() - 1;
			}
			else
			{
				index = it - newStates.begin();
			}

			newTransaction[transaction.first] = index;
		}

		newTransactions.push_back(newTransaction);

		if (newStates.size() - 1 > current)
		{
			current++;
		}
		else
		{
			isCreate = false;
		}
	}

	std::vector<std::string> states;
	for (size_t i = 0; i < newStates.size(); i++)
	{
		std::string state = "q" + std::to_string(i);

		auto it = std::find(newStates[i].begin(), newStates[i].end(), nka.end);

		if (it != newStates[i].end())
		{
			state += "(end)";
		}

		states.push_back(state);
	}

	for (size_t i = 0; i < newTransactions.size(); i++)
	{
		std::map<std::string, std::string> transactions;

		for (auto& pair : newTransactions[i])
		{
			transactions[pair.first] = states[pair.second];
		}

		dka.transactions[states[i]] = transactions;
	}

	return dka;
}

void WriteDKAToFile(DKA& dka, const std::string& fileName)
{
	std::ofstream out(fileName);

	if (!out.is_open())
	{
		throw std::invalid_argument("Cannot open output file");
	}

	for (auto& state : dka.transactions)
	{
		out << ";" << state.first;
	}

	out << std::endl;

	for (auto& inSymbol : dka.inSymbols)
	{
		out << inSymbol;
		for (auto& state : dka.transactions)
		{
			out << ";" << (state.second.contains(inSymbol) ? state.second[inSymbol] : "-");
		}

		out << std::endl;
	}
}

int main(int argc, char* argv[])
{
	try
	{
		Alphabet alphabet = ReadAlphabet(argv[1]);
		NKA nka = AlphabetToNKA(alphabet);
		DKA dka = NKAToDKA(nka);
		WriteDKAToFile(dka, argv[2]);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
