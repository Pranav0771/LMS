#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

// Functions to make time analysis for fine calculation
string CurrDate(){
    char dt[20]; 
    time_t now = time(0);
    tm *lt = localtime(&now);
    strftime(dt, 20, "%d-%m-%Y %H:%M:%S", lt);
    return string(dt);
}

int dateDiff(const string &date1, const string &date2){
    struct tm t1 = {0};
    struct tm t2 = {0};
    sscanf(date1.c_str(), "%d-%d-%d %d:%d:%d",&t1.tm_mday, &t1.tm_mon, &t1.tm_year,&t1.tm_hour, &t1.tm_min, &t1.tm_sec);
    sscanf(date2.c_str(), "%d-%d-%d %d:%d:%d", &t2.tm_mday, &t2.tm_mon, &t2.tm_year, &t2.tm_hour, &t2.tm_min, &t2.tm_sec);   
    time_t time1 = mktime(&t1);
    time_t time2 = mktime(&t2);
    int diff = difftime(time2, time1)/(60);  // diff in minutes(days)
    return diff;
}

// Class definitions and implementation logic
class Book{
    private:
    string title, auth, pub, yr, ISBN, stat;
   
    public:
    Book(){}
    Book(string title, string auth, string pub, string yr, string ISBN, string stat):
        title(title), auth(auth), pub(pub), yr(yr), ISBN(ISBN), stat(stat){}

    string getTitle(){return title;}
    string getauth(){return auth;}
    string getpub(){return pub;}
    string getyr(){return yr;}
    string getISBN(){return ISBN;}
    string getstat(){return stat;}
    void updtBook(const string &field, const string &newValue){
        if (field == "status"){
            stat = newValue;
        }
        else if(field == "title"){
            title = newValue;
        }
        else if(field == "author"){
            auth = newValue;
        }
        else if(field == "publisher"){
            pub = newValue;
        }
        else if (field == "year"){
            yr = newValue;
        }
        else if (field == "ISBN"){
            ISBN = newValue;
        }
    }
};

class Account{
    private:
    vector<pair<Book*, string>> borrBooks;
    vector<Book*> borrHist;
    pair<int, int> fine;
    string userType;

    protected:
    void escalatingFine(){
        fine.second=0;
        string date = CurrDate();
        for(int i=0; i<borrBooks.size(); i++) {
            int a = dateDiff(borrBooks[i].second, date);
                if(a>15) {
                    fine.second+=(a-15)*10;
                }
        }
    }

    public:
    Account(string userType): fine({0,0}), userType(userType){}
    vector<pair<Book*, string>> getborrBooks(){return borrBooks;}
    vector<Book*>& getborrHist(){return borrHist;} 
    int prevUnpaidFine(){return fine.first;}
    int getFine(){ 
        escalatingFine();
        return fine.first+fine.second;}
    string getUserType(){return userType;}
    void addBorrBook(Book* book, const string &date){
        borrBooks.push_back({book, date});
    }
    void rmBorrBook(Book* book){
        for(int i=0; i<borrBooks.size(); i++){
            if(borrBooks[i].first == book){
                borrBooks.erase(borrBooks.begin()+i);
                book->updtBook("status", "Available");
                borrHist.push_back(book);
                break;
            }
        }
    }
    int periodBorr(Book* book){
        for(int i=0; i<borrBooks.size(); i++){
            if(borrBooks[i].first == book){
                return dateDiff(borrBooks[i].second, CurrDate());
            }
        }
        return -1;
    }
    int borrBooksCount(){return borrBooks.size();}
    void addFine(int amnt){fine.first+=amnt;}
    void payFine(){
        cout << "Fine Paid: " << getFine() << " rupees" << endl;
        fine.first= 0;
        for(int i=0; i<borrBooks.size(); i++){
            char dt[20]; 
            time_t now = time(0);
            now -= 15*60; //1 minute = 1 day
            tm *lt = localtime(&now);
            strftime(dt, 20, "%d-%m-%Y %H:%M:%S", lt);
            borrBooks[i].second = string(dt);
        }
    }
};

