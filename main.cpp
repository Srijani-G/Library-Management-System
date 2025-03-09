#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <unordered_set>

using namespace std;

struct Date { 
    int day, month, year;
    
    Date() : day(0), month(0), year(0) { }
    Date(int d, int m, int y) : day(d), month(m), year(y) { }
    
    static Date toDate(const string &dateStr) { //this function converts from string to date format
        int d, m, y;
        stringstream ss(dateStr);
        char sep;
        ss >> d >> sep >> m >> sep >> y;
        return Date(d, m, y);
    }

    static string toString(Date d) { //this function converts Date to string
        string ans;
        string temp = to_string(d.day);
        ans = ans + temp + "/";
        temp = to_string(d.month);
        ans = ans + temp + "/";
        temp = to_string(d.year);
        ans = ans + temp;
        return ans;
    }

    bool isLeapYear(int y) const {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    int toDays() const { //converts a date to number of days since year 0
        int totalDays = day;  
        int monthDays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        for (int i = 0; i < month - 1; i++) {
            totalDays += monthDays[i];
        }

        if (month > 2 && isLeapYear(year)) {
            totalDays += 1;
        }

        for (int y = 0; y < year; y++) {
            totalDays += isLeapYear(y) ? 366 : 365;
        }

        return totalDays;
    }

    int operator-(const Date &other) const { //calculates the difference in two dates
        return this->toDays() - other.toDays();
    }
};

using namespace std;

bool isEmptyExceptHeader(const string& filename) { //checks if a csv file is empty except its headers
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file!\n";
        return false; 
    }
    string header, line;
    if (!getline(file, header)) {
        return true; 
    }

    
    if (!getline(file, line)) {
        return true; 
    }

    return false; 
}

class Book {
    friend class Library;
    friend class Librarian;
public:
    int ISBN;
    string title;
    string author;
    string publisher;
    int year;
    
    Book(int a, string t, string aut, string pub, int y, string bor, string reser, Date d, Date d1) :
        ISBN(a),
        title(t),
        author(aut),
        publisher(pub),
        year(y),
        borrower(bor), 
        reserver(reser), 
        returned(d),
        borrowed(d1) {} 

private:
    string borrower; // This stores the userID of the user currently holding the book, it is "-1" if book not being held
    string reserver; // This stores the userID of the user who reserved the book, it is "-1" if nobody reserved it
    Date returned; // Stores the date when the book got returned last, it is 0/0/0 if the book is still being held
    Date borrowed; //Stores the date when the book got borrowed, it is 0/0/0 otherwise
};

class User {
protected:
    string password;
public:
    string userid;
};

class Account {
protected:
    int balance;
};

class Library: public Account {
    friend class Librarian;
private:
    static unordered_map<string, string> UserRecords;
    static unordered_map<int, shared_ptr<Book>> BookRecords;
    static unordered_map<string, int> BalanceRecs;
    static unordered_map<string, int> MoneyOwedonCurrent;
    static unordered_map<string, int> MoneyOwedonHistory;
    
    void createFile_new(string username, string password) { // is called when we need to create files for new users
        string filename = username + "_current.csv";
        string filename1 = username + "_history.csv";
        ofstream file(filename, ios::out);
        
        if (!file) {
            cerr << "Error creating user_current file!" << endl;
            return;
        }
        
        file << "ISBN,Date Borrowed\n";
        file.close();
        
        file.open(filename1, ios::out);
        if (!file) {
            cerr << "Error creating user_history file!" << endl;
            return;
        }
        
        file << "ISBN,Date Borrowed,Date Returned\n";
        file.close();
        
        file.open("user_records.csv", ios::app);
        if (!file) {
            cerr << "Error opening user_records file!" << endl;
            return;
        }
        file << username << "," << password << "\n";
        cout<<"Added details into files"<<endl;
        file.close();
    }
    
    void addUser(int type, string password, int balance) {
        if (type == 0) {
            int num = UserRecords.size() + 1;
            string nums = to_string(num);
            string username = "L" + nums;
            UserRecords[username] = password;

            ofstream file("user_records.csv", ios::app);
            if (!file) {
            cerr << "Error opening user_records file!" << endl;
            return;
            }
            file << username << "," << password << "\n";
            cout<<"Added details into files"<<endl;
            file.close();
            cout<<"The new user's userid is: "<<username<<endl;
            
        }
        else if (type == 1) {
            int num = UserRecords.size() + 1;
            string nums = to_string(num);
            string username = "F" + nums;
            UserRecords[username] = password;
            cout<<"The new user's userid is: "<<username<<endl;
            createFile_new(username, password);
        }
        else {
            int num = UserRecords.size() + 1;
            string nums = to_string(num);
            string username = "S" + nums;
            UserRecords[username] = password;
            BalanceRecs[username] = balance;
            cout<<"The new user's userid is: "<<username<<" and the amount deposited is:"<<balance<<endl;
            createFile_new(username, password);
        }
    }

