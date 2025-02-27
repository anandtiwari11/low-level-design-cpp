#include <bits/stdc++.h>
using namespace std;
#include <memory>
#include <mutex>
using namespace std::chrono;

class Vehicle {
    private:
        int uid;
        time_point<steady_clock> entryTime;
        time_point<steady_clock> exitTime;
    public:
        Vehicle(int id) : uid(id) {};
        void park() {
            entryTime = steady_clock::now();
        }
        void vacate() {
            exitTime = steady_clock::now();
        }
        int getParkingDuration() {
            return duration_cast<seconds>(steady_clock::now() - entryTime).count();
        }
        virtual string getType() = 0;
        virtual ~Vehicle() {};
};

class Car : public Vehicle {
    public:
        Car(int id) : Vehicle(id) {}
        string getType() {
            return "Car";
        }
};

class Bike : public Vehicle {
    public: 
        Bike(int id) : Vehicle(id) {};
        string getType() {
            return "Bike";
        }
};

class VehicleFactory {
    public:
        static unique_ptr<Vehicle> createVehicle (int id, string type) {
            if (type == "Car") {
                return make_unique<Car>(id);
            }
            else if (type == "Bike") {
                return make_unique<Bike>(id);
            }
            return nullptr;
        }
};

class ParkingFeeStrategy {
    public: 
        virtual int getParkingFee(int duration) = 0;
        virtual ~ParkingFeeStrategy() {};
};


class CarFeeStrategy : public ParkingFeeStrategy {
public:
    int getParkingFee(int duration) override {
        return duration * 10; 
    }
};


class BikeFeeStrategy : public ParkingFeeStrategy {
public:
    int getParkingFee(int duration) override {
        return duration * 5; 
    }
};

class ParkingSpot {
    private:
        bool isOccupied;
        unique_ptr<Vehicle> V;
        unique_ptr<ParkingFeeStrategy> S;
    public:
        ParkingSpot(unique_ptr<ParkingFeeStrategy> strategy) : isOccupied(false), S(move(strategy)) {};
        bool isAvailable() { 
            return !isOccupied; 
        }
        void park(unique_ptr<Vehicle> v) {
            if (!isAvailable()) throw runtime_error("Spot Already Occupied");
            isOccupied = true;
            v->park();
            V = move(v);
        }
        int vacate() {
            if (isAvailable()) throw runtime_error("Spot is Empty");
            isOccupied = false;
            int duration = V->getParkingDuration();
            int fee = S->getParkingFee(duration);
            V.reset();
            return fee;
        }
};

class ParkingLot {
    private:
        vector<unique_ptr<ParkingSpot>> Cars;
        vector<unique_ptr<ParkingSpot>> Bikes;
        static mutex mtx;
        static ParkingLot* instance;
        ParkingLot(int cars, int bikes) {
            for (int i = 0; i < cars; i++) {
                Cars.push_back(make_unique<ParkingSpot>(make_unique<CarFeeStrategy>()));
            }
            for (int j = 0; j < bikes; j++) {
                Bikes.push_back(make_unique<ParkingSpot>(make_unique<BikeFeeStrategy>()));
            }
        }
    public:
        static ParkingLot* getInstance(int cars, int bikes) {
            lock_guard<mutex> lock(mtx);
            if (!instance) {
                instance = new ParkingLot(cars, bikes);
            }
            return instance;
        }
        void parkVehicle(unique_ptr<Vehicle> v) {
            vector<unique_ptr<ParkingSpot>>* spots = (v->getType() == "Car") ? &Cars : &Bikes;
            for (auto& spot : *spots) {
                if (spot->isAvailable()) {
                    spot->park(move(v));
                    cout << "Vehicle parked!\n";
                    return;
                }
            }
            cout << "No vacant spots available for " << v->getType() << ".\n";
        }
        void vacateSpot(const string& type) {
            vector<unique_ptr<ParkingSpot>>* spots = (type == "Car") ? &Cars : &Bikes;
            for (auto& spot : *spots) {
                if (!spot->isAvailable()) {
                    int fee = spot->vacate();
                    cout << "Vehicle exited. Parking Fee: ₹" << fee << "\n";
                    return;
                }
            }
            cout << "No " << type << " currently parked.\n";
        }
};

ParkingLot* ParkingLot::instance = nullptr;
mutex ParkingLot::mtx; 

int main() {
    ParkingLot* lot = ParkingLot::getInstance(2, 2); 
    auto car1 = VehicleFactory::createVehicle(1, "Car");
    auto bike1 = VehicleFactory::createVehicle(2, "Bike");
    lot->parkVehicle(move(car1));
    lot->parkVehicle(move(bike1));
    this_thread::sleep_for(chrono::seconds(3));
    lot->vacateSpot("Car");
    lot->vacateSpot("Bike");
    return 0;
}