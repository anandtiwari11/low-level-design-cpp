#include <bits/stdc++.h>
using namespace std;

class User {
private:
    string name;
    int uid;
    string email;
    string phone;
    string notificationMethod;
    map<int, pair<int, string>> feedbacks; 

public:
    User(string n, int id, string mail, string p, string method)
        : name(n), uid(id), email(mail), phone(p), notificationMethod(method) {}

    string getName() { 
        return name;
    }
    string getNotificationMethod() {
        return notificationMethod; 
    }
    
    void updateFeedback(int productId, int rating, string message) {
        feedbacks[productId] = {rating, message};
    }

    void getFeedback(int productId) {
        if (feedbacks.find(productId) != feedbacks.end()) {
            cout << "[Feedback] " << name << " rated " << feedbacks[productId].first 
                 << " with message: " << feedbacks[productId].second << endl;
        }
        else {
            cout << "[Feedback] No feedback for product ID: " << productId << endl;
        }
    }
};

class NotificationChannel {
public:
    virtual void sendNotification() = 0;
    virtual ~NotificationChannel() {} 
};

class Whatsapp : public NotificationChannel {
private:
    User* user;
    string message;

public:
    Whatsapp(string msg, User* u) : message(msg), user(u) {}
    void sendNotification() override {
        cout << "[WhatsApp] Sent to " << user->getName() << ": " << message << endl;
    }
};

class Email : public NotificationChannel {
private:
    User* user;
    string message;

public:
    Email(string msg, User* u) : message(msg), user(u) {}
    void sendNotification() override {
        cout << "[Email] Sent to " << user->getName() << ": " << message << endl;
    }
};

class NotificationFactory {
public:
    static unique_ptr<NotificationChannel> createNotification(string type, string message, User* user) {
        if (type == "Whatsapp") {
            return make_unique<Whatsapp>(message, user);
        } else if (type == "Email") {
            return make_unique<Email>(message, user);
        } else {
            return nullptr;
        }
    }
};

class FeedbackObserver {
public:
    virtual void onFeedbackReceived(int productId, int rating, User* user, string comments) = 0;
    virtual ~FeedbackObserver() {}
};

class CRMService : public FeedbackObserver {
public:
    void onFeedbackReceived(int productId, int rating, User* user, string comments) override {
        user->updateFeedback(productId, rating, comments);
        if (rating <= 7) {
            cout << "[CRM] Issue detected! Sending survey to user..." << endl;
            auto notification = NotificationFactory::createNotification(user->getNotificationMethod(), 
            "We're sorry! Please share your issue.", user);
            notification->sendNotification();
        }
    }
};

class FeedbackService {
private:
    vector<shared_ptr<FeedbackObserver>> observers;

public:
    void registerObserver(shared_ptr<FeedbackObserver> observer) {
        observers.push_back(observer);
    }

    void requestFeedback(User* user, int productId) {
        auto notification = NotificationFactory::createNotification(user->getNotificationMethod(),
        "Please rate your order (1-10):", user);
        notification->sendNotification();
    }

    void storeFeedback(int productId, int rating, User* user, string comments) {
        user->updateFeedback(productId, rating, comments);
        for (auto& obs : observers) {
            obs->onFeedbackReceived(productId, rating, user, comments);
        }
    }
};

class OrderService {
private:
    shared_ptr<FeedbackService> feedbackService;

public:
    OrderService(shared_ptr<FeedbackService> service) : feedbackService(service) {}
    void deliverOrder(User* user, int productId) {
        cout << "[Order] Order delivered to " << user->getName() << " (Product ID: " << productId << ")" << endl;
        feedbackService->requestFeedback(user, productId);
    }
};

int main() {
    User user("Anand Tiwari", 101, "anand@example.com", "+91-9260935205", "Whatsapp");
    auto feedbackService = make_shared<FeedbackService>();
    auto crmService = make_shared<CRMService>();
    feedbackService->registerObserver(crmService);
    auto orderService = make_shared<OrderService>(feedbackService);
    int productId = 123;
    orderService->deliverOrder(&user, productId);
    feedbackService->storeFeedback(productId, 5, &user, "Late delivery.");
    user.getFeedback(productId);
    return 0;
}
