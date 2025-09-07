#include "User.h"
#include "SessionBooking.h"
#include "Payment.h"
#include "Ticketing.h"
#include "FeedbackX.h"

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
    if (!fout.is_open()) {
        cerr << "Error: Could not open user_data.txt for writing." << endl;
        return;
    }

    for (int i = 0; i < users.size(); i++) {
        fout << users[i].userID << "," << users[i].name << "," << users[i].phone << ","
            << users[i].email << "," << users[i].password << ","
            << users[i].isBlocked << endl;
    }
    fout.close();
}

// Loads all user data from a file into the 'users' list.
vector<User> loadUsersFromFile() {
    vector<User> users;
    ifstream fin("user_data.txt");
    if (!fin.is_open()) return users;

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

        if (blockedStr == "1") {
            tempUser.isBlocked = true;
        }
        else {
            tempUser.isBlocked = false;
        }

        users.push_back(tempUser);
    }
    return users;
}

int loadLastUserID() {
    ifstream file("lastUserID.txt");
    int lastID = 0;
    if (file.is_open()) {
        file >> lastID;
    }
    return lastID;
}

void saveLastUserID(int lastID) {
    ofstream file("lastUserID.txt");
    if (file.is_open()) {
        file << lastID;
    }
}

// Generates a new, unique User ID.
string generateUserID() {
    int lastID = loadLastUserID();
    if (lastID < 0) {
        lastID = 0;
    }

    int newID = lastID + 1;
    saveLastUserID(newID);

    stringstream ss;
    ss << "U2025" << setfill('0') << setw(3) << newID;
    return ss.str();
}

// Gets password from user while displaying '*' instead of the actual characters.
string getMaskedPassword() {
    string password = "";
    char ch;
    ch = _getch();
    while (ch != 13) { // Enter key
        if (ch == 8) { // Backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b";
            }
        }
        else {
            password.push_back(ch);
            cout << '*';
        }
        ch = _getch();
    }
    cout << endl;
    return password;
}