    void displayBalance() { // displays the amount collected by the Library in fines
        cout << BalanceRecs["L0"] << endl;
    }

    void removeUser(string userid) { // removes user and deletes _history and _current files
        UserRecords.erase(userid);
        BalanceRecs.erase(userid);
        MoneyOwedonCurrent.erase(userid);
        MoneyOwedonCurrent.erase(userid);
        fstream fin, fout;
        fin.open("user_records.csv", ios::in);
        fout.open("user_recordsnew.csv", ios::out);
        vector<pair<string, string>> temp;
        string line;
        getline(fin, line); 
        fout << line << "\n"; 

        while (getline(fin, line)) {
            stringstream ss(line);
            string useridentity;
            string userpass;
            getline(ss, useridentity, ','); 
            getline(ss, userpass, ',');
            if (useridentity != userid)
                temp.push_back({useridentity, userpass});
        }
    
        fin.close();

        for (const pair<string, string>& l : temp) {
            fout << l.first << "," << l.second << "\n";
        }
        fout.close();

       
        remove("user_records.csv");
        rename("user_recordsnew.csv", "user_records.csv");
        string filename = userid + "_history.csv";
        filesystem::remove(filename);
        filename = userid + "_current.csv";
        filesystem::remove(filename);
        cout << "Entry with userid: " << userid << " deleted successfully in user-records.\n";
    }

    void removeBook(int x) {
        if(BookRecords[x]->borrower!="-1"){ //If the book has been borrowed and not returned yet
            cout<<"The book is currently being held by a user, wait till it is returned to delete"<<endl;
            return;
        }
        BookRecords.erase(x);
    }

    void addBook(int x, string t, string a, string p, int year) {
        BookRecords[x] = make_shared<Book>(x, t, a, p, year, "-1", "-1", Date(0, 0, 0), Date(0,0,0)); 
    }
    
public:
    bool verify(string username, string password) { //verifies password against that username
        if (UserRecords[username] == password) return 1;
        else return 0;
    }

    bool transfer(string username, int amount) { //transfers money from deposit of user to Library
        if (BalanceRecs[username] < amount) {
            cout << "You have insufficient balance in you deposit, please deposit more" << endl;
            return 0;
        }
        BalanceRecs[username] -= amount;
        BalanceRecs["Library"] += amount;
        if(MoneyOwedonHistory[username]+MoneyOwedonCurrent[username]>amount){
            MoneyOwedonHistory[username]-=amount;
        }
        else{      
            if(MoneyOwedonHistory[username]+MoneyOwedonCurrent[username]<amount){ //if the amount paid is greater than the fines incurred then the extra amount is added to deposit
                BalanceRecs[username]+=amount-(MoneyOwedonHistory[username]+MoneyOwedonCurrent[username]);
                cout<<"You have paid:"<< amount-(MoneyOwedonHistory[username]+MoneyOwedonCurrent[username])<<" extra, this amount shall be kept in you deposit"<<endl;
            }                             
            MoneyOwedonHistory.erase(username);
            MoneyOwedonCurrent.erase(username);
        }
        return 1;
    }

   

    int check_available(int isbn, string s) { // determines state of book
        if (BookRecords[isbn]->borrower != "-1" && BookRecords[isbn]->reserver != "-1" && BookRecords[isbn]->reserver !=s) return 0; // book is being held and is reserved by someone else
        else if(BookRecords[isbn]->borrower != "-1" && BookRecords[isbn]->reserver == s) return 4; //the book is being held and you have reserved
        else if(BookRecords[isbn]->borrower != "-1" && BookRecords[isbn]->reserver == "-1") return 1; //book is being held but not reserved
        else if(BookRecords[isbn]->borrower == "-1" && BookRecords[isbn]->reserver !="-1" && BookRecords[isbn]->reserver !=s) return 2; //book is reserved by someone else but not held
        else if(BookRecords[isbn]->borrower == "-1" && BookRecords[isbn]->reserver ==s) return 3; //book is not held and reserved by user
        else return 5; //book is not being held and not reserved
    }

