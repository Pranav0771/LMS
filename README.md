# Library Management System

This is a C++ console-based Library Management System. 

## Features

**Managing Users:** Using this system the admin(Librarian) can add, remove, and update users (Students, Faculty, Librarians) of the library.

**Managing Books:** The admin(Librarian) can add, remove, update books and their status in the library.

**Borrowing/Returning Books:** Users can borrow and return books with due date tracking.

**Reserved Books:** Reserved books are meant for only reading in the librrary. No one can issue a reserved book. 

**Managing fines:** Automatically calculates overdue fines when view fine option is chosen. The user can pay outstanding fine amount by choosing pay fine option.

**File Persistence:** Data is stored in the text files books.txt, users.txt, and accounts.txt so that the data isn't lost if the System shuts down or crashes.

**Admin Privileges:** Librarians can manage the library i.e. books and users.

## Usage

Make sure you are in the right folder containing all the required files(test.cpp, accounts.txt, books.txt, accounts.txt) 

Install a cpp compiler like g++, etc.

Compile and run the code by running the commands:
- g++ -g -o test test.cpp
- gdb ./test
- run

The system starts.

Enter your user ID to log in. The system shuts if a wrong user ID was entered.

On entering correct details the system welcomes you with your name.

Then the command line displays certain options to navigate through the system.

If you are a librarian, you can use the menu options to manage books and users.

If you are a student or faculty, you can borrow and return books or check and pay your fine using the menu options.

Fine must be paid before borrowing a new book.

And there are certain restrictions on the number of books being borrowed, the system will not allow you to borrow new books if any restriction is violated.

To close the system choose exit option. The code needs to be rerun to start the system again.

## Database

Certain files are made to store and load data to/from the system.

books.txt: Stores book details(Title,Author,Publisher,Year,ISBN,Status).

users.txt: Stores user details(ID,Name,UserType).

accounts.txt: Stores borrowing history and fines.
