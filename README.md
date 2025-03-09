# **Library Management System**

### This is a program written in C++ that manages the working of a library. There are 3 different users, Librarians, Students and Faculty. Students and Faculty may borrow, reserve and return books while Librarians may manage books and users.

#### to compile program use the command:
``` g++ -std=c++17 main.cpp -o library_program -lstdc++fs ```

#### to run, use:
```.\library_program.exe ```

- When the program is run for the first time and the required files for data storing are not created yet, the files for storing passwords, account balances, book records and the history and currently held book for each user (student or faculty) are created. 3 faculty, 5 students, 1 librarian and 10 books are also added.

- When a Student user is being added, the user must deposit an amount of money and this amount is deducted from, when the user pays their overdue fines. The user may also deposit any amount at any time to their library account. The amount deducted from their account gets added to the Library's account.

- When the program is run, the user will be asked to enter his userID and password. The userID for a librarian is in the form "L`<user number>`" and for the students and faculty there is an "S" and "F" respectively instead of "L". 

-A user can logout after usage and the program implementation continues. The next user may login and all the changes the previous user has made are effective immediately even if the updates haven't been made into the .csv files. This is possible due to the runtime memory usage such as maps and vectors.

-After the password is entered, a list of functions are listed. The user may also press "9" to exit.
    
    -Librarian:
        - Add Book
        - Add User
        - Remove Book
        - Remove User
        - View all the dues of the Students
        - View the dues of a particular Student
    
    -Student:
        - Borrow Book
        - Return Book
        - Reserve Book 
        - Pay Dues
        - View History

    -Faculty:
        - Borrow Book
        - Return Book
        - Reserve Book
        - View History

- A student or faculty can reserve a book when it is not available in the Library and being held by someone else. This ensures that once the book is returned, another person cannot borrow the book for a time period of 5 days.

## .csv files

-account_balances.csv:
Contains the account balance of the Library and Students.

-user_fines.csv:
Contains the dues of each Student user

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
    - UserID of reserver ()
    - Date reserved


