#include <SFML/Graphics.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

const char SEPARATOR = ';';
sf::Font font;

struct TransitionData
{
	std::string arc;
	std::string end;
};

struct Automota
{
	std::vector<std::string> states;
	std::map<std::string, std::vector<TransitionData>> transitions;
};

struct Point
{
	float x;
	float y;
};

struct State
{
	sf::Text text;
	sf::CircleShape cirle;
};

struct Arrow
{
	sf::Text text;
	sf::VertexArray line;
};

std::string ParseArg(int argc, char* argv[])
{
	if (argc < 1)
	{
		throw std::invalid_argument("Invalid agrument count");
	}

	return argv[1];
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

	return inSymbol;
}

Automota ReadMureAutomataFromFile(std::ifstream& file, const std::vector<std::string>& outSymbols, std::string& line)
{
	Automota automata;
	auto states = ParseLineWithSeparator(line);
	std::map<std::string, std::string> funcReturn;

	for (int i = 0; i < states.size(); i++)
	{
		std::string state = states[i] + "/" + outSymbols[i];
		automata.states.push_back(state);
		funcReturn[states[i]] = state;
	}

	while (std::getline(file, line))
	{
		std::string inSymbol = GetFromLineInSymbol(line);
		auto transactions = ParseLineWithSeparator(line);

		for (int i = 0; i < transactions.size(); i++)
		{
			std::string end = funcReturn[transactions[i]];
			automata.transitions[automata.states[i]].push_back(TransitionData{ inSymbol, end });
		}
	}

	return automata;
}

Automota ReadMiliAutomataFromFile(std::ifstream& file, const std::vector<std::string>& states, std::string& line)
{
	Automota automata;
	automata.states = states;

	do
	{
		std::string inSymbol = GetFromLineInSymbol(line);
		auto transactions = ParseLineWithSeparator(line);

		for (int i = 0; i < transactions.size(); i++)
		{
			size_t posofOutSymbol = transactions[i].find('/');
			std::string end;
			std::string outSymbol;

			end.append(transactions[i], 0, transactions[i].length() - posofOutSymbol - 1);
			outSymbol.append(transactions[i], posofOutSymbol + 1, transactions[i].length());
			automata.transitions[automata.states[i]].push_back(TransitionData{ inSymbol + "/" + outSymbol, end });
		}

	} while (std::getline(file, line));

	return automata;
}

Automota ReadAutomataFromFile(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		throw std::invalid_argument("Unknown file name");
	}

	std::string line;

	std::getline(file, line);
	auto firstLine = ParseLineWithSeparator(line);

	std::getline(file, line);
	std::string inSymbol = GetFromLineInSymbol(line);

	if (inSymbol.empty())
	{
		return ReadMureAutomataFromFile(file, firstLine, line);
	}
	else
	{
		return ReadMiliAutomataFromFile(file, firstLine, line);
	}
}

std::vector<State> CreateShapesOfStates(const Automota& automata)
{
	const float x0 = 400;
	const float y0 = 400;
	const float dx = 300;
	const float dy = 300;
	const float pi = static_cast<float>(2 * acos(0));
	const float angle = 2 * pi / automata.states.size();

	std::vector<State> states;
	for (int i = 0; i < automata.states.size(); i++)
	{
		sf::Vector2f pos(x0 + dx * cos(i * angle), y0 + dy * sin(i * angle));

		State state;
		state.text.setString(automata.states[i]);
		state.text.setFont(font);
		state.text.setFillColor(sf::Color::White);
		state.text.setCharacterSize(15);

		const float radius = 30;
		state.text.setPosition(sf::Vector2f(pos.x + radius / 2, pos.y + radius / 1.5));
		state.cirle.setRadius(radius);
		state.cirle.setPosition(pos);
		int r = 50 + rand() % (255 - 50 + 1);
		int g = 50 + rand() % (255 - 50 + 1);
		int b = 50 + rand() % (255 - 50 + 1);
		state.cirle.setFillColor(sf::Color(r, g, b, 255));

		states.push_back(state);
	}

	return states;
}

