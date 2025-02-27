#include<bits/stdc++.h>
using namespace std;

class User {
    private:
        int id;
        string name;
        vector<BookItem*> borrowed;
    public:
        User(int id, string name) : id(id), name(name) {}
        void borrow(BookItem* bookItem) {
            borrowed.push_back(bookItem);
        }
        int returnBook(BookItem* bookItem, FineCalculator* fineCalculator) {
            time_t now = time(0);
            int fine = fineCalculator->fineCalculator(bookItem->getDueDate(), now);
            borrowed.erase(remove(borrowed.begin(), borrowed.end(), bookItem), borrowed.end());
            delete bookItem;
            return fine;
        }
        string getName() {
            return name;
        }
};


class Book {
    private:
        int bookId;
        string bookName;
        string bookAuthor;
        int borrowDuration;
    public:
        Book(int id, string name, string author, int duration) : bookId(id), bookName(name), bookAuthor(author), borrowDuration(duration) {};
        int getId() {
            return bookId;
        }
        string getName() {
            return bookName;
        }
        string getAuthor() {
            return bookAuthor;
        }
        int getBorrowDuration() {
            return borrowDuration;
        }
};

class BookItem {
    private:
        Book* book;
        string issuedTo;
        time_t issuedDate;
        time_t dueDate;
    public:
        BookItem(Book* book, string user) : book(book), issuedTo(user) {
            time_t now = time(0);
            issuedDate = now;
            dueDate = now + (book->getBorrowDuration() * 24 * 60 * 60);
        }
        bool isOverDue() {
            time_t now = time(0);
            return now > dueDate;
        }
        time_t getDueDate() const {
            return dueDate;
        }
        string getIssuedTo() const {
            return issuedTo; 
        }
};

class FineCalculator {
    public:
        virtual int fineCalculator(time_t dueDate, time_t returnDate) = 0;
        virtual ~FineCalculator() {} 
};

class PerDayFineCalculator : public FineCalculator {
    public:
            int fineCalculator(time_t dueDate, time_t returnDate) override {
            if (returnDate <= dueDate) return 0;
            int daysLate = (returnDate - dueDate) / 24 * 60 * 60;
            return daysLate * 10;
        }
};

class Library {
    private:
        vector<Book*> books;
        vector<User*> users;
        map<int, BookItem*> borrowedBooks;
    public:
        void addBook(Book* book) {
            books.push_back(book);
        }
        void createUser(User* user) {
            users.push_back(user);
        }
        void borrowBook(User* user, Book* book) {
            BookItem* bookItem = new BookItem(book, user->getName());
            borrowedBooks[book->getId()] = bookItem;
            user->borrow(bookItem);
        }
        void returnBook(User* user, Book* book, FineCalculator* fineCalc) {
            if (borrowedBooks.find(book->getId()) == borrowedBooks.end()) return;
            BookItem* bookItem = borrowedBooks[book->getId()];
            int fine = user->returnBook(bookItem, fineCalc);
            borrowedBooks.erase(book->getId());
            std::cout << "Fine for late return: " << fine << " units" << std::endl;
        }
};

int main() {
    Library lib;
    FineCalculator* fineCalc = new PerDayFineCalculator();
    User* user1 = new User(1, "Alice");
    lib.createUser(user1);
    Book* book1 = new Book(101, "C++ Primer", "Stanley Lippman", 7);
    lib.addBook(book1);
    lib.borrowBook(user1, book1);
    lib.returnBook(user1, book1, fineCalc);
    delete fineCalc;
    return 0;
}