    void borrow(int isbn, string userid, Date borrowed) { //updates the members of Book
        BookRecords[isbn]->borrower = userid;
        BookRecords[isbn]->borrowed = borrowed;
        BookRecords[isbn]->returned = Date(0, 0, 0);
        if (BookRecords[isbn]->reserver == userid) { //if the reserver borrows the book
            BookRecords[isbn]->reserver = "-1";
            

        }
    }
    void returnBook(int isbn, Date d) {
        if(BookRecords[isbn]->borrower[0]=='S' && d-BookRecords[isbn]->borrowed>15){ // changes the dynamic fine into non-dynamic fine
            MoneyOwedonCurrent[BookRecords[isbn]->borrower]-=(d-BookRecords[isbn]->borrowed-15)*10;
            MoneyOwedonHistory[BookRecords[isbn]->borrower]+=(d-BookRecords[isbn]->borrowed-15)*10;

        }
        BookRecords[isbn]->borrower = "-1"; //updates Book member attributes
        BookRecords[isbn]->borrowed = Date(0,0,0);
        BookRecords[isbn]->returned=d;
        
    }

    void reserveBook(int isbn, string userid, Date current) { // updates the member attributes of Book when reserve is called
        BookRecords[isbn]->reserver = userid;
        
    }

    int display_due_foruser(string username){ //displays dues of a particular user
        return MoneyOwedonCurrent[username]+MoneyOwedonHistory[username];
    }
    
    Library(Date current) {
        if (!filesystem::exists("user_records.csv")) { //first time the program is being run
            fstream book_recs("book_records.csv", ios::out);
            book_recs << "ISBN,title,author,publisher,year,borrower,reserver,date returned, date borrowed\n";
            cout << "Created book-records and added header" << endl;
            book_recs.close();

            fstream user_fines("user_fines.csv", ios::out);
            user_fines << "UserID,Money Owed\n";
            cout << "Created record for money owed and added header" << endl;
            user_fines.close();

            fstream acc_recs("account_balances.csv", ios::out);
            acc_recs << "UserID,Balance\n";
            acc_recs << "Library,0\n";
            BalanceRecs["Library"] = 0;
            cout << "Created acc-records and added header" << endl;
            acc_recs.close();
            
            fstream user_recs("user_records.csv", ios::out);
            user_recs << "UserID,Password\n";
            user_recs << "L1,meow123\n";
            user_recs.close();
            cout << "Created user-records and added header" << endl;
            UserRecords["L1"] = "meow123";
            addUser(2,"bleh",500);
            addUser(2,"bleh1",500);
            addUser(2,"bleh2",500);
            addUser(2,"bleh3", 500);
            addUser(2,"bleh4",500);
            addUser(1, "bleh5",0);
            addUser(1, "bleh6",0);
            addUser(1, "bleh7",0);
            addBook(100, "Harry_Potter","JK_Rowling","Penguin_Publications",2005);
            addBook(108, "It", "Stephen_King", "Viking", 1986);
            addBook(109, "The_Haunting_of_Hill_House", "Shirley_Jackson", "Viking_Press", 1959);
            addBook(101, "Hamlet", "William_Shakespeare", "Simon_&_Schuster", 1603);
            addBook(102, "Dracula", "Bram_Stoker", "Archibald_Constable_&_Co", 1897);
            addBook(103, "Dune", "Frank_Herbert", "Chilton_Books", 1965);
            addBook(104, "Murder_on_the_Orient_Express", "Agatha_Christie", "Collins_Crime_Club", 1934);
            addBook(105, "Frankenstein", "Mary_Shelley", "Lackington_Allen_&_Co", 1818);
            addBook(106, "Percy_Jackson_&_the_Olympians:_The_Lightning_Thief", "Rick_Riordan", "Disney_Hyperion", 2005);
            addBook(107, "The_Shining", "Stephen_King", "Doubleday", 1977);

            
        } else { //not the first time program is being run, needs to extract data from files 
            ifstream user_recs("user_records.csv"); //extracting usernames and passwords into UserRecords map
            
            if (!user_recs) {
                cout << "Error opening user authentication file!" << endl;
            }
            
            string line, userid, pass;
            getline(user_recs, line); // takes header first
        
            while (getline(user_recs, line)) {
                stringstream ss(line);
                getline(ss, userid, ',');
                getline(ss, pass, ',');
        
                UserRecords[userid] = pass;
                
            }
            user_recs.close();
            ifstream user_fines("user_fines.csv"); //extracting usernames and passwords into UserRecords map
            
            if (!user_fines) {
                cout << "Error opening user authentication file!" << endl;
            }
            
            string amt;
            getline(user_fines, line); // takes header first
        
            while (getline(user_fines, line)) {
                stringstream ss(line);
                getline(ss, userid, ',');
                getline(ss, amt, ',');
        
                MoneyOwedonHistory[userid]=stoi(amt);
                
            }
            user_fines.close();

            ifstream book_recs("book_records.csv"); //extracting Book data into BookRecords map
            if (!book_recs) {
                cout << "Error accessing the book records" << endl;
            }
            
            string ISBN;
            string title, author, publisher, year, borrower, reserver, temp, temp1;
            getline(book_recs, line); // takes header first
            
            while (getline(book_recs, line)) {
                stringstream ss(line);
                getline(ss, ISBN, ',');
                getline(ss, title, ',');
                getline(ss, author, ',');
                getline(ss, publisher, ',');
                getline(ss, year, ',');
                getline(ss, borrower, ',');
                getline(ss, reserver, ','); 
                getline(ss, temp, ','); 
                Date date = Date::toDate(temp);
                getline(ss , temp1,',');
                Date date1 = Date::toDate(temp1);
                if ((current - date > 5) && reserver!="-1" && borrower=="-1") // checks if the reserver has reserved but hasn't borrowed for 5 days after the previous user already returned
                    BookRecords[stoi(ISBN)] = make_shared<Book>(stoi(ISBN), title, author, publisher, stoi(year), borrower, "-1", Date(0, 0, 0), date1); 
                else 
                    BookRecords[stoi(ISBN)] = make_shared<Book>(stoi(ISBN), title, author, publisher, stoi(year), borrower, reserver, date, date1); 
                if(current - date1 > 15 && borrower[0]=='S') MoneyOwedonCurrent[borrower]=((current-date1)-15)*10; //updating the MoneyOwed map incase the borrower is a student holding for more than 15 days
            }
            book_recs.close();
            
            ifstream acc_recs("account_balances.csv"); //extracts the balances into BalanceRecs map
            if (!acc_recs) {
                cout << "Error accessing the account records" << endl;
            }
            
            string acc_balance;
            getline(acc_recs, line); // takes header first
            
            while (getline(acc_recs, line)) {
                stringstream ss(line);
                getline(ss, userid, ',');
                getline(ss, acc_balance, ',');
                BalanceRecs[userid] = stoi(acc_balance);
            }
            acc_recs.close();
        }
    }

