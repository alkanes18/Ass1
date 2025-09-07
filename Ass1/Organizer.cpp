#include "Organizer.h"
#include "User.h"
#include "Report.h"
#include "FeedbackX.h"

#include <iostream>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

bool confirmOrganizerPassword(const SystemCredentials& creds) {   // Verify organizer password
    string pw;
    cout << "\nEnter organizer password to continue: ";
    pw = getMaskedPassword();                                     // Masked input with '*'
    return pw == creds.organizerPW;                               // Compare with stored password
}

void organizerMenu(vector<User>& users, const SystemCredentials& creds, vector<Session>& session, vector<Merchandise>& merchandise) {
    // Main organizer menu controlling all admin-level functions
    int sessionMenuOption1 = 0;

    while (true) {
        cout << "\n--- Organizer Menu ---\n";
        cout << "1. View All Users\n2. Manage User Status\n3. Add User\n4. Delete User\n5. Session Menu\n6. View Feedback\n7. Back\nChoice: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (cin.fail()) {                                         // Handle invalid input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number between 1 and 7.\n";
            continue;
        }

        if (choice >= 2 && choice <= 5 && !confirmOrganizerPassword(creds)) {
            // Require organizer password again for sensitive options
            cout << "Incorrect password. Returning to organizer menu.\n";
            continue;
        }

        switch (choice) {
        case 1:                                                   // View all users
            viewAllUsers(users, creds);
            break;
        case 2: {                                                 // Block/unblock user
            cout << "Enter User ID to manage status: ";
            string id;
            getline(cin, id);
            int userIndex = findUserIndex(users, id);
            if (userIndex != -1) {
                cout << "\nUser: " << users[userIndex].name
                    << " | Current status: "
                    << (users[userIndex].isBlocked ? "Blocked" : "Active") << endl;
                cout << "1. Block user\n2. Unblock user\n3. Cancel\nChoice: ";
                int statusChoice;
                cin >> statusChoice;
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

                if (statusChoice == 1) users[userIndex].isBlocked = true;
                else if (statusChoice == 2) users[userIndex].isBlocked = false;
                else if (statusChoice == 3) {
                    cout << "Operation cancelled.\n";
                    break;
                }
                else {
                    cout << "Invalid option. Please enter 1, 2, or 3.\n";
                    break;
                }

                saveUsersToFile(users);                           // Save updated status
                cout << "User status updated.\n";
            }
            else {
                cout << "User not found.\n";
            }
            break;
        }
        case 3:                                                   // Add/register user
            registerUser(users);
            break;
        case 4: {                                                 // Delete user
            cout << "Enter User ID to delete: ";
            string id_to_delete;
            getline(cin, id_to_delete);

            int index_to_delete = findUserIndex(users, id_to_delete);

            if (index_to_delete != -1) {
                users.erase(users.begin() + index_to_delete);     // Remove from vector
                saveUsersToFile(users);
                cout << "User deleted.\n";
            }
            else {
                cout << "User not found.\n";
            }
            break;
        }
        case 5: {                                                 // Session management submenu
            bool runningSessionMenu1 = true;
            while (runningSessionMenu1) {
                displaySessionMenu1();                            // Show session submenu
                cin >> sessionMenuOption1;
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number.\n";
                    continue;
                }

                switch (sessionMenuOption1) {
                case 1:                                           // View and manage sessions
                    displayAllSessions(session);
                    runSessionMenu2(session);
                    break;
                case 2:                                           // Add new session
                    addSession(session);
                    saveSeatsToFile(session);
                    break;
                case 3:                                           // Delete session
                    deleteSession(session);
                    break;
                case 4:                                           // Edit session
                    editCurrentSession(session);
                    break;
                case 5: {                                         // Reset seats confirmation
                    while (true) {
                        char confirm;
                        cout << "Are you sure you want to reset all seats? (y/n): ";
                        cin >> confirm;
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        if (confirm == 'y' || confirm == 'Y') {
                            resetSeats(session);
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
                }
                case 6:                                           // Run reports
                    runReportMenu(session, merchandise);
                    break;
                case 7:                                           // Exit session menu
                    cout << "Exiting Session Menu...\n";
                    runningSessionMenu1 = false;
                    break;
                default:
                    cout << "Invalid option! Please type 1 - 7 only." << endl;
                    continue;
                }
            }
            break;
        }
        case 6:                                                   // View all feedback
            viewAllFeedbackX();
            break;
        case 7:                                                   // Back to main menu
            cout << "Returning to main menu...\n";
            return;
        default:
            cout << "Invalid choice. Please enter 1-7.\n";
            break;
        }
    }
}

void viewAllUsers(vector<User>& users, const SystemCredentials& creds) {
    // Display all users in a formatted table, with option to sort

    if (users.empty()) {
        cout << "No registered users found.\n";
        return;
    }

    printUserList(users, creds);                                 // Print user list

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
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1) {
                sort(users.begin(), users.end(), compareByID);   // Sort by ID
                cout << "Users sorted by User ID.\n";
            }
            else if (choice == 2) {
                sort(users.begin(), users.end(), compareByName); // Sort by Name
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

            saveUsersToFile(users);                              // Save sorted order
            cout << "\n--- Sorted User List ---\n";
            printUserList(users, creds);
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
    // Nicely print all users with columns

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
        if (users[i].userID == creds.organizerID) {              // Skip organizer account itself
            continue;
        }
        foundUsers = true;
        cout << left
            << setw(10) << users[i].userID
            << setw(20) << users[i].name
            << setw(14) << users[i].phone
            << setw(30) << users[i].email
            << setw(20) << "********"                            // Masked password
            << setw(9) << (users[i].isBlocked ? "Blocked" : "Active")
            << endl;
    }
    if (!foundUsers) {
        cout << "No users found.\n";
    }
    cout << string(115, '-') << endl;
}
