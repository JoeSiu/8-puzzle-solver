#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <windows.h>
#include "8-Puzzle_Solver.h"

using namespace std;

string goalValue;
int goalState[gridSize][gridSize];


struct Point
{
	int row;
	int col;

	Point()
	{
		row = 0;
		col = 0;
	}

	Point(int r, int c)
	{
		row = r;
		col = c;
	}

	Point indexToPoint(int index)
	{
		Point pos;

		for (int row = 0; row < gridSize; row++)
		{
			for (int col = 0; col < gridSize; col++)
			{
				if (goalState[row][col] == index)
				{
					pos.row = row;
					pos.col = col;
					break;
				}
			}
		}
		return pos;
	}

	int pointToIndex()
	{
		return row * gridSize + col;
	}
};

struct Tile
{
	int value;
	int goalValue;

	Tile()
	{
		value = -1;
		goalValue = -1;
	}
};

Tile puzzle[gridSize][gridSize];

// Store the puzzle state
class State
{
public:
	Tile grid[gridSize][gridSize];
	State* parent; // Pointer to parent node in the close set
	Point blankPos;
	vector<Point> neighbours;
	int gScore;
	int hScore;
	int fScore;

	State()
	{
		parent = NULL;
		blankPos = Point(-1, -1);
		gScore = 0;
		hScore = 0;
		fScore = INFINITY;
	}

	void setGrid(Tile inputGrid[gridSize][gridSize])
	{
		for (int row = 0; row < gridSize; row++)
		{
			for (int col = 0; col < gridSize; col++)
			{
				grid[row][col].value = inputGrid[row][col].value;
				grid[row][col].goalValue = inputGrid[row][col].goalValue;

				// Set the blank tile's position
				if (grid[row][col].value == 0)
				{
					blankPos.row = row;
					blankPos.col = col;
				}
			}
		}

		setNeighbour();
	}

	void slide(Point newPos)
	{
		grid[blankPos.row][blankPos.col].value = grid[newPos.row][newPos.col].value;
		grid[newPos.row][newPos.col].value = 0;
		blankPos = newPos;

		setNeighbour();
	}

	void updateScore()
	{
		// Calculate scores
		if (parent)
		{
			gScore = parent->gScore + 1;
		}

		calHScore();

		fScore = gScore + hScore;
	}

	void updateParent(State* state)
	{
		parent = state;
	}

	bool isStateSame(State compareState)
	{
		for (int row = 0; row < gridSize; row++)
		{
			for (int col = 0; col < gridSize; col++)
			{
				if (grid[row][col].value != compareState.grid[row][col].value) return false;
			}
		}
		return true;
	}

	bool isTarget()
	{
		return hScore == 0;
	}

	void printGrid(bool showBlankAsZero = true)
	{
		cout << "+-----+" << endl;
		for (int row = 0; row < gridSize; row++)
		{
			for (int col = 0; col < gridSize; col++)
			{
				if (col == 0)
					cout << "|";

				(grid[row][col].value == 0 && !showBlankAsZero) ?
					cout << " " :
					cout << grid[row][col].value;

				if (col == gridSize - 1)
					cout << "|";
				else
					cout << " ";
			}
			cout << endl;
		}
		cout << "+-----+" << endl;
	}

	void debug(bool showExtra)
	{
		for (int row = 0; row < gridSize; row++)
		{
			for (int col = 0; col < gridSize; col++)
			{
				cout << grid[row][col].value;
			}
		}
		cout << "~~~" << endl;
		if (showExtra)
		{
			//printf("parent: index %i\n", parent);
			printf("blankPos: (%i,%i)\n", blankPos.row, blankPos.col);
			printf("neighbours: ");
			for (auto v : neighbours)
			{
				printf("(%i,%i)", v.row, v.col);
			}
			cout << endl;
			printf("gScore: %i\n", gScore);
			printf("hScore: %i\n", hScore);
			printf("fScore: %i\n", fScore);
			cout << "~~~~~~~~~" << endl;
		}
	}

private:
	void setNeighbour()
	{
		neighbours.clear();

		// Check left
		if (blankPos.col - 1 >= 0)
			neighbours.push_back(Point(blankPos.row, blankPos.col - 1));

		// Check right
		if (blankPos.col + 1 < gridSize)
			neighbours.push_back(Point(blankPos.row, blankPos.col + 1));

		// Check up
		if (blankPos.row - 1 >= 0)
			neighbours.push_back(Point(blankPos.row - 1, blankPos.col));

		// Check down
		if (blankPos.row + 1 < gridSize)
			neighbours.push_back(Point(blankPos.row + 1, blankPos.col));
	}