    ~Library() {
        ofstream book_recs("book_records.csv", ios::out); // puts back BookRecords map details into file
        book_recs << "ISBN,title,author,publisher,year,borrower,reserver,date returned, date borrowed\n";
        
        if (!book_recs) {
            cout << "Error accessing the book records" << endl;
            return;
        }
        
        for (const auto& [key, value] : BookRecords) {
            book_recs << key << "," << value->title << "," << value->author << "," << value->publisher << ","
                      << to_string(value->year) << "," << value->borrower << "," << value->reserver << ","
                      << Date::toString(value->returned) <<","<< Date::toString(value->borrowed)<<"\n"; 
        }

        ofstream user_fines("user_fines.csv", ios::out); // puts back BookRecords map details into file
        user_fines << "UserID,Money Owed\n";
        
        if (!user_fines) {
            cout << "Error accessing the book records" << endl;
            return;
        }
        
        for (const auto& [key, value] : MoneyOwedonHistory) {
            user_fines << key << "," <<value<<"\n";
        }
    
        
        cout << "Succesfully written all the new fine updates into file" << endl;
        user_fines.close();

        ofstream account_recs("account_balances.csv", ios::out); //puts back BalanceRecords map data into file
        account_recs << "UserID,Balance\n";
        
        if (!account_recs) {
            cout << "Error accessing the account records" << endl;
            return;
        }
        
        for (const auto& [key, value] : BalanceRecs) {
            account_recs << key << "," << value << "\n"; 
        }
        
        cout << "Succesfully written all the new account balance updates into file" << endl;
        account_recs.close();

        
    }
};

class Librarian: public User {
public:

    void display_balance(Library& obj){
        obj.displayBalance();
    }
    void addUser(int type, string password, Library& obj, int balance) {
        obj.addUser(type, password, balance);
    }

    void removeUser(string userid, Library &obj) {
        string filename = userid + "_current.csv";
        if (!isEmptyExceptHeader(filename)) { //checks if the user that has to be removed still has books
            cout << "The user you are trying to delete is still holding some books" << endl;
            return;
        }

        if((Library::MoneyOwedonHistory[userid]+Library::MoneyOwedonCurrent[userid])!=0){ //checks if the user still owes money
            cout<<"The user still owes money"<<endl;
        }
        obj.removeUser(userid);
    }

    void removeBook(int isbn, Library &obj) {
        obj.removeBook(isbn);
    }

    void addBook(int isbn, Library &obj, string title, string author, string publisher, int year) {
        obj.addBook(isbn, title, author, publisher, year);
    }