std::vector<Arrow> CreateShapesOfTransactions(Automota& automata, const std::vector<State>& states)
{
	const float radius = states[0].cirle.getRadius();
	std::vector<Arrow> transactions;
	for (int i = 0; i < automata.states.size(); i++)
	{
		sf::Color color = states[i].cirle.getFillColor();
		std::string lastState;
		int repeatedCount = 0;
		for (auto& transaction : automata.transitions[automata.states[i]])
		{
			if (lastState == transaction.end)
			{
				repeatedCount++;
			}
			else
			{
				lastState = transaction.end;
				repeatedCount = 0;
			}


			const float offset = 10;
			sf::Vector2f pos0(states[i].cirle.getPosition());
			pos0.x += radius;
			pos0.y += radius;
			int endIndex = std::find(automata.states.begin(), automata.states.end(), transaction.end) - automata.states.begin();
			sf::Vector2f pos1(states[endIndex].cirle.getPosition());
			pos1.x += radius;
			pos1.y += radius;
			Arrow arrow;
			arrow.text.setString(transaction.arc);
			arrow.text.setFillColor(color);
			arrow.text.setPosition(sf::Vector2f(((pos0.x + pos1.x) / 2), ((pos0.y + pos1.y) / 2)));
			arrow.text.setFont(font);
			arrow.text.setCharacterSize(13);

			arrow.line = sf::VertexArray(sf::LineStrip, 8);
			if (pos0 == pos1)
			{
				arrow.line[0].position = pos0;
				arrow.line[0].color = color;
				arrow.line[7].position = pos0;
				arrow.line[7].color = color;
				arrow.line[1].position = sf::Vector2f(pos0.x - radius - repeatedCount * offset, pos1.y - radius / 2 - repeatedCount * offset);
				arrow.line[1].color = color;
				arrow.line[2].position = sf::Vector2f(pos0.x - radius - repeatedCount * offset, pos1.y - radius - repeatedCount * offset);
				arrow.line[2].color = color;
				arrow.line[3].position = sf::Vector2f(pos0.x - radius / 2 - repeatedCount * offset, pos1.y - radius * 2 - repeatedCount * offset);
				arrow.line[3].color = color;
				arrow.line[4].position = sf::Vector2f(pos0.x + radius / 2 + repeatedCount * offset, pos1.y - radius * 2 - repeatedCount * offset);
				arrow.line[4].color = color;
				arrow.line[5].position = sf::Vector2f(pos0.x + radius + repeatedCount * offset, pos1.y - radius - repeatedCount * offset);
				arrow.line[5].color = color;
				arrow.line[6].position = sf::Vector2f(pos0.x + radius + repeatedCount * offset, pos1.y - radius / 2 - repeatedCount * offset);
				arrow.line[6].color = color;
				arrow.text.setPosition(sf::Vector2f(pos0.x - radius / 2 - repeatedCount * offset, pos0.y - radius * 2 - repeatedCount * offset));
			}
			else
			{
				float angle = atan((pos1.y - pos0.y) / (pos1.x - pos0.x));
				float direction = 1.0f;
				if (pos0.x > pos1.x || (pos0.x == pos1.x && pos0.y > pos1.y))
				{
					direction = -1.0f;
				}

				arrow.line[0].position = pos0;
				arrow.line[0].color = color;
				arrow.line[7].position = pos1;
				arrow.line[7].color = color;
				arrow.line[1].position = sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.1f + 15.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.1f - 15.0f * direction * cos(angle) - (repeatedCount * offset));
				arrow.line[1].color = color;
				arrow.line[2].position = sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.2f + 25.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.2f - 25.0f * direction * cos(angle) - (repeatedCount * offset));
				arrow.line[2].color = color;
				arrow.line[3].position = sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.4f + 30.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.4f - 30.0f * direction * cos(angle) - (repeatedCount * offset));
				arrow.line[3].color = color;
				arrow.line[4].position = sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.6f + 30.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.6f - 30.0f * direction * cos(angle) - (repeatedCount * offset));
				arrow.line[4].color = color;
				arrow.line[5].position = sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.8f + 25.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.8f - 25.0f * direction * cos(angle) - (repeatedCount * offset));
				arrow.line[5].color = color;
				arrow.line[6].position = sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.9f + 15.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.9f - 15.0f * direction * cos(angle) - (repeatedCount * offset));
				arrow.line[6].color = color;
				arrow.text.setPosition(sf::Vector2f(pos0.x + (pos1.x - pos0.x) * 0.45f + 30.0f * direction * sin(angle) + (repeatedCount * offset), pos0.y + (pos1.y - pos0.y) * 0.45f - 20.0f * direction * cos(angle)- (repeatedCount * offset)));
			}

			transactions.push_back(arrow);
		}
	}

	return transactions;
}

void DrawStates(sf::RenderWindow& window, std::vector<State>& states)
{
	for (auto& shape : states)
	{
		window.draw(shape.cirle);
		window.draw(shape.text);
	}
}

void DrawTransactions(sf::RenderWindow& window, std::vector<Arrow>& arcs)
{
	for (auto& arc : arcs)
	{
		window.draw(arc.line);
		window.draw(arc.text);
	}
}

int main()
{
	std::vector<State> states;
	std::vector<Arrow> transactions;
	try
	{
		std::string fileName;
		std::cin >> fileName;
		auto automata = ReadAutomataFromFile(fileName);

		states = CreateShapesOfStates(automata);
		transactions = CreateShapesOfTransactions(automata, states);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 1;
	}

	sf::RenderWindow window(sf::VideoMode(1000, 800), "Automata");
	window.setVerticalSyncEnabled(true);
	if (!font.loadFromFile("SUSE-Medium.ttf"))
	{
		throw std::invalid_argument("File with font not found");
	}

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}
		window.clear(sf::Color::White);

		DrawTransactions(window, transactions);
		DrawStates(window, states);
		window.display();
	}

	return 0;
}
