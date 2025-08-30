#include "User.h"
#include "Payment.h"
#include "Ticketing.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <conio.h>
#include <algorithm>
#include <limits>

using namespace std;

// --- Validation Function Implementations ---

bool isValidName(const string& name) {
    bool hasLetter = false, hasDigit = false;
    if (name.empty()) return false;
    for (int i = 0; i < name.length(); i++) {
        if (isalpha(name[i])) hasLetter = true;
        if (isdigit(name[i])) hasDigit = true;
    }
    return hasLetter && hasDigit;
}

bool isValidPhone(const string& phone) {
    return regex_match(phone, regex("^01[0-9]{8,9}$"));
}

bool isValidEmail(const string& email) {
    return regex_match(email, regex(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"));
}

bool isValidPassword(const string& pw) {
    if (pw.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSymbol = false;
    for (int i = 0; i < pw.length(); i++) {
        if (isupper(pw[i])) hasUpper = true;
        else if (islower(pw[i])) hasLower = true;
        else if (isdigit(pw[i])) hasDigit = true;
        else if (ispunct(pw[i])) hasSymbol = true;
    }
    return hasUpper && hasLower && hasDigit && hasSymbol;
}


// Saves the entire 'users' list to a file.
void saveUsersToFile(const vector<User>& users) {
    ofstream fout("user_data.txt");
    for (int i = 0; i < users.size(); i++) {
        fout << users[i].userID << "," << users[i].name << "," << users[i].phone << ","
            << users[i].email << "," << users[i].password << ","
            << users[i].isBlocked << "," << users[i].bookingInfo << endl;
    }
    fout.close(); // Close the file after writing.
}

// Loads all user data from a file into the 'users' list.
vector<User> loadUsersFromFile() {
    vector<User> users;
    ifstream fin("user_data.txt");
    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        User tempUser;
        string blockedStr;

        getline(ss, tempUser.userID, ',');
        getline(ss, tempUser.name, ',');
        getline(ss, tempUser.phone, ',');
        getline(ss, tempUser.email, ',');
        getline(ss, tempUser.password, ',');
        getline(ss, blockedStr, ',');
        getline(ss, tempUser.bookingInfo);

        // This is a clearer if-else version of the boolean assignment.
        if (blockedStr == "1") {
            tempUser.isBlocked = true; // If the file stores "1", the account is blocked.
        }
        else {
            tempUser.isBlocked = false; // Otherwise, it's active.
        }

        users.push_back(tempUser);
    }
    return users;
}

// Generates a new, unique User ID.
string generateUserID(int count) {
    stringstream ss;
    ss << "U2025" << setfill('0') << setw(3) << count + 1;
    return ss.str();
}

// Gets password from user while displaying '*' instead of the actual characters.
string getMaskedPassword() {
    string password = "";
    char ch;
    // _getch() reads a character directly from the keyboard without showing it.
    ch = _getch();
    while (ch != 13) { // 13 is the ASCII code for the Enter key.
        if (ch == 8) { // 8 is the ASCII code for the Backspace key.
            if (!password.empty()) {
                password.pop_back(); // Remove the last character from the string.
                cout << "\b \b";   // Move the cursor back, print a space, move back again.
            }
        }
        else {
            password.push_back(ch); // Add the character to the end of the password string.
            cout << '*';            // Display a star on the screen.
        }
        ch = _getch(); // Get the next character.
    }
    cout << endl; // After the user presses Enter, move to the next line.
    return password; // Return the complete password string.
}

// Finds a user by their ID and returns their position (index) in the list.
int findUserIndex(const vector<User>& users, const string& id) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i].userID == id) {
            return i; // Found it, return the index 'i'.
        }
    }
    return -1; // If the loop finishes and nothing was found, return -1.
}


// --- Comparison Functions for Sorting ---

bool compareByID(const User& a, const User& b) {
    return a.userID < b.userID;
}

bool compareByName(const User& a, const User& b) {
    return a.name < b.name;
}


// --- Core Feature Function Implementations ---