    void display_dues(Library &obj){ // displays dues of all users
        cout<<"These are the dues of students"<<endl;
        for (const auto &pair : obj.MoneyOwedonCurrent) {
            cout << pair.first << ": " << pair.second + obj.MoneyOwedonHistory[pair.first] << "\n";
        }
        // cout<<"These are the dues of users on books they have returned"<<endl;
        // for (const auto &pair : obj.MoneyOwedonHistory) {
        //     cout << pair.first << ": " << pair.second << "\n";
        // }
    }


};

class Faculty: public User {
    friend class Librarian;
private:
    unordered_map<int, Date> current_books; // stores isbn for currently held books
    unordered_set <int> overdue; //stores isbn for books kept for more than 60 days
    
public:
    Faculty(string username, string pass, Date current) {
        userid = username;
        password = pass;
        string filename = username + "_current.csv";
        ifstream user_current(filename);
        
        if (!user_current) {
            cout << "Error accessing the currently borrowed books list" << endl;
        }
    
        string line, temp, isbn;
        getline(user_current, line);  // takes header first
        int d, m, y;
        
        while (getline(user_current, line)) {
            stringstream ss(line);
            getline(ss, isbn, ',');
            getline(ss, temp, ','); 
            Date date = Date::toDate(temp);
            current_books[stoi(isbn)] = date;
            
            if ((current - date) > 30) {
                cout << "The book: " << isbn << " has been due for " << current - date << " days" << endl;
                if ((current - date) > 60) overdue.insert(stoi(isbn));
            }

            
        }
    }

    ~Faculty() {
        string filename = this->userid + "_current.csv";
        ofstream current_book(filename, ios::out);
        current_book << "ISBN,Date Borrowed\n";
        
        if (!current_book) {
            cout << "Error accessing the current book records" << endl;
            return;
        }
        
        for (const auto& [key, value] : current_books) {
            current_book << key << "," << Date::toString(value) << "\n";
        }
        
        cout << "Succesfully written all the new books borrowed into file" << endl;
        current_book.close();
    }

    

    void display_history() { 
        string filename = this->userid + "_history.csv";
        ifstream user_history(filename);

        if (!user_history) {
            cout << "Error: Cannot open file!" << endl;
            return;
        }

        string line;
        string ISBN, d_b, d_r;
        cout << left << setw(5) << "ISBN" << setw(10) << "Date Borrowed" << setw(10) << "Date Returned" << endl;
        cout << "--------------------------------" << endl;

        getline(user_history, line);  // takes header first

        while (getline(user_history, line)) {
            stringstream ss(line);
            getline(ss, ISBN, ',');
            getline(ss, d_b, ',');
            getline(ss, d_r, ',');

            cout << left << setw(5) << ISBN << setw(10) << d_b << setw(5) << d_r << endl;
        }

        user_history.close();
        return;
    }

    void borrowBook(int isbn, Library& obj, Date current_date) {
        if (current_books.size() == 5) {
            cout << "You have reached the limit of number of books you may borrow at a time (5)" << endl;
            return;
        }

        else if (overdue.size()>0) {
            cout << "You have overdues book that you have held for more than 60 days! Please return it to continue borrowing" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==0) {
            cout << "This book is being held and is reserved by someone else" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==1) {
            cout << "This book is held but not reserved" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==4) {
            cout << "This book is still being held, but you have already reserved it" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==2) {
            cout << "This book has been reserved by someone else" << endl;
            return;
        }
        
        else {
            obj.borrow(isbn, this->userid, current_date);
            current_books[isbn] = current_date;
        }
    }

    void returnBook(int isbn, Library& obj, Date current_date) {
        auto it = current_books.find(isbn);
        if (it == current_books.end()) {
            cout << "You currently do not have the book you entered" << endl;
            return;
        }
        else {
            obj.returnBook(isbn, current_date);
            string filename = this->userid + "_history.csv";
            ofstream file(filename, ios::app);
            
            if (!file) {
                cerr << "Error opening file!" << endl;
                return;
            }
            
            string temp = to_string(current_books[isbn].day) + "/" + to_string(current_books[isbn].month) + "/" + to_string(current_books[isbn].year);
            string temp1 = to_string(current_date.day) + "/" + to_string(current_date.month) + "/" + to_string(current_date.year);
            file << to_string(isbn) << "," << temp << "," << temp1 << "\n"; 
            file.close(); 
            cout << "Data written successfully!" << endl;
            current_books.erase(isbn);
            overdue.erase(isbn);
        }
        
        return;
    }

