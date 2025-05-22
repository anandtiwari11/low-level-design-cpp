#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

class Product
{
    int id;
    std::string name;
    double price;

public:
    Product(int id, const std::string &name, double price) : id(id), name(name), price(price) {}
    int getId() const { return id; }
    std::string getName() const { return name; }
    double getPrice() const { return price; }
};

class CartItem
{
    std::shared_ptr<Product> product;
    int quantity;

public:
    CartItem(std::shared_ptr<Product> product, int quantity)
        : product(product), quantity(quantity) {}
    std::shared_ptr<Product> getProduct() const { return product; }
    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }
    double getTotalPrice() const { return product->getPrice() * quantity; }
};

class ICart
{
public:
    virtual void addProduct(std::shared_ptr<Product> product, int quantity) = 0;
    virtual void removeProduct(int productId) = 0;
    virtual double getTotal() const = 0;
    virtual void showCart() const = 0;
    virtual ~ICart() = default;
};

class ShoppingCart : public ICart
{
    std::unordered_map<int, std::shared_ptr<CartItem>> items;

public:
    void addProduct(std::shared_ptr<Product> product, int quantity) override
    {
        int id = product->getId();
        if (items.count(id))
        {
            items[id]->setQuantity(items[id]->getQuantity() + quantity);
        }
        else
        {
            items[id] = std::make_shared<CartItem>(product, quantity);
        }
    }
    void removeProduct(int productId) override
    {
        items.erase(productId);
    }
    double getTotal() const override
    {
        double total = 0;
        for (const auto &pair : items)
        {
            total += pair.second->getTotalPrice();
        }
        return total;
    }
    void showCart() const override
    {
        std::cout << "Cart Contents:\n";
        for (const auto &pair : items)
        {
            std::cout << pair.second->getProduct()->getName()
                      << " x" << pair.second->getQuantity()
                      << " = " << pair.second->getTotalPrice() << "\n";
        }
        std::cout << "Total: " << getTotal() << "\n";
    }
};

class IPaymentStrategy
{
public:
    virtual void pay(double amount) = 0;
    virtual ~IPaymentStrategy() = default;
};

class CreditCardPayment : public IPaymentStrategy
{
public:
    void pay(double amount) override
    {
        std::cout << "Paid " << amount << " using Credit Card.\n";
    }
};

class PayPalPayment : public IPaymentStrategy
{
public:
    void pay(double amount) override
    {
        std::cout << "Paid " << amount << " using PayPal.\n";
    }
};

class CartManager
{
    std::shared_ptr<ICart> cart;

public:
    CartManager(std::shared_ptr<ICart> cart) : cart(cart) {}
    void addProduct(std::shared_ptr<Product> product, int quantity)
    {
        cart->addProduct(product, quantity);
    }
    void removeProduct(int productId)
    {
        cart->removeProduct(productId);
    }
    void checkout(std::shared_ptr<IPaymentStrategy> paymentStrategy)
    {
        double total = cart->getTotal();
        paymentStrategy->pay(total);
    }
    void showCart() const
    {
        cart->showCart();
    }
};

int main()
{
    auto cart = std::make_shared<ShoppingCart>();
    CartManager manager(cart);

    auto apple = std::make_shared<Product>(1, "Apple", 0.5);
    auto banana = std::make_shared<Product>(2, "Banana", 0.3);

    manager.addProduct(apple, 4);
    manager.addProduct(banana, 10);
    manager.showCart();

    manager.removeProduct(1);
    manager.showCart();

    auto payment = std::make_shared<CreditCardPayment>();
    manager.checkout(payment);

    return 0;
}
