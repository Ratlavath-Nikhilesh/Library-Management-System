#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <conio.h> // for getch()

using namespace std;

struct Book {
    string name;
    int quantity;
};

unordered_map<string, Book> books;
vector<pair<string, pair<string, string>>> librarians; // username -> {role, password}
string loggedInUsername;
string loggedInRole;

const string adminUsername = "nikhi";
const string adminPassword = "nikhilesh";

// Utility function to read password with masking
string readPassword() {
    string password;
    char ch;
    while ((ch = _getch()) != '\r') { // Enter key
        if (ch == '\b') { // Backspace
            if (!password.empty()) {
                cout << "\b \b";
                password.pop_back();
            }
        } else {
            password += ch;
            cout << '*';
        }
    }
    cout << '\n';
    return password;
}

// Load books from file
void loadBooks() {
    ifstream fin("books.txt");
    if (!fin) return;

    string name;
    int quantity;
    while (getline(fin, name)) {
        fin >> quantity;
        fin.ignore();
        books[name] = {name, quantity};
    }

    fin.close();
}

// Save books to file
void saveBooks() {
    ofstream fout("books.txt");
    for (auto& b : books) {
        fout << b.first << '\n' << b.second.quantity << '\n';
    }
    fout.close();
}

// Load librarians from file
void loadLibrarians() {
    ifstream fin("librarians.txt");
    if (!fin) return;

    string username, role, password;
    while (getline(fin, username) && getline(fin, role) && getline(fin, password)) {
        librarians.push_back({username, {role, password}});
    }

    fin.close();
}

// Save librarians to file
void saveLibrarians() {
    ofstream fout("librarians.txt");
    for (auto& l : librarians) {
        fout << l.first << '\n' << l.second.first << '\n' << l.second.second << '\n';
    }
    fout.close();
}

void registerLibrarian() {
    string username;

    cout << "Enter username: ";
    cin >> username;

    if (username == adminUsername) {
        cout << "You cannot use the reserved admin username. Please try a different username.\n";
        return;
    }

    for (const auto& lib : librarians) {
        if (lib.first == username) {
            cout << "Username already exists. Please try a different one.\n";
            return;
        }
    }

    cout << "Enter password: ";
    string password = readPassword();

    librarians.push_back({username, {"user", password}});
    saveLibrarians(); // save after registration
    cout << "User registered successfully!\n";
}

bool login() {
    string username;
    cout << "Enter username: ";
    cin >> username;
    cout << "Enter password: ";
    string password = readPassword();

    if (username == adminUsername && password == adminPassword) {
        loggedInUsername = username;
        loggedInRole = "admin";
        cout << "Admin login successful.\n";
        return true;
    }

    for (auto& lib : librarians) {
        if (lib.first == username && lib.second.second == password) {
            loggedInUsername = username;
            loggedInRole = lib.second.first;
            cout << "User login successful.\n";
            return true;
        }
    }

    cout << "Invalid credentials.\n";
    return false;
}

void addBook() {
    string name;
    int quantity;
    cout << "Enter book name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter quantity: ";
    cin >> quantity;

    if (books.find(name) != books.end()) {
        books[name].quantity += quantity;
    } else {
        books[name] = {name, quantity};
    }

    saveBooks(); // save after addition
    cout << "Book added successfully.\n";
}

void removeBook() {
    string name;
    int quantity;
    cout << "Enter book name to remove: ";
    cin.ignore();
    getline(cin, name);

    if (books.find(name) != books.end()) {
        cout << "Enter quantity to remove: ";
        cin >> quantity;

        if (quantity >= books[name].quantity) {
            books.erase(name);
            cout << "All copies removed. Book deleted from library.\n";
        } else {
            books[name].quantity -= quantity;
            cout << quantity << " copies removed. Remaining: " << books[name].quantity << "\n";
        }

        saveBooks(); // save after removal
    } else {
        cout << "Book not found.\n";
    }
}

void issueBook() {
    string name;
    cout << "Enter book name to issue: ";
    cin.ignore();
    getline(cin, name);

    if (books.find(name) != books.end() && books[name].quantity > 0) {
        books[name].quantity--;
        saveBooks(); // save after issuing
        cout << "Book issued successfully.\n";
    } else {
        cout << "Book not found or unavailable.\n";
    }
}

void viewBooks() {
    if (books.empty()) {
        cout << "No books in the library.\n";
        return;
    }

    for (auto& b : books) {
        cout << "Book: " << b.first << ", Quantity: " << b.second.quantity << "\n";
    }
}

void adminMenu() {
    int choice;
    while (true) {
        cout << "\n--- Admin Menu ---\n";
        cout << "1. Add Book\n2. Remove Book\n3. Issue Book\n4. View Books\n5. Logout\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: addBook(); break;
            case 2: removeBook(); break;
            case 3: issueBook(); break;
            case 4: viewBooks(); break;
            case 5:
                cout << "Logged out.\n";
                return;
            default: cout << "Invalid choice.\n";
        }
    }
}

void userMenu() {
    int choice;
    while (true) {
        cout << "\n--- User Menu ---\n";
        cout << "1. Issue Book\n2. View Books\n3. Logout\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: issueBook(); break;
            case 2: viewBooks(); break;
            case 3:
                cout << "Logged out.\n";
                return;
            default: cout << "Invalid choice.\n";
        }
    }
}

int main() {
    // Load saved data
    loadBooks();
    loadLibrarians();

    while (true) {
        int choice;
        cout << "\n--- Library System ---\n";
        cout << "1. Register\n2. Login\n3. Exit\nChoice: ";
        cin >> choice;

        if (choice == 1) {
            registerLibrarian();
        } else if (choice == 2) {
            if (login()) {
                if (loggedInRole == "admin") {
                    adminMenu();
                } else {
                    userMenu();
                }
                loggedInUsername = "";
                loggedInRole = "";
            }
        } else if (choice == 3) {
            cout << "Exiting the system. Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