    void reserveBook(int isbn, Library& obj, Date current_date) {
        if (overdue.size()>0) {
            cout << "You have overdue books that you have held for more than 60 days! Please return it to continue borrowing" << endl;
            return;
        }

        else if(obj.check_available(isbn, userid)==2 || obj.check_available(isbn, userid)==0){
            cout<<"Book is already reserved"<<endl;
        }
        else if(obj.check_available(isbn, userid)==4){
            cout<<"You have already reserved it, wait till book is returned"<<endl;
        }
        else if(obj.check_available(isbn, userid)==3){
            cout<<"You have already reserved it, you may borrow now"<<endl;
        }

        else if(obj.check_available(isbn, userid)==5){
            cout<<"Book is available and not reserved, pls borrow"<<endl;
        }


        else {
            obj.reserveBook(isbn, this->userid, current_date);
        }
    }
};

class Student: public User, public Account {
    friend class Librarian;
private:
    unordered_map<int, Date> current_books;
    int fine;
    unordered_set<int> overdue;
    
public:
    Student(string username, string pass, Date current) {
        userid = username;
        password = pass;
        string filename = username + "_current.csv";
        ifstream user_current(filename);
        
        if (!user_current) {
            cout << "Error accessing the currently borrowed books list" << endl;
        }
    
        string line, temp, isbn;
        getline(user_current, line);  // takes header first
        int d, m, y;
        
        while (getline(user_current, line)) {
            stringstream ss(line);
            getline(ss, isbn, ',');
            getline(ss, temp, ',');
            Date date = Date::toDate(temp);
            current_books[stoi(isbn)] = date;
            
            if ((current - date) > 15) {
                cout << "The book: " << isbn << " has been due for " << (current - date)-15 << "days" << endl;
                overdue.insert(stoi(isbn));
                
            }

        }
    }

    ~Student() {
        string filename = this->userid + "_current.csv";
        ofstream current_book(filename, ios::out);
        current_book << "ISBN,Date Borrowed\n";
        
        if (!current_book) {
            cout << "Error accessing the current book records" << endl;
            return;
        }
        
        for (const auto& [key, value] : current_books) {
            current_book << key << "," << Date::toString(value) << "\n";
        }
        
        cout << "Succesfully written all the new books borrowed into file" << endl;
        current_book.close();
    }

    void display_history() {
        string filename = userid + "_history.csv";
        ifstream user_history(filename);

        if (!user_history) {
            cout << "Error: Cannot open file!" << endl;
            return;
        }

        string line;
        string ISBN, d_b, d_r;
        cout << left << setw(5) << "ISBN" << setw(10) << "Date Borrowed" << setw(5) << "Date Returned" << endl;
        cout << "--------------------------------" << endl;

        getline(user_history, line);  // takes header first

        while (getline(user_history, line)) {
            stringstream ss(line);
            getline(ss, ISBN, ',');
            getline(ss, d_b, ',');
            getline(ss, d_r, ',');

            cout << left << setw(5) << ISBN << setw(10) << d_b << setw(5) << d_r << endl;
        }

        user_history.close();
        return;
    }

    void borrowBook(int isbn, Library& obj, Date current_date) {
        if (current_books.size() == 3) {
            cout << "You have reached the limit of number of books you may borrow at a time (3)" << endl;
            return;
        }

        else if (overdue.size() > 0) {
            cout << "You have an overdue book! Please return it to continue borrowing" << endl;
            return;
        }

        else if (obj.check_available(isbn, userid)==0) {
            cout << "This book is being held and is reserved by someone else" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==1) {
            cout << "This book is held but not reserved" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==4) {
            cout << "This book is still being held, but you have already reserved it" << endl;
            return;
        }
        else if (obj.check_available(isbn, userid)==2) {
            cout << "This book has been reserved by someone else" << endl;
            return;
        }
        
        else {
            obj.borrow(isbn, this->userid, current_date);
            current_books[isbn] = current_date;
        }
    }

    void ReserveBook(int isbn, Library& obj, Date current_date) {
        if (obj.check_available(isbn, userid)==0 || obj.check_available(isbn, userid)==2) {
            cout << "This book is reserved already" << endl;
            return;
        }
        else if(obj.check_available(isbn, userid)==2 || obj.check_available(isbn, userid)==0){
            cout<<"Book is already reserved"<<endl;
        }
        else if(obj.check_available(isbn, userid)==4){
            cout<<"You have already reserved it, wait till book is returned"<<endl;
        }
        else if(obj.check_available(isbn, userid)==3){
            cout<<"You have already reserved it, you may borrow now"<<endl;
        }

        else if(obj.check_available(isbn, userid)==5){
            cout<<"Book is available and not reserved, pls borrow"<<endl;
        }
        else {
            obj.reserveBook(isbn, this->userid, current_date);
        }
    }