class User{
    private:
    int id;
    string name;
    Account acc;

    public:
    User(int id, string name, string usertype): id(id), name(name), acc(usertype){}
    int getId(){return id;}
    string getName(){return name;}
    Account& getAcc(){return acc;}
    void updtName(const string &newName){name = newName;}
    void updtId(int newId){id = newId;}
    virtual void borrowBook(Book* book) = 0;
    virtual void returnBook(Book* book) = 0;
    virtual void addBook(const string &title, const string &auth, const string &pub, const string &yr, const string &ISBN) = 0;
    virtual void rmBook(const string &ISBN) = 0;
    virtual void updtBook(const string &ISBN, const string &field, const string &newValue) = 0;
    virtual void addUser(User* user) = 0;
    virtual void rmUser(User* user) = 0;
    virtual User* identifyUser(int id) = 0;
};

class Student: public User{
    public:
    Student(int id, string name) : User(id, name, "Student") {}
    void borrowBook(Book* book) {
        if(getAcc().borrBooksCount()>=3){
            cout << "You have reached the maximum borrowing limit" << endl;
            return;
        }
        if(book->getstat() == "Borrowed"){
            cout << "Book is already borrowed" << endl;
            return;
        }
        if(book->getstat() == "Reserved"){
            cout << "Book is reserved" << endl;
            return;
        }
        if(getAcc().getFine()>0){
            cout << "You have a fine of " << getAcc().getFine() << " to pay. Please pay the outstanding fine amount before borrowing a new book." << endl;
            return;
        }
        book->updtBook("status","Borrowed");
        getAcc().addBorrBook(book, CurrDate());
        cout << "Book borrowed successfully." << endl;
    }
    void returnBook(Book* book){
        int overdueDays = getAcc().periodBorr(book)-15;
        if(overdueDays>0){
            getAcc().addFine(overdueDays*10);
            cout << "Book returned late by " << overdueDays 
                 << " days. Fine imposed: " << overdueDays*10 << " rupees." << endl;
        }
        getAcc().rmBorrBook(book);
    }   
    void addBook(const string &title, const string &auth, const string &pub, const string &yr, const string &ISBN) {}
    void rmBook(const string &ISBN){}
    void updtBook(const string &ISBN, const string &field, const string &newValue){}
    void addUser(User* user){}
    void rmUser(User* user){}
    User* identifyUser(int id){ return nullptr; }
};

class Faculty: public User{
    public:
    Faculty(int id, string name): User(id, name, "Faculty"){}
    void borrowBook(Book* book){
        //need to add the function to find if any book is overdue for more than 60 days
        for(int i=0; i<getAcc().getborrBooks().size(); i++){
            if(getAcc().periodBorr(getAcc().getborrBooks()[i].first)>60){
                cout << "You have a book overdue for more than 60 days. Please return the book before borrowing a new one." << endl;
                return;
            }
        }
        if(getAcc().borrBooksCount()>=5){
            cout << "You have reached the maximum borrowing limit" << endl;
            return;
        }
        if(book->getstat() == "Borrowed"){
            cout << "Book is already borrowed" << endl;
            return;
        }
        if(book->getstat() == "Reserved"){
            cout << "Book is reserved" << endl;
            return;
        }
        book->updtBook("status", "Borrowed");
        getAcc().addBorrBook(book, CurrDate());
        cout << "Book borrowed successfully." << endl;
    }
    void returnBook(Book* book){
        int overdueDays = getAcc().periodBorr(book)-30;
        if(overdueDays > 0){
            cout<< "Book returned late by " << overdueDays << " days." <<endl;
        }
        getAcc().rmBorrBook(book);
    }
    void addBook(const string &title, const string &auth, const string &pub, const string &yr, const string &ISBN) {}
    void rmBook(const string &ISBN){}
    void updtBook(const string &ISBN, const string &field, const string &newValue){}
    void addUser(User* user){}
    void rmUser(User* user){}
    User* identifyUser(int id){ return nullptr;}
};

