#include <iostream>
#include "User.h"
#include "Organizer.h"
#include "Ticketing.h"
#include "Payment.h"
#include "SessionBooking.h"
#include "Report.h"

using namespace std;

int main() {
    vector<User> users = loadUsersFromFile();

    vector<Session> session = { {"", "Kuala Lumpur, Malaysia", "17/9/2025 (7.30pm - 10.30pm)"}, {"", "Kuala Lumpur, Malaysia", "18/9/2025 (7.30pm - 10.30pm)"},
                                {"", "Singapore, Singapore", "21/9/2025 (8.00pm - 10.30pm)"}, {"", "Pulau Penang, Malaysia", "1/10/2025 (2.00pm - 4.30pm)"} };
    initializeAllSeats(session);
    initializeSessionID(session);
    initializePrices(session); // Initialize prices for sessions

    vector<Merchandise> merchandise;
    initializeMerchandise(merchandise); // Initialize merchandise

    SystemCredentials creds;
    int choice;

    while (true) {
        cout << "\n=== Spatula & Guitar Concert System ===\n";
        cout << "1. Register\n2. Login as User\n3. Login as Organizer\n0. Exit\nChoice: ";

        cin >> choice;

        // Check if the user's input is a valid number.
        if (cin.fail()) {
            cin.clear(); // Clear the error flag.
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the rest of the bad input.
            cout << "Invalid option. Please enter a number between 0 and 3.\n";
            continue; // Skip the rest of this loop and start over.
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Ignore the newline character after the number.

        switch (choice) {
        case 1:
            registerUser(users);
            break;
        case 2: { // User Login
            string id, pw;
            cout << "\n--- User Login ---\n";
            cout << "User ID: ";
            getline(cin, id);
            cout << "Password: ";

            // Call the function to get the password with '*' masking.
            pw = getMaskedPassword();

            // Call the function to find the user's position (index) in the list.
            int userIndex = findUserIndex(users, id);

            // If the user was found (index is not -1) and the password is correct...
            if (userIndex != -1 && users[userIndex].password == pw) {
                // Check if the account is blocked.
                if (users[userIndex].isBlocked) {
                    cout << "Account is blocked.\n";
                }
                else {
                    // Login successful, go to the user menu.
                    cout << "Login successful.\n";
                    userMenu(userIndex, users, session, merchandise);
                }
            }
            else {
                cout << "Login failed.\n";
            }
            break;
        }
        case 3: { // Organizer Login
            string id, pw;
            cout << "\n--- Organizer Login ---\n";
            cout << "Organizer ID: ";
            getline(cin, id);
            cout << "Password: ";
            pw = getMaskedPassword();

            if (id == creds.organizerID && pw == creds.organizerPW) {
                cout << "Organizer login successfully.\n";
                organizerMenu(users, creds, session, merchandise);
            }
            else {
                cout << "Organizer login failed.\n";
            }
            break;
        }
        case 0:
            cout << "Thank you for using the system. Goodbye.\n";
            return 0; // Exit the program.
        default:
            cout << "Invalid option. Please enter a number between 0 and 3.\n";
            break;
        }
    }

    return 0;
}