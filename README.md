# **Library Management System**

### This is a program written in C++ that manages the working of a library. There are 3 different users, Librarians, Students and Faculty. Students and Faculty may borrow, reserve and return books while Librarians may manage books and users.

#### to compile program use the command (for gcc compiler and **PLEASE USE C++17**):
``` g++ -std=c++17 main.cpp -o library_program -lstdc++fs ```

#### to run, use:
```.\library_program.exe ```

- The binay executable has been shared, along with the .csv files that get created on first time program execution. The function addUser creates new files in the folder for Student and Faculty user everytime it is called.

- When the program is run for the first time and the required files for data storing are not created yet, the files for storing passwords, account balances, book records and the history and currently held book for each user (student or faculty) are created. 3 faculty, 5 students, 1 librarian and 10 books are also added.

- Another assumption made is that the program is not run continuously for more that 1 day as this will cause an error in the fine calculations that happen during the file reading of book_records.csv and if the program is run for more than a day, the system does not take into account the overdue conditions. The functionality of reserve being lifted after 5 days of returning is also not implemented correctly.

- When a Student user is being added, the user must deposit an amount of money and this amount is deducted from, when the user pays their overdue fines. The user may also deposit any amount at any time to their library account. The amount deducted from their account gets added to the Library's account.

- When the program is run, the user will be asked to enter his userID and password. The userID for a librarian is in the form "L`<user number>`" and for the students and faculty there is an "S" and "F" respectively instead of "L". 

-A user can logout after usage and the program implementation continues. The next user may login and all the changes the previous user has made are effective immediately even if the updates haven't been made into the .csv files. This is possible due to the runtime memory usage such as maps and vectors. When asked "Do you want to close the program", 1 closes the program and 0 asks to login again.

-After the password is entered and the user logs in, a list of overdue books with the number of days they have been due in displayed. And a list of possible functions is displayed. The user may also press "9" to log out.
    
    -Librarian:
        - Add Book
        - Add User
        - Remove Book (cannot be removed incase the book is being held by user)
        - Remove User (if the librarian tries to remove a user with dues or currently held books, the system shall not allow it)
        - View all the dues of the Students
        - View the dues of a particular Student
    
    -Student:
        - Borrow Book (cannot borrow in case of limit (3) exceeding or overdue books or payement dues)
        - Return Book
        - Reserve Book (cannot reserve in case of overdue books ot payement dues)
        - Pay Dues
        - View History

    -Faculty:
        - Borrow Book (cannot borrow in case of an overdue book being held for more than 60 days)
        - Return Book
        - Reserve Book (cannot reserve in case overdue books are held for more than 60 days)
        - View History

- A student or faculty can reserve a book when it is not available in the Library and being held by someone else. This ensures that once the book is returned, another person cannot borrow the book for a time period of 5 days. After 5 days, the reserve is lifted and anyone can borrow. A user cannot reserve a book that is not being held currently, the system will notify the user that the book is available and they can borrow it. A user cannot reserve a book that is already reserved.

- A student may pay any amount at a time to clear dues partially or completely. This amount is deducted from their balances and added into the Library account's balance. If the amount paid is more than the fines incurred, the extra amount is added into the deposit (it does not get deducted from the deposit).

## .csv files

    -account_balances.csv:
Contains the account balance of the Library and Students.

    -user_fines.csv:
    Contains the dues of each Student user for the books they have returned. But during runtime, the dues with respect to the returned books (non-dynamic dues) and the dues with respect to the currently held books (dynamic dues) both are calculated and considered.

    -user_records.csv:
    Contains the usernames and passwords of the users

    -book_records.csv:
    Contains the following info for a book:
        - ISBN number
        - Title
        - Author
        - Publications
        - Year Published
        - UserID of borrower (is "-1" if book is currently not being held by any user)
        - Date borrowed (is 0/0/0 is the book is currently not being held by any user)
        - UserID of reserver (is "-1" if the book is not reserved)
        - Date returned (is 0/0/0 if the book is being held, is the date when the book was last returned)

    - `<userID>`_history.csv
    contains the ISBN number, Date borrowed and Date Returned for all the books that have been returned by the user.

    - `<userID>`_current.csv
    contains the ISBN number, Date borrowed for all the books currently being held by the user.




