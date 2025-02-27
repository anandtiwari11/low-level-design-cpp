#include <bits/stdc++.h>
#include <mutex>
using namespace std;

class Car
{
private:
    string maker, model, year;
    int rentalPricePerDay;

public:
    Car(string c, string m, string y, int rent) : maker(c), model(m), year(y), rentalPricePerDay(rent) {}
    int getPrice() const { return rentalPricePerDay; }
};

class User
{
private:
    string name, contactNumber, drivingLicenseNumber;

public:
    User(string n, string number, string license) : name(n), contactNumber(number), drivingLicenseNumber(license) {}
};

class IPaymentStrategy
{
public:
    virtual void makePayment(int amount) = 0;
    virtual ~IPaymentStrategy() = default;
};

class UPIPayment : public IPaymentStrategy
{
public:
    void makePayment(int amount) override { cout << "Payment of " << amount << " done via UPI.\n"; }
};

class CashPayment : public IPaymentStrategy
{
public:
    void makePayment(int amount) override { cout << "Payment of " << amount << " done via Cash.\n"; }
};

class PaymentFactory
{
public:
    static shared_ptr<IPaymentStrategy> createPaymentMethod(const string &method)
    {
        if (method == "UPI")
            return make_shared<UPIPayment>();
        else if (method == "Cash")
            return make_shared<CashPayment>();
        else
            throw invalid_argument("Invalid payment method.");
    }
};

class Reservation
{
private:
    int uid;
    shared_ptr<User> user;
    shared_ptr<Car> car;
    string start, end;
public:
    Reservation(int id, shared_ptr<User> u, shared_ptr<Car> c, string st, string e)
        : uid(id), user(u), car(c), start(st), end(e) {}
    shared_ptr<Car> getCar() const { return car; }
};

class IRentalSystemInterface
{
public:
    virtual void addCar(shared_ptr<Car> car) = 0;
    virtual void removeCar(shared_ptr<Car> car) = 0;
    virtual bool makeReservation(shared_ptr<Reservation> r) = 0;
    virtual vector<shared_ptr<Car>> searchCarsByPrice(int start, int end) = 0;
    virtual vector<shared_ptr<Car>> getAllCars() = 0;
    virtual vector<shared_ptr<Car>> getAvailableCars() = 0;
    virtual ~IRentalSystemInterface() = default;
};

class CarAvailabilityService
{
private:
    map<shared_ptr<Car>, bool> carAvailability;
public:
    void addCar(shared_ptr<Car> car) { carAvailability[car] = true; }
    void removeCar(shared_ptr<Car> car) { carAvailability.erase(car); }
    bool isAvailable(shared_ptr<Car> car)
    {
        return carAvailability.count(car) && carAvailability[car];
    }
    bool bookCar(shared_ptr<Car> car)
    {
        if (isAvailable(car))
        {
            carAvailability[car] = false;
            return true;
        }
        return false;
    }
    vector<shared_ptr<Car>> getAvailableCars()
    {
        vector<shared_ptr<Car>> available;
        for (const auto &pair : carAvailability)
            if (pair.second)
                available.push_back(pair.first);
        return available;
    }
};

class RentalSystem : public IRentalSystemInterface
{
private:
    static shared_ptr<RentalSystem> rentalInstance;
    static once_flag initFlag;
    CarAvailabilityService availabilityService;
    RentalSystem() {}
public:
    static shared_ptr<RentalSystem> getInstance()
    {
        call_once(initFlag, []() 
        { 
            rentalInstance = make_shared<RentalSystem>(); 
        });
        return rentalInstance;
    }
    void addCar(shared_ptr<Car> car) override { availabilityService.addCar(car); }
    void removeCar(shared_ptr<Car> car) override { availabilityService.removeCar(car); }
    bool makeReservation(shared_ptr<Reservation> r) override
    {
        if (availabilityService.bookCar(r->getCar()))
            return true;
        cout << "Car is already booked.\n";
        return false;
    }
    vector<shared_ptr<Car>> searchCarsByPrice(int start, int end) override
    {
        vector<shared_ptr<Car>> res;
        for (auto &car : getAllCars())
            if (car->getPrice() >= start && car->getPrice() <= end)
                res.push_back(car);
        return res;
    }
    vector<shared_ptr<Car>> getAllCars() override { return availabilityService.getAvailableCars(); }
    vector<shared_ptr<Car>> getAvailableCars() override { return availabilityService.getAvailableCars(); }
};

shared_ptr<RentalSystem> RentalSystem::rentalInstance = nullptr;
once_flag RentalSystem::initFlag;

class CarRentalSystem
{
private:
    shared_ptr<IRentalSystemInterface> rental;
    int reservationCounter;
public:
    CarRentalSystem(shared_ptr<IRentalSystemInterface> rentalSystem) : rental(rentalSystem), reservationCounter(0) {}
    shared_ptr<Reservation> reserveCar(shared_ptr<User> user, shared_ptr<Car> car, shared_ptr<IPaymentStrategy> paymentStrategy, const string &start, const string &end)
    {
        int reservationId = ++reservationCounter;
        auto reservation = make_shared<Reservation>(reservationId, user, car, start, end);

        if (rental->makeReservation(reservation))
        {
            paymentStrategy->makePayment(car->getPrice());
            cout << "Car reserved successfully with Reservation ID: " << reservationId << endl;
            return reservation;
        }
        else
        {
            cout << "Reservation failed. Car is not available." << endl;
            return nullptr;
        }
    }

    vector<shared_ptr<Car>> searchCarsByPriceRange(int low, int high)
    {
        return rental->searchCarsByPrice(low, high);
    }
    vector<shared_ptr<Car>> getAllAvailableCars()
    {
        return rental->getAvailableCars();
    }
};

int main()
{
    auto rentalSystem = RentalSystem::getInstance();
    CarRentalSystem system(rentalSystem);
    auto car1 = make_shared<Car>("Toyota", "Corolla", "2022", 50);
    auto car2 = make_shared<Car>("Honda", "Civic", "2021", 55);
    rentalSystem->addCar(car1);
    rentalSystem->addCar(car2);
    auto user1 = make_shared<User>("Alice", "1234567890", "DL12345");
    auto paymentMethod = PaymentFactory::createPaymentMethod("UPI");
    system.reserveCar(user1, car1, paymentMethod, "2025-02-28", "2025-03-05");
    auto availableCars = system.getAllAvailableCars();
    cout << "Available cars: " << availableCars.size() << endl;
    return 0;
}