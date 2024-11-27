#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <vector>

std::string RemoveSpacesInStr(const std::string& str)
{
	std::string result;

	size_t pos = 0;

	while (pos < str.length())
	{
		size_t foundPos = str.find(" ", pos);
		result.append(str, pos, foundPos - pos);
		if (foundPos == std::string::npos)
		{
			break;
		}
		pos = foundPos + 1;
	}

	return result;
}

std::string ReadStrFromFile(const std::string& fileName)
{
	std::ifstream in(fileName);
	if (!in.is_open())
	{
		throw std::invalid_argument("Can't open input file");
	}

	std::string line;
	std::getline(in, line);

	return RemoveSpacesInStr(line);
}

struct State
{
	std::map<std::string, std::set<int>> next;
};

struct Group
{
	int start;
	std::set<int> ends;
};

struct Transaction
{
	int start;
	int end;
};

std::vector<State> CreateGraphFromRegex(const std::string& regex)
{
	std::vector<State> states = { State() };
	std::stack<Group> groupStates;
	groupStates.push(Group{ 0 });
	std::stack<Transaction> lastTr;
	int current = 0;

	for (size_t i = 0; i < regex.length(); i++)
	{
		if (regex[i] == '|')
		{
			groupStates.top().ends.insert(current);
			current = groupStates.top().start;

			continue;
		}

		if (regex[i] == '(')
		{
			states.push_back(State());
			states[current].next[""].insert(states.size() - 1);
			current = states.size() - 1;
			groupStates.push(Group{ current });

			continue;
		}

		if (regex[i] == ')')
		{
			states.push_back(State());
			states[current].next[""].insert(states.size() - 1);
			current = states.size() - 1;

			while (!lastTr.empty())
			{
				lastTr.pop();
			}
			lastTr.push({ groupStates.top().start, current });

			for (int index : groupStates.top().ends)
			{
				states[index].next[""].insert(current);
			}

			groupStates.pop();

			continue;
		}

		if (regex[i] == '*' || regex[i] == '+')
		{
			if (regex[i] == '*')
			{
				states[lastTr.top().start].next[""].insert(lastTr.top().end);
			}
			states[lastTr.top().end].next[""].insert(lastTr.top().start);

			lastTr.pop();

			continue;
		}

		states.push_back(State());
		states.push_back(State());
		states[current].next[""].insert(states.size() - 2);
		states[states.size() - 2].next[std::string(1, regex[i])].insert(states.size() - 1);
		current = states.size() - 1;

		while (!lastTr.empty())
		{
			lastTr.pop();
		}
		lastTr.push({ current - 1, current });
	}

	states.push_back(State());
	states[current].next[""].insert(states.size() - 1);

	for (int index : groupStates.top().ends)
	{
		states[index].next[""].insert(states.size() - 1);
	}

	groupStates.pop();

	return states;
}

struct DKA
{
	std::map<std::string, std::map<std::string, std::string>> transactions;
	std::set<std::string> inSymbols;
};

State AddEmptyTransactions(int index, std::vector<State>& states, std::set<int>& endStates)
{
	State newState = states[index];
	newState.next[""].clear();
	newState.next[""].insert(index);
	std::queue<int> queue;

	for (int empty : states[index].next[""])
	{
		queue.push(empty);
	}

	while (!queue.empty())
	{
		int child = queue.front();
		newState.next[""].insert(child);

		if (child == states.size() - 1)
		{
			endStates.insert(index);
		}

		for (auto& transaction : states[child].next)
		{
			for (auto& ends : transaction.second)
			{
				if (transaction.first == "")
				{
					if (!newState.next[""].contains(ends))
					{
						queue.push(ends);
					}
				}
				else
				{
					newState.next[transaction.first].insert(ends);
				}
			}
		}

		queue.pop();
	}

	return newState;
}

DKA CreateDKA(std::vector<State> states)
{
	std::set<int> endStates;
	for (int i = 0; i < states.size(); i++)
	{
		states[i] = AddEmptyTransactions(i, states, endStates);
	}

	std::vector<std::set<int>> newStates = { { 0 } };
	std::vector<std::map<std::string, int>> newTransactions;

	bool isCreate = true;
	int current = 0;

	while (isCreate)
	{
		std::map<std::string, std::set<int>> transactions;

		for (auto& state : newStates[current])
		{

			for (auto& transaction : states[state].next)
			{
				if (transaction.first == "")
				{
					continue;
				}

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

	std::vector<std::string> stateNames;

	for (size_t i = 0; i < newStates.size(); i++)
	{
		std::string name = "q" + std::to_string(i);

		bool isEnd = false;

		for (auto state : newStates[i])
		{
			if (endStates.contains(state))
			{
				isEnd = true;
				break;
			}
		}

		if (isEnd)
		{
			name += "(end)";
		}

		stateNames.push_back(name);
	}

	DKA dka;

	for (size_t i = 0; i < newTransactions.size(); i++)
	{
		std::map<std::string, std::string> transactions;

		for (auto& pair : newTransactions[i])
		{
			dka.inSymbols.insert(pair.first);
			transactions[pair.first] = stateNames[pair.second];
		}

		dka.transactions[stateNames[i]] = transactions;
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
		std::string regex = ReadStrFromFile(argv[1]);
		std::vector<State> states = CreateGraphFromRegex(regex);
		DKA dka = CreateDKA(states);
		WriteDKAToFile(dka, argv[2]);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}
