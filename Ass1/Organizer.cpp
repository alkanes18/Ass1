#include "Organizer.h"
#include "User.h"
#include "Report.h"

#include <iostream>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

bool confirmOrganizerPassword(const SystemCredentials& creds) {
    string pw;
    cout << "\nEnter organizer password to continue: ";
    pw = getMaskedPassword();
    return pw == creds.ORGANIZERPW;
}

void organizerMenu(vector<User>& users, const SystemCredentials& creds, vector<Session>& session, vector<Merchandise>& merchandise) {
    int sessionMenuOption1 = 0;
    bool runningSessionMenu1 = true;

    while (true) {
        cout << "\n--- Organizer Menu ---\n";
        cout << "1. View All Users\n2. Manage User Status\n3. Add User\n4. Delete User\n5. Session Menu\n6. Back\nChoice: ";

        int choice;
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 6.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice >= 2 && choice <= 5) {
            if (!confirmOrganizerPassword(creds)) {
                cout << "Incorrect password. Returning to organizer menu.\n";
                continue;
            }
        }

        switch (choice) {
        case 1:
            viewAllUsers(users, creds);
            break;
        case 2: { // Manage User Status
            cout << "Enter User ID to manage status: ";
            string id;
            getline(cin, id);
            int userIndex = findUserIndex(users, id);
            if (userIndex != -1) {
                cout << "\nUser: " << users[userIndex].name << " | Current status: " << (users[userIndex].isBlocked ? "Blocked" : "Active") << endl;
                cout << "1. Block user\n2. Unblock user\n3. Cancel\nChoice: ";
                int statusChoice;
                cin >> statusChoice;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter 1, 2, or 3.\n";
                    break;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (statusChoice == 1) {
                    users[userIndex].isBlocked = true;
                    cout << "User has been blocked.\n";
                    saveUsersToFile(users);
                }
                else if (statusChoice == 2) {
                    users[userIndex].isBlocked = false;
                    cout << "User has been unblocked.\n";
                    saveUsersToFile(users);
                }
                else if (statusChoice == 3) {
                    cout << "Operation cancelled.\n";
                }
                else {
                    cout << "Invalid option. Please enter 1, 2, or 3.\n";
                }
            }
            else {
                cout << "User not found.\n";
            }
            break;
        }
        case 3:
            registerUser(users);
            break;
        case 4: { // Delete User
            cout << "Enter User ID to delete: ";
            string id_to_delete;
            getline(cin, id_to_delete);

            int index_to_delete = findUserIndex(users, id_to_delete);

            if (index_to_delete != -1) {
                users.erase(users.begin() + index_to_delete);
                saveUsersToFile(users);
                cout << "User deleted.\n";
            }
            else {
                cout << "User not found.\n";
            }
            break;
        }
        case 5:
            while (runningSessionMenu1) {
                displaySessionMenu1();
                cin >> sessionMenuOption1;

                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number between 1 and 6.\n";
                    continue;
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                switch (sessionMenuOption1) {
                case 1:
                    displayAllSessions(session);
                    runSessionMenu2(session);
                    break;
                case 2:
                    addSession(session);
                    saveSeatsToFile(session);
                    break;
                case 3:
                    deleteSession(session);
                    break;
                case 4:
                    while (true) {
                        char confirm;
                        cout << "Are you sure you want to reset all seats? (y/n): ";
                        cin >> confirm;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (confirm == 'y' || confirm == 'Y') {
                            resetSeats(session, "seats.txt");
                            break;
                        }
                        else if (confirm == 'n' || confirm == 'N') {
                            cout << "Reset cancelled.\n";
                            break;
                        }
                        else {
                            cout << "Invalid input. Please enter 'y' or 'n'.\n";
                        }
                    }
                    break;
                case 5:
                    runReportMenu(session, merchandise);
                    break;
                case 6:
                    cout << "Exiting...\n";
                    runningSessionMenu1 = false;
                    break;
                default:
                    cout << "Invalid option! Please type 1 - 6 only." << endl;
                    continue;
                }
            }
            break;
        case 6:
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice. Please enter 1 - 6.\n";
            break;
        }
    }
}

void viewAllUsers(vector<User>& users, const SystemCredentials& creds) {
    if (users.empty()) {
        cout << "No registered users found.\n";
        return;
    }

    printUserList(users, creds);

    while (true) {
        char sortChoice;
        cout << "\nDo you want to sort this list? (y/n): ";
        cin >> sortChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (sortChoice == 'y' || sortChoice == 'Y') {
            cout << "\n--- Sort Users ---\n";
            cout << "1. Sort by User ID (Ascending)\n";
            cout << "2. Sort by Name (Alphabetical)\n";
            cout << "3. Back\nChoice: ";

            int choice;
            cin >> choice;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                cout << "Invalid input. Please enter 1, 2, or 3.\n";
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                sort(users.begin(), users.end(), compareByID);
                cout << "Users sorted by User ID.\n";
            }
            else if (choice == 2) {
                sort(users.begin(), users.end(), compareByName);
                cout << "Users sorted by Name.\n";
            }
            else if (choice == 3) {
                cout << "Returning...\n";
                break;
            }
            else {
                cout << "Invalid option. Please enter 1, 2, or 3.\n";
                continue;
            }

            if (choice == 1 || choice == 2) {
                saveUsersToFile(users);
                cout << "\n--- Sorted User List ---\n";
                printUserList(users, creds);
            }
            break;
        }
        else if (sortChoice == 'n' || sortChoice == 'N') {
            break;
        }
        else {
            cout << "Invalid input. Please enter 'y' or 'n'.\n";
        }
    }
}

void printUserList(const vector<User>& users, const SystemCredentials& creds) {
    cout << "\n" << left
        << setw(10) << "User ID"
        << setw(20) << "Name"
        << setw(14) << "Phone"
        << setw(30) << "Email"
        << setw(20) << "Password"
        << setw(9) << "Status" << endl;
    cout << string(115, '-') << endl;

    bool foundUsers = false;
    for (int i = 0; i < users.size(); i++) {
        if (users[i].userID == creds.ORGANIZERID) {
            continue;
        }
        foundUsers = true;
        cout << left
            << setw(10) << users[i].userID
            << setw(20) << users[i].name
            << setw(14) << users[i].phone
            << setw(30) << users[i].email
            << setw(20) << users[i].password
            << setw(9) << (users[i].isBlocked ? "Blocked" : "Active") << endl;
    }
    if (!foundUsers) {
        cout << "No users found.\n";
    }
    cout << string(115, '-') << endl;
}
