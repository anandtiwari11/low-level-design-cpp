#include <bits/stdc++.h>
using namespace std;

class User {
private:
    string name;
    string accountNumber;
    int money;
    int pin;
public:
    User(string name, string ac, int pass, int m) : name(name), accountNumber(ac), pin(pass), money(m) {}
    void addMoney(int amount) {
        money += amount;
    }
    bool withdrawMoney(int amount) {
        if (money >= amount) {
            money -= amount;
            return true;
        }
        return false;
    }
    int getBalance() {
        return money;
    }
    string getAcc() {
        return accountNumber;
    }
    int getPin() {
        return pin;
    }
    string getName() {
        return name;
    }
};

class ICash {
public:
    virtual void addMoney(int val, int cnt) = 0;
    virtual bool canProcess(int amount) = 0;
    virtual void performTransaction() = 0;
};

class Cash : public ICash {
private:
    map<int, int> money; 
    map<int, int> transaction;
    static Cash* instance;
    Cash() {};
public: 
    static Cash* getInstance() {
        if (!instance) {
            instance = new Cash();
        }
        return instance;
    }
    void addMoney(int val, int cnt) override {
        money[val] += cnt;
    }
    bool canProcess(int amount) override {
        int sum = amount;
        transaction.clear();
        for (auto &x : money) {
            int denomination = x.first;
            int count = x.second;
            while (sum >= denomination && count) {
                count--;
                sum -= denomination;
                transaction[denomination]++;
            }
        }
        return sum == 0; 
    }
    void performTransaction() override {
        for (auto &x : transaction) {
            money[x.first] -= x.second;
        }
        transaction.clear();
    }
    int getTotalCash() {
        int total = 0;
        for (auto &x : money) {
            total += x.first * x.second;
        }
        return total;
    }
};

class IATMMachine {
public:
    virtual void enterCard(string acc, int pin) = 0;
    virtual void withdrawMoney(int amount) = 0;
    virtual void depositMoney(int amount) = 0;
    virtual void checkBalance() = 0;
};

class ATMMachine : public IATMMachine {
private:
    enum State {
        IDLE,
        ACTIVE
    };
    State state;
    ICash *icash;
    vector<User*> userDetails;
    User *currentUser;
    static ATMMachine *instance;

    ATMMachine(ICash *c) : state(IDLE), icash(c), currentUser(nullptr) {}

public:
    static ATMMachine* getInstance(ICash *c) {
        if (!instance) {
            instance = new ATMMachine(c);
        }
        return instance;
    }
    void addUser(string name, string acc, int pin, int balance) {
        userDetails.push_back(new User(name, acc, pin, balance));
    }
    void enterCard(string acc, int pin) override {
        for (auto &x : userDetails) {
            if (x->getAcc() == acc) {
                if (x->getPin() == pin) {
                    state = ACTIVE;
                    currentUser = x;
                    cout << "Welcome " << acc << "! You are now logged in.\n";
                    return;
                } else {
                    cout << "Wrong PIN! Try again.\n";
                    return;
                }
            }
        }
        cout << "Account not found!\n";
    }
    void withdrawMoney(int amount) override {
        if (state != ACTIVE || currentUser == nullptr) {
            cout << "Please authenticate first.\n";
            return;
        }
        if (!icash->canProcess(amount)) {
            cout << "ATM does not have enough cash.\n";
            return;
        }
        if (!currentUser->withdrawMoney(amount)) {
            cout << "Insufficient balance in your account.\n";
            return;
        }
        icash->performTransaction();
        cout << "Withdrawn: $" << amount << "\n";
    }
    void depositMoney(int amount) override {
        if (state != ACTIVE || currentUser == nullptr) {
            cout << "Please authenticate first.\n";
            return;
        }
        currentUser->addMoney(amount);
        icash->addMoney(500, amount / 500); 
        cout << "Deposited: $" << amount << "\n";
    }
    void checkBalance() override {
        if (state != ACTIVE || currentUser == nullptr) {
            cout << "Please authenticate first.\n";
            return;
        }
        cout << "Your balance: $" << currentUser->getBalance() << "\n";
    }
    void logout() {
        if (state == ACTIVE) {
            cout << "Goodbye " << currentUser->getName() << "!\n";
            currentUser = nullptr;
            state = IDLE;
        } else {
            cout << "No user logged in.\n";
        }
    }
};

ATMMachine* ATMMachine::instance = nullptr;
Cash* Cash::instance = nullptr;

int main() {
    Cash* atmCash = Cash::getInstance();
    atmCash->addMoney(500, 10); 
    atmCash->addMoney(200, 10); 
    atmCash->addMoney(100, 20); 
    ATMMachine* atm = ATMMachine::getInstance(atmCash);
    atm->addUser("John Doe", "12345", 6789, 5000);
    atm->addUser("Alice", "54321", 1234, 10000);
    atm->enterCard("12345", 6789);
    atm->checkBalance();
    atm->withdrawMoney(700);
    atm->checkBalance();
    atm->depositMoney(500);
    atm->checkBalance();
    atm->logout();
    return 0;
}
