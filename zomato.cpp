#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

enum class OrderStatus { PLACED, PREPARING, OUT_FOR_DELIVERY, DELIVERED };

class Order {
public:
    static int orderCounter;
    int orderId;
    std::string user;
    std::string restaurant;
    std::vector<std::string> items;
    OrderStatus status;

    Order(std::string u, std::string r, std::vector<std::string> i)
        : user(u), restaurant(r), items(i), status(OrderStatus::PLACED) {
        orderId = ++orderCounter;
    }

    void updateStatus(OrderStatus newStatus) {
        status = newStatus;
        std::cout << "Order " << orderId << " status updated to: " << getStatusString() << std::endl;
    }

    std::string getStatusString() {
        switch (status) {
            case OrderStatus::PLACED: return "PLACED";
            case OrderStatus::PREPARING: return "PREPARING";
            case OrderStatus::OUT_FOR_DELIVERY: return "OUT_FOR_DELIVERY";
            case OrderStatus::DELIVERED: return "DELIVERED";
        }
        return "UNKNOWN";
    }
};
int Order::orderCounter = 0;

class Restaurant {
public:
    std::string name;
    std::vector<std::string> menu;
    std::vector<Order> orders;

    Restaurant(std::string n, std::vector<std::string> m) : name(n), menu(m) {}

    void acceptOrder(Order& order) {
        std::cout << "Restaurant " << name << " is preparing Order " << order.orderId << std::endl;
        order.updateStatus(OrderStatus::PREPARING);
        orders.push_back(order);
    }
};

class DeliveryPartner {
public:
    std::string name;

    DeliveryPartner(std::string n) : name(n) {}

    void deliverOrder(Order& order) {
        std::cout << "Delivery Partner " << name << " picked up Order " << order.orderId << std::endl;
        order.updateStatus(OrderStatus::OUT_FOR_DELIVERY);
        order.updateStatus(OrderStatus::DELIVERED);
    }
};

class User {
public:
    std::string name;

    User(std::string n) : name(n) {}

    Order placeOrder(Restaurant& restaurant, std::vector<std::string> items) {
        std::cout << name << " placed an order at " << restaurant.name << std::endl;
        Order order(name, restaurant.name, items);
        restaurant.acceptOrder(order);
        return order;
    }
};

class ZomatoSystem {
private:
    static ZomatoSystem* instance;
    std::unordered_map<std::string, Restaurant> restaurants;

    ZomatoSystem() {}  // Private constructor for Singleton

public:
    static ZomatoSystem* getInstance() {
        if (!instance) instance = new ZomatoSystem();
        return instance;
    }

    void addRestaurant(std::string name, std::vector<std::string> menu) {
        restaurants[name] = Restaurant(name, menu);
    }

    Restaurant* findRestaurant(std::string name) {
        if (restaurants.find(name) != restaurants.end()) return &restaurants[name];
        return nullptr;
    }
};
ZomatoSystem* ZomatoSystem::instance = nullptr;

int main() {
    ZomatoSystem* zomato = ZomatoSystem::getInstance();

    zomato->addRestaurant("Domino's", {"Pizza", "Pasta", "Burger"});
    zomato->addRestaurant("McDonald's", {"Fries", "Burger", "Coke"});

    User user("Anand");
    Restaurant* restaurant = zomato->findRestaurant("Domino's");

    if (restaurant) {
        Order order = user.placeOrder(*restaurant, {"Pizza", "Burger"});
        DeliveryPartner deliveryBoy("Ravi");
        deliveryBoy.deliverOrder(order);
    }

    return 0;
}