	void calHScore()
	{
		hScore = 0;
		int distance = 0;

		for (int row = 0; row < gridSize; row++)
		{
			for (int col = 0; col < gridSize; col++)
			{
				// find h score by finding the distance between current position and goal position (exclude blank)
				Point tileGoalPos = tileGoalPos.indexToPoint(grid[row][col].value);
				if (grid[row][col].value != 0)
					distance += findDistance(Point(row, col), tileGoalPos);
			}
		}

		hScore = distance;
	}

	int findDistance(Point currentPos, Point goalPos)
	{
		// Manhattan distance
		return abs(currentPos.row - goalPos.row) + abs(currentPos.col - goalPos.col);
	}
};

std::vector<State> output;

void getValues()
{
	// Get inputs
	string puzzleInput;
	cout << "Enter input puzzle: ";
	getline(cin, puzzleInput);

	cout << "Enter goal state (leave empty for default '012345678'): ";
	getline(cin, goalValue);

	if (goalValue.empty())
		goalValue = "012345678";

	// Set the puzzle grid and goal grid
	for (int row = 0; row < gridSize; row++)
	{
		for (int col = 0; col < gridSize; col++)
		{
			goalState[row][col] = goalValue[row * gridSize + col] - '0'; // Convert char to int

			puzzle[row][col].value = puzzleInput[row * gridSize + col] - '0'; // Convert char to int
			puzzle[row][col].goalValue = goalState[row][col];
		}
	}
}

void reconstructPath(State* current)
{
	// Recursive function which will trace back to start state
	if (current->parent)
	{
		output.push_back(*current);
		reconstructPath(current->parent);
	}
}

void printOutput()
{
	// Get the console window's handle and set the cursor position
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD destCoord = { 0,0 };
	system("CLS");

	// Revearse the output path
	int step = 0;
	for (auto it = output.rbegin(); it != output.rend(); it++)
	{
		SetConsoleCursorPosition(hStdout, destCoord);	
		printf("Step %i:\n\n", step);
		it->printGrid(false);

		Sleep(config::consoleOutputSpeed);
		step++;
	}

	system("CLS");
	printf("Full Output:\n\n");
	for (auto it = output.rbegin(); it != output.rend(); it++)
	{
		it->printGrid(false);
		printf("\n��\n\n");
	}
	printf("Finish!\n");
}

// A* search
void search()
{
	State currentState;
	State nextState;
	list<State> openSet;
	list<State> closeSet;
	list<State>::iterator iter;

	// Use the input as starting state
	State startState;
	startState.setGrid(puzzle);
	startState.updateScore();
	openSet.push_back(startState);

	while (!openSet.empty())
	{
		// Get the state with lowest f score
		currentState = openSet.front();
		list<State>::iterator lowest = openSet.begin(); // Store a iterator of the lowest f score state
		for (iter = openSet.begin(); iter != openSet.end(); ++iter)
		{
			if (iter->fScore < currentState.fScore)
			{
				currentState = *iter;
				lowest = iter;
			}
		}

		// Add that state to close set and remove it from open set
		closeSet.push_back(currentState);
		openSet.erase(lowest);

		// If current state is target state
		if (currentState.isTarget())
			break;

		// Cheak all neighbours
		for (auto& neighbour : currentState.neighbours)
		{
			// Simulate the next move
			nextState.setGrid(currentState.grid);
			nextState.slide(neighbour);

			// Check if the neighbour is in close set already
			bool isInCloseSet = false;
			for (auto& state : closeSet)
			{
				if (nextState.isStateSame(state))
				{
					isInCloseSet = true;
					break;
				}
			}

			if (!isInCloseSet)
			{
				// Check if the neighbour is in open set already
				State* stateRef = NULL;
				for (auto& state : openSet)
				{
					if (nextState.isStateSame(state))
					{
						stateRef = &state;
						break;
					}
				}

				if (nextState.hScore < currentState.hScore || !stateRef)
				{
					iter = --closeSet.end(); // Get the last element in the close set
					nextState.updateParent(&*iter);
					nextState.updateScore();

					if (!stateRef)
						openSet.push_back(nextState);
					else
						stateRef->updateScore();
				}
			}
		}
	}

	reconstructPath(&closeSet.back());
	output.push_back(startState); // Add the start state to output so it will show when printing
}

int main(int argc, char** argv)
{
	int retry;

	do
	{
		system("CLS");
		getValues();
		search();
		printOutput();

		printf("\n*****\n");
		printf("Retry? (1 to continue, -1 to quit): ");
		cin >> retry;
		cin.ignore(); // As the new line will count as input
	} while (retry != -1);

	return 0;
}