class Library{
    public:
    vector<Book> books;
    vector<User*> users;
    void addBook(const string &title, const string &auth, const string &pub, const string &yr, const string &ISBN) {
        books.push_back(Book(title, auth, pub, yr, ISBN, "Available")); 
    }
    void addUser(User* user){users.push_back(user);}
    void rmBook(const string &ISBN){
        for(int i=0; i<books.size(); i++){
            if(books[i].getISBN() == ISBN){
                books.erase(books.begin()+i);
                break;
            }
        }
    }
    void rmUser(User* user){
        for(int i=0; i<users.size(); i++){
            if(users[i]->getId() == user->getId()){
                users.erase(users.begin()+i);
                break;
            }
        }
    }
    void updtBook(const string &ISBN, const string &field, const string &newValue){
        for(auto &book: books){
            if(book.getISBN() == ISBN){
                book.updtBook(field, newValue);
                break;
            }
        }
    }
    User* idtUser(int userID){
        for(auto user: users){
            if(user->getId() == userID) return user;
        }
        cout << "User with ID " << userID << " not found." << endl;
        return nullptr;
    }
};

class Librarian: public User{
    private:
    Library &lib;
    
    public:
    Librarian(int id, string name, Library &lib): User(id, name, "Librarian"), lib(lib){}
    void borrowBook(Book* book){}
    void returnBook(Book* book){}
    void addBook(const string &title, const string &auth, const string &pub, const string &yr, const string &ISBN) {
        lib.addBook(title, auth, pub, yr, ISBN);
    }
    User* identifyUser(int id){ 
        return lib.idtUser(id); }
    void rmBook(const string &ISBN){    lib.rmBook(ISBN);}
    void addUser(User* user){ lib.addUser(user); }
    void rmUser(User* user){ lib.rmUser(user);}
    void updtBook(const string &ISBN, const string &field, const string &newValue){
        lib.updtBook(ISBN, field, newValue);
    }
};

//Functions to load and save data in/from files
void loadBooks(Library &lib) {
    ifstream file("books.txt");
    string title, auth, pub, yr, ISBN, stat;
    while (file >> title >> auth >> pub >> yr >> ISBN >> stat) {
        lib.addBook(title, auth, pub, yr, ISBN);
        lib.books.back().updtBook("status", stat);
    }
    file.close();
}

void storeBooks(Library &lib) {
    ofstream file("books.txt");
    for (auto &book : lib.books) {
        file << book.getTitle() << " " << book.getauth() << " " << book.getpub() << " " << book.getyr() << " " << book.getISBN() << " " << book.getstat() << endl;
    }
    file.close();
}

void loadUsers(Library &lib) {
    ifstream file("users.txt");
    int id;
    string name, userType;
    while (file >> id >> name >> userType) {
        User* user;
        if (userType == "Student") {
            user = new Student(id, name);
        } else if (userType == "Faculty") {
            user = new Faculty(id, name);
        } else if (userType == "Librarian") {
            user = new Librarian(id, name, lib);
        }
        lib.addUser(user);
    }
    file.close();
}

void storeUsers(Library &lib) {
    ofstream file("users.txt");
    for (auto &user : lib.users) {
        file << user->getId() << " " << user->getName() << " " << user->getAcc().getUserType() << endl;
    }
    file.close();
}

void loadAccounts(Library &lib) {
    ifstream file("accounts.txt");
    int userId, fine;
    string userType, ISBN, date, date2;
    while (file >> userId >> fine >> userType) {
        User* user = lib.idtUser(userId);
        if (user) {
            user->getAcc().addFine(fine);
            while (file >> ISBN) {
                if (ISBN == "END") break;
                file >> date;
                file >> date2;
                Book* book = nullptr;
                for (auto &b : lib.books) {
                    if (b.getISBN() == ISBN) {
                        book = &b;
                        break;
                    }
                }
                if (book) {
                    date.append(" ");
                    date.append(date2);
                    user->getAcc().addBorrBook(book, date);
                }
            }
            while (file >> ISBN) {
                if (ISBN == "ENDHISTORY") break;
                Book* book = nullptr;
                for (auto &b : lib.books) {
                    if (b.getISBN() == ISBN) {
                        book = &b;
                        break;
                    }
                }
                if (book) {
                    user->getAcc().getborrHist().push_back(book);
                }
            }
        }
    }
    file.close();
}

