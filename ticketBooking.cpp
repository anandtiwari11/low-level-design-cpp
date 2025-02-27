#include<bits/stdc++.h>
using namespace std;

class User  {
    private:
        int uid;
        string name;
        bool isPremium;
        string seatType;
    public:
        User(int id, string s, bool p) : uid(id), name(s) {};
        int getUserId () {
            return uid;
        }
};

class PremiumUser : public User {
    public:
        PremiumUser(int id, string name) : User(id, name, true) {};
};

class GeneralUser : public User {
    public:
        GeneralUser(int id, string name) : User(id, name, false) {};
};

class UserFactory {
    public:
        static User* createUser(int id, string name, string type) {
            if (type == "Premium") {
                return new PremiumUser(id, name);
            }
            else if (type == "General") {
                return new GeneralUser(id, name);
            }
        } 
};

class Seat {
    private:
        int userId;  
        int seatId;
        IBookingStrategy* bookingStrategy;
        string type;
    public:
        Seat(int id, IBookingStrategy* strategy, string s) : seatId(id), userId(0), bookingStrategy(strategy), type(s) {}

        void bookTicket(int user) {
            bookingStrategy->bookTicket(user, seatId, userId);
        }

        bool isAvailable() {
            return userId == 0;  
        }
        
        string getType() {
            return type;
        }
        ~Seat() {
            delete bookingStrategy;
        }
};

class PremiumSeat : public Seat {
    public:
        PremiumSeat(int id) : Seat(id, new PremiumBookingStrategy(), "Premium") {};
};

class GeneralSeat : public Seat {
    public:
        GeneralSeat(int id) : Seat(id, new GeneralBookingStrategy(), "General") {};
};

class SeatFactory {
    public:
        static Seat* createSeat(int id, string type) {
            if (type == "Premium") {
                return new PremiumSeat(id);
            }
            else if (type == "General") {
                return new GeneralSeat(id);
            }
            else {
                return nullptr;
            }
        } 
};

class IBookingStrategy {
    public:
        virtual void bookTicket(int user, int seatId, int &userId) = 0;
        virtual ~IBookingStrategy() = default;
};

class PremiumBookingStrategy : public IBookingStrategy {
    public:
        void bookTicket(int user, int seatId, int &userId) override {
            if (userId == 0) {
                userId = user;
                cout << "Premium Seat " << seatId << " booked successfully by User " << user << endl;
            }
            else {
                cout << "Premium Seat " << seatId << " is already booked!" << endl;
            }
        }
};

class GeneralBookingStrategy : public IBookingStrategy {
    public:
        void bookTicket(int user, int seatId, int& userId) override {
            if (userId == 0) {  
                userId = user;
                cout << "General Seat " << seatId << " booked successfully by User " << user << endl;
            } else {
                cout << "General Seat " << seatId << " is already booked!" << endl;
            }
        }
};


class Cinema {
    private:
        static Cinema* instance;
        vector<Seat*> seats;
        Cinema(int n) {
            for (int i = 1; i <= n; i++) {
                seats.push_back(SeatFactory::createSeat(i, "Premium"));
                seats.push_back(SeatFactory::createSeat(n + i, "General"));
            }
        };
    public:
        static Cinema* getInstance(int n) {
            if (!instance) {
                instance = new Cinema(n);
            }
            return instance;
        }
        void bookSeat(User* user, string type) {
            bool isAvailabe = false;
            for (auto &seat : seats) {
                if (seat->isAvailable() && seat->getType() == type) {
                    isAvailabe = true;
                    seat->bookTicket(user->getUserId());
                    break;
                }
            }
            if (!isAvailabe) {
                cout << "Seats Not Available" << endl;
            }
        }
};

Cinema* Cinema::instance = nullptr;

int main() {
    int n = 3;
    Cinema* cinema = Cinema::getInstance(n);
    auto user1 = UserFactory::createUser(1, "Anand", "Premium");
    auto user2 = UserFactory::createUser(2, "Alice", "General");
    cinema->bookSeat(user1, "Premium");
    cinema->bookSeat(user2, "General");
    return 0;
}