void registerUser(vector<User>& users) {
    User newUser;
    char backChoice;

    cout << "\n--- User Registration ---\n";

    // 1. Get and validate username
    do {
        cout << "Name (must contain letters and numbers): ";
        getline(cin >> ws, newUser.name);
        if (!isValidName(newUser.name)) {
            cout << "Invalid name format. Return to main menu? (y/n): ";
            cin >> backChoice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidName(newUser.name));

    // 2. Get and validate phone
    do {
        cout << "Phone (starts with 01, 10-11 digits): ";
        getline(cin, newUser.phone);
        if (!isValidPhone(newUser.phone)) {
            cout << "Invalid phone format. Return to main menu? (y/n): ";
            cin >> backChoice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidPhone(newUser.phone));

    // 3. Get and validate email
    do {
        cout << "Email: ";
        getline(cin, newUser.email);
        if (!isValidEmail(newUser.email)) {
            cout << "Invalid email format. Return to main menu? (y/n): ";
            cin >> backChoice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidEmail(newUser.email));

    // 4. Get and validate password
    string confirmPassword;
    do {
        cout << "Password (min 8 chars, include upper, lower, digit, symbol): ";
        getline(cin, newUser.password);
        cout << "Confirm Password: ";
        getline(cin, confirmPassword);
        if (!isValidPassword(newUser.password) || newUser.password != confirmPassword) {
            cout << "Invalid or mismatched password. Return to main menu? (y/n): ";
            cin >> backChoice; cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidPassword(newUser.password) || newUser.password != confirmPassword);

    newUser.userID = generateUserID(static_cast<int>(users.size()));
    users.push_back(newUser);
    // Call the function to save the updated 'users' list to the file.
    saveUsersToFile(users);
    cout << "\nRegistered. Your User ID is: " << newUser.userID << endl;
}

void editUserInfo(int userIndex, vector<User>& users) {
    int choice;
    do {
        cout << "\n--- Edit My Info ---\n";
        cout << "1. Change Name\n";
        cout << "2. Change Phone\n";
        cout << "3. Change Email\n";
        cout << "4. Change Password\n";
        cout << "5. Back\n";
        cout << "Choice: ";

        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        bool infoUpdated = false;

        if (choice == 1) { // Change Name
            string newName;
            do {
                cout << "Enter new name (must contain letters and numbers, or '0' to cancel): ";
                getline(cin, newName);
                if (newName == "0") break; // Allow user to cancel
                if (!isValidName(newName)) {
                    cout << "Invalid name format. Please try again.\n";
                }
            } while (!isValidName(newName));

            if (newName != "0") {
                users[userIndex].name = newName;
                infoUpdated = true;
            }
        }
        else if (choice == 2) { // Change Phone
            string newPhone;
            do {
                cout << "Enter new phone (or '0' to cancel): ";
                getline(cin, newPhone);
                if (newPhone == "0") break;
                if (!isValidPhone(newPhone)) {
                    cout << "Invalid phone format. Please try again.\n";
                }
            } while (!isValidPhone(newPhone));

            if (newPhone != "0") {
                users[userIndex].phone = newPhone;
                infoUpdated = true;
            }
        }
        else if (choice == 3) { // Change Email
            string newEmail;
            do {
                cout << "Enter new email (or '0' to cancel): ";
                getline(cin, newEmail);
                if (newEmail == "0") break;
                if (!isValidEmail(newEmail)) {
                    cout << "Invalid email format. Please try again.\n";
                }
            } while (!isValidEmail(newEmail));

            if (newEmail != "0") {
                users[userIndex].email = newEmail;
                infoUpdated = true;
            }
        }
        else if (choice == 4) { // Change Password
            string pw1, pw2;
            do {
                cout << "Enter new password (or '0' to cancel): ";
                getline(cin, pw1);
                if (pw1 == "0") break;

                cout << "Confirm new password: ";
                getline(cin, pw2);

                if (pw1 != pw2) cout << "Passwords do not match.\n";
                else if (!isValidPassword(pw1)) cout << "Password is not strong enough.\n";

            } while (pw1 != pw2 || !isValidPassword(pw1));

            if (pw1 != "0") {
                users[userIndex].password = pw1;
                infoUpdated = true;
                cout << "Password updated.\n";
            }
        }

        if (infoUpdated) {
            saveUsersToFile(users);
            cout << "Information updated successfully!\n";
        }

    } while (choice != 5);
}

void userMenu(int userIndex, vector<User>& users, vector<Session>& sessions, vector<Merchandise> merchandise) {
    while (true) {
        cout << "\n--- User Menu ---\n";
        cout << "Welcome, " << users[userIndex].name << "!\n";
        cout << "1. View Profile\n2. Edit My Info\n3. Purchase Concert Ticket\n4. View Order History\n5. Logout\nChoice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            cout << "\n--- Your Profile ---\n";
            cout << "User ID  : " << users[userIndex].userID << "\n";
            cout << "Name     : " << users[userIndex].name << "\n";
            cout << "Phone    : " << users[userIndex].phone << "\n";
            cout << "Email    : " << users[userIndex].email << "\n";
            break;
        case 2:
            editUserInfo(userIndex, users); // Call the newly implemented function
            break;
        case 3:
            displayTicketPurchaseMenu(userIndex, users, sessions, merchandise); // Call the new ticket purchase menu
            break;
        case 4:
            displayOrderHistory(users[userIndex].userID);
            break;
        case 5:
            cout << "Logging out...\n";
            return;
        default:
            cout << "Invalid option.\n";
        }
    }
}