void storeAccounts(Library &lib) {
    ofstream file("accounts.txt");
    if (!file.is_open()) {
        cout << "Could not open accounts file." << endl;
        return;
    }
    for (auto &user : lib.users) {
        file << user->getId() << " " << user->getAcc().prevUnpaidFine() << " " << user->getAcc().getUserType() << endl;
        for (auto &pair : user->getAcc().getborrBooks()) {
            file << pair.first->getISBN() << " " << pair.second << endl;
        }
        file << "END" << endl;
        for (auto &book : user->getAcc().getborrHist()) {
            file << book->getISBN() << endl;
        }
        file << "ENDHISTORY" << endl;
    }
    file.close();
}
// Functions to make the command line interface
void displayLibrarianMenu() {
    cout << "Library Management System - Librarian" << endl;
    cout << "1. Add Book" << endl;
    cout << "2. Remove Book" << endl;
    cout << "3. Add User" << endl;
    cout << "4. Remove User" << endl;
    cout << "5. Update Book" << endl;
    cout << "6. View Books in the Library" << endl;
    cout << "7. Exit" << endl;
    cout << "Enter your choice: ";
}
void displayUserMenu() {
    cout << "Library Management System - User" << endl;
    cout << "1. Borrow Book" << endl;
    cout << "2. Return Book" << endl;
    cout << "3. View Borrowed Books" << endl;
    cout << "4. View Borrowing History" << endl;
    cout << "5. View Fine" << endl;
    cout << "6. Pay Fine" << endl;
    cout << "7. Exit" << endl;
    cout << "Enter your choice: ";
}

void operAddBook(User* user) {
    string title, auth, pub, yr, ISBN;
    cout << "Enter title: ";
    cin >> title;
    cout << "Enter author: ";
    cin >> auth;
    cout << "Enter publisher: ";
    cin >> pub;
    cout << "Enter year: ";
    cin >> yr;
    cout << "Enter ISBN: ";
    cin >> ISBN;
    user->addBook(title, auth, pub, yr, ISBN);
    cout << "Book added successfully." << endl;
}
void operrmBook(User* user){
    string ISBN;
    cout << "Enter ISBN: ";
    cin >> ISBN;
    user->rmBook(ISBN);
    cout << "Book removed successfully." << endl;
}
void operAddUser(User* user){
    int id;
    string name, userType;
    cout << "Enter user ID: ";
    cin >> id;
    cout << "Enter user name: ";
    cin >> name;
    cout << "Enter user type (Student/Faculty): ";
    cin >> userType;
    User* newUser;
    if(userType == "Student"){
        newUser = new Student(id, name);
    } else if(userType == "Faculty"){
        newUser = new Faculty(id, name);
    }
    user->addUser(newUser);
    cout << "User added successfully." << endl;
}
void operrmUser(User* user){
    int id;
    cout << "Enter user ID: ";
    cin >> id;
    User* targetUser = user->identifyUser(id);
    if(targetUser){
        user->rmUser(targetUser);
        cout << "User removed successfully." << endl;
    }
    else{
        cout << "User not found." << endl;
    }
}
void operupdtBook(User* user){
    string ISBN, field, newValue;
    cout << "Enter ISBN: ";
    cin >> ISBN;
    cout << "Enter field to update (status/title/author/publisher/year/ISBN): ";
    cin >> field;
    cout << "Enter new value: ";
    cin >> newValue;
    user->updtBook(ISBN, field, newValue);
    cout << "Book updated successfully." << endl;
}
void operViewBooks(Library &lib){
    cout << "Books in the Library:" << endl;
    for (auto &book : lib.books) {
        cout << "Title: " << book.getTitle() << ", author: " << book.getauth() << ", publisher: " << book.getpub() << ", Year: " << book.getyr() << ", ISBN: " << book.getISBN() << ", stat: " << book.getstat() << endl;
    }
}
void operBorrBook(User* user, Library &lib){
    string ISBN;
    cout << "Enter ISBN: ";
    cin >> ISBN;
    Book* book = nullptr;
    for(auto &b: lib.books){
        if(b.getISBN() == ISBN){
            book = &b;
            break;
        }
    }
    if(book){
        user->borrowBook(book);
    }
    else{
        cout << "Book not found." << endl;
    }
}
void operReturnBook(User* user, Library &lib){
    string ISBN;
    cout << "Enter ISBN: ";
    cin >> ISBN;
    Book* book = nullptr;
    for(auto &b: user->getAcc().getborrBooks()){
        if(b.first->getISBN() == ISBN){
            book = b.first;
            break;
        }
    }
    if (book) {
        user->returnBook(book);
        cout << "Book returned successfully." << endl;
    } else {
        cout << "Book was not issued by you." << endl;
    }
}
void operViewborrBooks(User* user){
    auto borrBooks = user->getAcc().getborrBooks();
    cout << "Borrowed Books:" << endl;
    for (auto &pair: borrBooks) {
        cout << "Title: " << pair.first->getTitle() << ", Borrowed Date: " << pair.second << endl;
    }
}
void operViewborrHist(User* user){
    auto borrHist = user->getAcc().getborrHist();
    cout << "Borrowing History:" << endl;
    for(auto &book: borrHist){ cout << "Title: " << book->getTitle() << endl;}
}
void operViewFine(User* user){
    cout << "Fine: " << user->getAcc().getFine() << " rupees" << endl;
}

