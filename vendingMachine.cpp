#include <bits/stdc++.h>
using namespace std;

class IVendingMachine
{
public:
    virtual void enterCash(int amount) = 0;
    virtual void chooseProduct(int productId) = 0;
    virtual void refund() = 0;
    virtual ~IVendingMachine() = default;
};

class IInventory
{
public:
    virtual void addProduct(int id, string name, int price, int quantity) = 0;
    virtual bool isAvailable(int id) = 0;
    virtual Product getProduct(int id) = 0;
    virtual void dispenseProduct(int id) = 0;
    virtual ~IInventory() = default;
};

class Product
{
public:
    int id;
    string name;
    int price;

    Product() : id(0), name(""), price(0) {}
    Product(int id, string name, int price) : id(id), name(name), price(price) {}
};

class Inventory : public IInventory
{
private:
    map<int, pair<Product, int>> stock;
public:
    void addProduct(int id, string name, int price, int quantity) override
    {
        stock[id] = {Product(id, name, price), quantity};
    }

    bool isAvailable(int id) override
    {
        return stock.count(id) && stock[id].second > 0;
    }

    Product getProduct(int id) override
    {
        return stock.count(id) ? stock[id].first : Product();
    }

    void dispenseProduct(int id) override
    {
        if (stock.count(id) && stock[id].second > 0)
        {
            stock[id].second--;
        }
    }
};

class VendingMachine : public IVendingMachine
{
private:
    enum State
    {
        IDLE,
        ACTIVE,
        DISPENSING
    };
    State state;
    int currentBalance;
    IInventory *inventory;

public:
    VendingMachine(IInventory *inv) : state(IDLE), currentBalance(0), inventory(inv) {}

    void enterCash(int amount) override
    {
        if (state == IDLE)
        {
            state = ACTIVE;
            currentBalance += amount;
            cout << "Cash entered: " << currentBalance << endl;
        }
        else
        {
            cout << "Invalid action.\n";
        }
    }

    void chooseProduct(int productId) override
    {
        if (state == ACTIVE)
        {
            if (!inventory->isAvailable(productId))
            {
                cout << "Product out of stock. Refunding: " << currentBalance << endl;
                refund();
                return;
            }
            Product p = inventory->getProduct(productId);
            if (currentBalance < p.price)
            {
                cout << "Insufficient funds. Refunding: " << currentBalance << endl;
                refund();
                return;
            }
            state = DISPENSING;
            dispenseProduct(productId);
        }
        else
        {
            cout << "Invalid action.\n";
        }
    }

    void dispenseProduct(int productId)
    {
        Product p = inventory->getProduct(productId);
        if (p.id == 0)
            return;

        inventory->dispenseProduct(productId);
        currentBalance -= p.price;
        cout << "Dispensing: " << p.name << endl;
        state = IDLE;
    }

    void refund() override
    {
        cout << "Refunding: " << currentBalance << endl;
        currentBalance = 0;
        state = IDLE;
    }
};

int main()
{
    Inventory inv;
    inv.addProduct(1, "Soda", 50, 5);
    inv.addProduct(2, "Chips", 30, 5);

    VendingMachine vm(&inv);

    vm.enterCash(40);
    vm.chooseProduct(1);
    vm.enterCash(50);
    vm.chooseProduct(1);

    return 0;
}