    void returnBook(int isbn, Library& obj, Date current_date) {
        auto it = current_books.find(isbn);
        if (it == current_books.end()) {
            cout << "You currently do not have the book you entered" << endl;
            return;
        }
        else {
            obj.returnBook(isbn, current_date);
            string filename = this->userid + "_history.csv";
            ofstream file(filename, ios::app);
            
            if (!file) {
                cerr << "Error opening file!" << endl;
                return;
            }
            
            string temp = to_string(current_books[isbn].day) + "/" + to_string(current_books[isbn].month) + "/" + to_string(current_books[isbn].year);
            string temp1 = to_string(current_date.day) + "/" + to_string(current_date.month) + "/" + to_string(current_date.year);
            file << to_string(isbn) << "," << temp << "," << temp1 << "\n"; 
            file.close(); 
            cout << "Data written successfully!" << endl;
            current_books.erase(isbn);
            overdue.erase(isbn);
        }
    
        return;
    }

    void pay(int amount, Library &obj) {
        if (obj.transfer(this->userid, amount))
            fine -= amount;
        else {
            cout << "Payment failed due to insufficient balance" << endl;
        }
    }

   
};

unordered_map<string, string> Library::UserRecords;
unordered_map<int, shared_ptr<Book>> Library::BookRecords;
unordered_map<string, int> Library::BalanceRecs;
unordered_map <string, int> Library::MoneyOwedonCurrent;
unordered_map <string, int> Library::MoneyOwedonHistory;

int main() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm local_tm;
    
#ifdef _WIN32
    localtime_s(&local_tm, &t);
#else
    localtime_r(&t, &local_tm);