// Main function to run the program
int main(){
    Library lib;
    loadBooks(lib);
    loadUsers(lib);
    loadAccounts(lib);
    while(1){
        int a = 1;
        cout << "Enter your user ID: ";
        int userId;
        cin >> userId;
        User* user = lib.idtUser(userId);
        if(!user){
            cout << "User not found." << endl;
            break;
        }
        if(user->getAcc().getUserType() == "Librarian"){
            while(a){
                cout << "Welcome " << user->getName() << endl;
                displayLibrarianMenu();
                int choice;
                cin >> choice;
                switch(choice){
                    case 1: operAddBook(user); break;
                    case 2: operrmBook(user); break;
                    case 3: operAddUser(user); break;
                    case 4: operrmUser(user); break;
                    case 5: operupdtBook(user); break;
                    case 6: operViewBooks(lib); break;
                    case 7: cout << "Exiting..." << endl;
                            a = 0; break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                        a = 0;
                        break;
                }
            }
        }
        else if(user->getAcc().getUserType() == "Student"||user->getAcc().getUserType() == "Faculty"){
            while(a){
                cout << "Welcome " << user->getName() << endl;
                displayUserMenu();
                int choice;
                cin >> choice;
                switch(choice){
                    case 1: operBorrBook(user, lib); break;
                    case 2: operReturnBook(user, lib); break;
                    case 3: operViewborrBooks(user); break;
                    case 4: operViewborrHist(user); break;
                    case 5: if(user->getAcc().getUserType() == "Faculty"){
                            cout << "Fine: 0 rupees" << endl; break;}
                            operViewFine(user); break;
                    case 6: if(user->getAcc().getUserType() == "Faculty"){
                        cout << "Fine Paid: 0 rupees" << endl; break;}
                            user->getAcc().payFine(); break;
                    case 7: cout << "Exiting..." << endl;
                            a = 0; break;
                    default:
                        cout << "Invalid choice. Please try again." << endl;
                        a = 0;
                        break;
                }
            }
        }
        break;
    }
    storeBooks(lib);
    storeUsers(lib);
    storeAccounts(lib);
    return 0;
}