// Finds a user by their ID and returns their position (index) in the list.
int findUserIndex(const vector<User>& users, const string& id) {
    for (int i = 0; i < users.size(); i++) {
        if (users[i].userID == id) {
            return i;
        }
    }
    return -1;
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

    // 1. Name
    do {
        cout << "Name (must contain letters and numbers): ";
        getline(cin >> ws, newUser.name);
        if (!isValidName(newUser.name)) {
            cout << "Invalid name format. Return to main menu? (y/n): ";
            cin >> backChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidName(newUser.name));

    // 2. Phone
    do {
        cout << "Phone (starts with 01, 10-11 digits): ";
        getline(cin, newUser.phone);
        if (!isValidPhone(newUser.phone)) {
            cout << "Invalid phone format. Return to main menu? (y/n): ";
            cin >> backChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidPhone(newUser.phone));

    // 3. Email
    do {
        cout << "Email: ";
        getline(cin, newUser.email);
        if (!isValidEmail(newUser.email)) {
            cout << "Invalid email format. Return to main menu? (y/n): ";
            cin >> backChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidEmail(newUser.email));

    // 4. Password
    string confirmPassword;
    do {
        cout << "Password (min 8 chars, include upper, lower, digit, symbol): ";
        getline(cin, newUser.password);
        cout << "Confirm Password: ";
        getline(cin, confirmPassword);
        if (!isValidPassword(newUser.password) || newUser.password != confirmPassword) {
            cout << "Invalid or mismatched password. Return to main menu? (y/n): ";
            cin >> backChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (backChoice == 'y' || backChoice == 'Y') return;
        }
    } while (!isValidPassword(newUser.password) || newUser.password != confirmPassword);

    newUser.userID = generateUserID();
    users.push_back(newUser);
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

        if (choice == 1) {
            string newName;
            do {
                cout << "Enter new name (must contain letters and numbers, or '0' to cancel): ";
                getline(cin, newName);
                if (newName == "0") break;
                if (!isValidName(newName)) {
                    cout << "Invalid name format. Please try again.\n";
                }
            } while (!isValidName(newName) && newName != "0");
            if (newName != "0" && isValidName(newName)) {
                users[userIndex].name = newName;
                infoUpdated = true;
            }
        }
        else if (choice == 2) {
            string newPhone;
            do {
                cout << "Enter new phone (or '0' to cancel): ";
                getline(cin, newPhone);
                if (newPhone == "0") break;
                if (!isValidPhone(newPhone)) {
                    cout << "Invalid phone format. Please try again.\n";
                }
            } while (!isValidPhone(newPhone) && newPhone != "0");
            if (newPhone != "0" && isValidPhone(newPhone)) {
                users[userIndex].phone = newPhone;
                infoUpdated = true;
            }
        }
        else if (choice == 3) {
            string newEmail;
            do {
                cout << "Enter new email (or '0' to cancel): ";
                getline(cin, newEmail);
                if (newEmail == "0") break;
                if (!isValidEmail(newEmail)) {
                    cout << "Invalid email format. Please try again.\n";
                }
            } while (!isValidEmail(newEmail) && newEmail != "0");
            if (newEmail != "0" && isValidEmail(newEmail)) {
                users[userIndex].email = newEmail;
                infoUpdated = true;
            }
        }
        else if (choice == 4) {
            string pw1, pw2;
            do {
                cout << "Enter new password (or '0' to cancel): ";
                getline(cin, pw1);
                if (pw1 == "0") break;
                cout << "Confirm new password: ";
                getline(cin, pw2);
                if (pw1 != pw2) cout << "Passwords do not match.\n";
                else if (!isValidPassword(pw1)) cout << "Password is not strong enough.\n";
            } while ((pw1 != pw2 || !isValidPassword(pw1)) && pw1 != "0");
            if (pw1 != "0" && pw1 == pw2 && isValidPassword(pw1)) {
                users[userIndex].password = pw1;
                infoUpdated = true;
                cout << "Password updated.\n";
            }
        }
        else if (choice != 5) {
            cout << "Invalid option. Please select a number between 1 and 5.\n";
        }

        if (infoUpdated) {
            saveUsersToFile(users);
            cout << "Information updated successfully!\n";
        }
    } while (choice != 5);
}

void userMenu(int userIndex, vector<User>& users, vector<Session>& sessions, vector<Merchandise>& merchandise) {
    while (true) {
        cout << "\n--- User Menu ---\n";
        cout << "Welcome, " << users[userIndex].name << "!\n";
        cout << "1. View Profile\n2. Edit My Info\n3. Purchase Concert Ticket\n4. Refund Ticket\n5. View Order History\n6. Leave Feedback\n7. Logout\nChoice: ";

        int choice;
        cin >> choice;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 7.\n";
            continue;
        }
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
            editUserInfo(userIndex, users);
            break;
        case 3:
            displayTicketPurchaseMenu(userIndex, users, sessions, merchandise);
            break;
        case 4:
            processTicketRefund(users[userIndex].userID, sessions);
            break;
        case 5:
            displayOrderHistory(users[userIndex].userID);
            break;
        case 6: {
            FeedbackX fb;
            fb.userID = users[userIndex].userID;
            fb.userName = users[userIndex].name;
            do {
                cout << "\nEnter your feedback: ";
                getline(cin, fb.feedback);
                if (fb.feedback.empty()) {
                    cout << "Feedback cannot be empty. Please try again.\n";
                }
            } while (fb.feedback.empty());
            saveFeedbackX(fb);
            cout << "Thank you! Your feedback has been recorded.\n";
        }
              break;
        case 7: {
            cout << "Logging out...\n";
            char fbChoice;
            while (true) {
                cout << "Do you want to leave feedback? (y/n): ";
                cin >> fbChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                if (fbChoice == 'y' || fbChoice == 'Y') {
                    FeedbackX fb;
                    fb.userID = users[userIndex].userID;
                    fb.userName = users[userIndex].name;
                    do {
                        cout << "Enter your feedback: ";
                        getline(cin, fb.feedback);
                        if (fb.feedback.empty()) {
                            cout << "Feedback cannot be empty. Please try again.\n";
                        }
                    } while (fb.feedback.empty());
                    saveFeedbackX(fb);
                    cout << "Thank you! Your feedback has been recorded.\n";
                    break;
                }
                else if (fbChoice == 'n' || fbChoice == 'N') {
                    cout << "Okay, no feedback submitted.\n";
                    break;
                }
                else {
                    cout << "Invalid choice. Please enter 'y' or 'n'.\n";
                }
            }
            return;
        }
        default:
            cout << "Invalid option. Please enter a number between 1-7.\n";
        }
    }
}
