#include <bits/stdc++.h>
using namespace std;

class Player {
private:
    int id;
    string name;
    int runsScored;
    int ballsFaced;
    int ballsThrown;
    int wicketsTaken;
    int runsGiven;

public:
    Player(int i, string s) : id(i), name(s), runsScored(0), ballsFaced(0), ballsThrown(0), wicketsTaken(0), runsGiven(0) {}

    int getRunsScored() const { 
        return runsScored; 
    }
    int getWicketsTaken() const {
         return wicketsTaken; 
    }
    int getRunsGiven() const {
         return runsGiven; 
    }
    string getName() const {
         return name;
    }
    void addRuns(int runs) {
        runsScored += runs;
        ballsFaced++;
    }
    void giveRuns(int runs) {
        runsGiven += runs;
        ballsThrown++;
    }
    void takeWicket() {
        wicketsTaken++;
    }
};

class Team {
private:
    int id;
    string name;
    vector<Player*> players;

public:
    Team(int i, string n) : id(i), name(n) {}
    void addPlayer(Player* p) {
        if (players.size() == 11) {
            cerr << "Error: Team is already full!" << endl;
            return;
        }
        players.push_back(p);
    }
};

class ScoringStrategy {
public:
    virtual void updateScore(int &totalRuns, int &totalWickets) = 0;
    virtual ~ScoringStrategy() = default;
};

class RunsScoring : public ScoringStrategy {
private:
    int runs;

public:
    RunsScoring(int r) : runs(r) {}

    void updateScore(int &totalRuns, int &totalWickets) override {
        totalRuns += runs;
    }
};

class WicketScoring : public ScoringStrategy {
public:
    void updateScore(int &totalRuns, int &totalWickets) override {
        totalWickets++;
    }
};

class ScoringFactory {
public:
    static shared_ptr<ScoringStrategy> createScoringEvent(const string &type, int value = 0) {
        if (type == "RUN")
            return make_shared<RunsScoring>(value);
        else if (type == "WICKET")
            return make_shared<WicketScoring>();
        return nullptr;
    }
};

class IObserver {
public:
    virtual void update(int totalRuns, int totalWickets) = 0;
    virtual ~IObserver() = default;
};

class ScoreBoard : public IObserver {
private:
    static ScoreBoard *instance;
    int totalRuns, totalWickets;

    ScoreBoard() : totalRuns(0), totalWickets(0) {}

public:
    static ScoreBoard *getInstance() {
        static ScoreBoard instance;
        return &instance;
    }

    void update(int runs, int wickets) override {
        totalRuns = runs;
        totalWickets = wickets;
        display();
    }

    void display() {
        cout << "Score: " << totalRuns << "/" << totalWickets << endl;
    }
};

ScoreBoard *ScoreBoard::instance = nullptr;

class Match {
private:
    int totalRuns, totalWickets;
    vector<IObserver*> observers;

public:
    Match() : totalRuns(0), totalWickets(0) {}

    void addObserver(IObserver *obs) {
        observers.push_back(obs);
    }

    void notifyObservers() {
        for (auto obs : observers) {
            obs->update(totalRuns, totalWickets);
        }
    }

    void addScoringEvent(shared_ptr<ScoringStrategy> event) {
        if (event) {
            event->updateScore(totalRuns, totalWickets);
            notifyObservers();
        }
    }
};

int main() {
    auto team1 = make_shared<Team>(1, "India");
    team1->addPlayer(new Player(1, "Virat Kohli"));
    team1->addPlayer(new Player(2, "Rohit Sharma"));
    Match match;
    ScoreBoard *scoreboard = ScoreBoard::getInstance();
    match.addObserver(scoreboard);
    match.addScoringEvent(ScoringFactory::createScoringEvent("RUN", 4));
    match.addScoringEvent(ScoringFactory::createScoringEvent("RUN", 6));
    match.addScoringEvent(ScoringFactory::createScoringEvent("WICKET"));
    return 0;
}