#include<bits/stdc++.h>
#include<random>
using namespace std;

class Player {
    private:
        string name;
        int id;
    public:
        Player(string n, int i) : id(i), name(n) {};
        int getId() {
            return id;
        }
        string getName() {
            return name;
        }
};

class Dice {
    private:
        int numberOfDice;
    public:
        Dice(int n) : numberOfDice(n) {};
        int rollDice() {
            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> distrib(numberOfDice, numberOfDice * 6);
            return distrib(gen);
        }
};

class Jump {
    private:
        int startPoint;
        int endPoint;
    public:
        Jump(int s, int e) : startPoint(s), endPoint(e) {};
        int getStartPoint() {
            return startPoint;
        }
        int getEndpoint() {
            return endPoint;
        }
};

class Board {
    private:
        Dice dice;
        queue<Player> nextTurn;
        vector<Jump> snakes;
        vector<Jump> ladders;
        map<int, int> playerPosition;
    public:
        Board(Dice d, queue<Player> n, vector<Jump> s, vector<Jump> l, map<int, int> mp) :
        dice(d), nextTurn(n), snakes(s), ladders(l), playerPosition(mp) {};
        void startGame() {
            while(nextTurn.size() > 1) {
                auto p = nextTurn.front();
                int currentPos = playerPosition[p.getId()];
                nextTurn.pop();
                int nextPos = currentPos + dice.rollDice();
                if (nextPos < 100)
                playerPosition[p.getId()] = nextPos;
                else if (nextPos == 100) {
                    cout << "Player with player id " << p.getId() << "Name " << p.getName() << "won" << endl;
                    continue;
                }
                for (auto &ladder : ladders) {
                    if (ladder.getStartPoint() == playerPosition[p.getId()]) {
                        playerPosition[p.getId()] = ladder.getEndpoint();
                    }
                }
                for (auto &ladder : snakes) {
                    if (ladder.getStartPoint() == playerPosition[p.getId()]) {
                        playerPosition[p.getId()] = ladder.getEndpoint();
                    }
                }
                nextTurn.push(p);
            } 
        }
};