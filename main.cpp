#include <iostream>
#include <string>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <sstream>
#include <fstream>
#include <iterator>
#include <cmath>
#include <thread>
#include <future>
#include <C:\Users\RLS\Documents\GitHub\cpp-nlp\library.hpp>

std::vector<std::vector<int>> world = {
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1
    },
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
}; //2d vector of bools. 1 means there is a wall. 10x10 grid.

std::vector<int> position = {0, 0}; //x, y position of the player (ANN). Note that they correspond to the world index, so the values start at 0, 0). This is the starting position of the player.

//reads from file 'world0.txt' and populates the world vector with the values in the file. It will scan the file line by line and then split each part of the file based on spaces (new line is new row) and convert each value to an int and store it in the world vector.
void initializeWorld() {
    std::ifstream file("world0.txt");
    if (!file) {
        std::cout << "Unable to open file." << std::endl;
        return;
    }

    std::string line;
    std::vector<std::vector<int>> tempWorld;
    std::vector<std::future<std::vector<int>>> futures;

    while (std::getline(file, line)) {
        futures.push_back(std::async(std::launch::async, [line]() {
            std::istringstream iss(line);
            std::vector<int> row;
            int value;
            while (iss >> value) {
                row.push_back(value);
            }
            return row;
        }));
    }
    file.close();

    for (auto& future : futures) {
        tempWorld.push_back(future.get());
    }

    if (!tempWorld.empty()) {
        world = tempWorld;
    }
}

bool detectCollision(std::vector<int> position) {
    return world[position[0]][position[1]] == 1;
}

bool detectWin(std::vector<int> position) {
    return world[position[0]][position[1]] == 2;
}

int main() {
    initializeWorld();
    std::cout << "World initialized" << std::endl;
    std::cout << "World size: " << world.size() << "x" << world[0].size() << std::endl;
    std::cout << "World: " << std::endl;
    for (int i = 0; i < world.size(); i++) {
        for (int j = 0; j < world[i].size(); j++) {
            std::cout << world[i][j] << " ";
        }
        std::cout << std::endl;
    }
    int i = 0;
    while (detectCollision(position))
    {
        i++;
        std::cout << "Moving player to new position. Pass: " <<  std::to_string(i) << std::endl;
        if (i < world.size())
        {
            std::cout << "shiftR1" << std::endl;
            position[0]++;
        }
        else
        {
            std::cout << "shiftD1-resetCol" << std::endl;
            i=0;
            position[0] = 0;
            position[1]++;
        }
    }
    std::cout << "Final position: " << position[0] << ", " << position[1] << std::endl;
    return 0;
}