#endif

    Date today(local_tm.tm_mday, local_tm.tm_mon + 1, local_tm.tm_year + 1900);
    Library L(today);
    string userID, password;
    
    while (1) {
        bool exit = 0;
        cout << "Please enter your UserID:" << endl;
        cin >> userID;
        cout << "Please enter your Password:" << endl;
        cin >> password;
        
        if (L.verify(userID, password)) {
            cout << "Welcome!" << endl;
            
            if (userID[0] == 'L') {
                Librarian L_temp;
                
                while (1) {
                    int response = 9;
                    cout << "What would you like to do today?" << endl;
                    cout << "Add Book (press 0)" << endl;
                    cout << "Add User (press 1)" << endl;
                    cout << "Remove User (press 2)" << endl;
                    cout << "Remove Book (press 3)" << endl;
                    cout << "Display all dues (press 4)" << endl;
                    cout << "Display dues of a particular student (Press 5)" << endl;
                    cout<< "Display balance of Library (press 6)"<<endl;
                    cout << "Log Out (press 9)" << endl;
                    cin >> response;
                    
                    switch (response) {
                        default:
                            screw:break;
                            
                        case 0: {
                            int ISBN;
                            string title, author, publisher;
                            int year;
                            int confirm = 0;
                            cout << "Please enter the details of the book you want to add in the order, ISBN, title, author, publisher and year published" << endl;
                            cin >> ISBN >> title >> author >> publisher >> year;
                            cout << "Are you sure?" << endl;
                            cin >> confirm;
                            if (confirm == 0) goto screw;
                            L_temp.addBook(ISBN, L, title, author, publisher, year);
                            break;
                        }
                            
                        case 1: {
                            int type, deposit_amount = 0;
                            string password;
                            int confirm = 0;
                            cout << "Please enter new user's type and password (0 is librarian, 1 is faculty and 2 is student)" << endl;
                            cin >> type >> password;
                            if (type == 2) {
                                cout << "Please enter initial deposit amount" << endl;
                                cin >> deposit_amount;
                            }

                            cout << "Are you sure?" << endl;
                            cin >> confirm;
                            if (confirm == 0) goto screw;
                            L_temp.addUser(type, password, L, deposit_amount);
                            break;
                        }
                            
                        case 2: {
                            string userid;
                            int confirm = 0;
                            cout << "Enter the userid of the user to be removed" << endl;
                            cin >> userid;
                            cout << "Are you sure?" << endl;
                            cin >> confirm;
                            if (confirm == 0) goto screw;
                            L_temp.removeUser(userid, L);
                            break;
                        }
                            
                        case 3: {
                            int isbn;
                            int confirm = 0;
                            cout << "Enter the ISBN number of the book to be deleted" << endl;
                            cin >> isbn;
                            cout << "Are you sure?" << endl;
                            cin >> confirm;
                            if (confirm == 0) goto screw;
                            L_temp.removeBook(isbn, L);
                            break;
                        }
                        case 4:{
                            L_temp.display_dues(L);
                            break;
                        }
                        case 5:{
                            string userid;
                            cout<<"Please enter the userid of the student you would like to view the dues of"<<endl;
                            cin>>userid;
                            cout<<"This user owes:"<<L.display_due_foruser(userid)<<endl;
                            break;
                        }
                        case 6:{
                            L_temp.display_balance(L);
                            break;
                        }
                    }
                    
                    if (response == 9) {
                        break;
                    }
                }
            } else if (userID[0] == 'S') {
                Student S_temp(userID, password, today);
                
                while (1) {
                    int response = 9;
                    cout << "What would you like to do today?" << endl;
                    cout << "Borrow Book (press 0)" << endl;
                    cout << "Return Book (press 1)" << endl;
                    cout << "Reserve Book (press 2)" << endl;
                    cout << "Pay dues (press 3)" << endl;
                    cout << "Display History (press 4)" << endl;
                    cout << "Log Out (press 9)" << endl;
                    cin >> response;
                    
                    switch (response) {
                        default:
                            screw1:break;
                            
                            case 0:{
                                if(L.display_due_foruser(userID)>0){
                                    cout<<"You have "<< L.display_due_foruser(userID)<<" in dues, please pay first"<<endl;
                                    goto screw1;
                                }
                                int ISBN;
                                int confirm=0;
                                cout<<"Please enter the ISBN number of the book you want to borrow"<<endl;
                                cin>>ISBN;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw1;
                                S_temp.borrowBook(ISBN,L,today);
                                break;
                            }
                                
                            case 1:{
                                int ISBN;
                                int confirm=0;
                                cout<<"Please enter the ISBN number of the book you would like to return"<<endl;
                                cin>>ISBN;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw1;
                                S_temp.returnBook(ISBN,L,today);
                                break;
                            }
                                
                            case 2:{
                                if(L.display_due_foruser(userID)>0){
                                    cout<<"You have "<< L.display_due_foruser(userID)<<" in dues, please pay first"<<endl;
                                    goto screw1;
                                }
                                int ISBN;
                                int confirm=0;
                                cout<<"Enter the ISBN number of the book you would like to reserve"<<endl;
                                cin>>ISBN;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw1;
                                S_temp.ReserveBook(ISBN, L, today);
                                break;
                            }
                                
                            case 3:{
                                int amount;
                                int confirm=0;
                                cout<<"Enter the amount you would like to pay to clear fines"<<endl;
                                cin>>amount;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw1;
                                S_temp.pay(amount, L);
                                break;
                            }
                            case 4:{
                                S_temp.display_history();
                                break;
        
                            }
                                
                        }
                        if(response==9){
                            break;
                        }
                    }
        
                }
        
                else{
                    Faculty F_temp(userID,password,today);
                    while(1){
                        int response=9;
                        cout<<"What would you like to do today?"<<endl;
                        cout<<"Borrow Book (press 0)"<<endl;
                        cout<<"Return Book (press 1)"<<endl;
                        cout<<"Reserve Book (press 2)"<<endl;
                        cout<<"Display History (press 3)"<<endl;
                        cout<<"Log Out (press 9)"<<endl;
                        cin>>response;
                        switch(response){
                            default:
                                screw2:break;
                                
                            case 0:{
                                int ISBN;
                                int confirm=0;
                                cout<<"Please enter the ISBN number of the book you want to borrow"<<endl;
                                cin>>ISBN;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw2;
                                F_temp.borrowBook(ISBN,L,today);
                                break;
                            }
                                
                            case 1:{
                                int ISBN;
                                int confirm=0;
                                cout<<"Please enter the ISBN number of the book you would like to return"<<endl;
                                cin>>ISBN;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw2;
                                F_temp.returnBook(ISBN,L,today);
                                break;
                            }
                                
                            case 2:{
                                int ISBN;
                                int confirm=0;
                                cout<<"Enter the ISBN number of the book you would like to reserve"<<endl;
                                cin>>ISBN;
                                cout<<"Are you sure?"<<endl;
                                cin>>confirm;
                                if(confirm==0) goto screw2;
                                F_temp.reserveBook(ISBN, L, today);
                                break;
                            }
                                
                            
                            case 3:{
                                F_temp.display_history();
                                break;
                            }
                                
                        }
                        if(response==9){
                            break;
                        }
                    }
        
                }
            }
        
            else{
                cout<<"Invalid Credentials! Try Again"<<endl;
            }
        
            cout<<"Do you want to close program?"<<endl;
            cin>>exit;
            if(exit==1){
                cout<<"Thank You!"<<endl;
                break;
            }
            else continue;
            
        }
        
}