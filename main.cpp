#include <iostream>
#include <limits>
#include "User.h"
#include "Organizer.h"
#include "Ticketing.h"
#include "Payment.h"
#include "SessionBooking.h"
#include "Report.h"
#include "FeedbackX.h"   // <-- Added Feedback support

using namespace std;

int main() {
    vector<User> users = loadUsersFromFile();

    vector<Session> session = loadSessionsFromFile();
    if (session.empty()) {
        session = {
            {"", "Kuala Lumpur, Malaysia", "17/9/2025 (7.30pm - 10.30pm)", 400.00, 150.00},
            {"", "Kuala Lumpur, Malaysia", "18/9/2025 (7.30pm - 10.30pm)", 400.00, 150.00},
            {"", "Singapore, Singapore", "21/9/2025 (8.00pm - 10.30pm)", 500.00, 200.00},
            {"", "Pulau Penang, Malaysia", "1/10/2025 (2.00pm - 4.30pm)", 420.00, 170.00}
        };
        initializeAllSeats(session);
        initializeSessionID(session);
        loadSeatsFromFile(session);
    }
    else {
        initializeAllSeats(session);
        loadSeatsFromFile(session);
    }

    vector<Merchandise> merchandise;
    initializeMerchandise(merchandise);

    SystemCredentials creds;
    int choice;

    while (true) {
        cout << "\n=== Spatula & Guitar Concert System ===\n";
        cout << "1. Register\n2. Login as User\n3. Login as Organizer\n0. Exit\nChoice: ";

        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid option. Please enter a number between 0 and 3.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
        case 1:
            registerUser(users);
            break;

        case 2: {
            string id, pw;
            cout << "\n--- User Login ---\n";
            cout << "User ID: ";
            getline(cin, id);
            cout << "Password: ";
            pw = getMaskedPassword();

            int userIndex = findUserIndex(users, id);

            if (userIndex != -1 && users[userIndex].password == pw) {
                if (users[userIndex].isBlocked) {
                    cout << "Account is blocked.\n";
                }
                else {
                    cout << "Login successful.\n";
                    userMenu(userIndex, users, session, merchandise);

                    // After returning from user menu, ask if they want to leave feedback
                    char fbChoice;
                    cout << "\nDo you want to leave feedback? (y/n): ";
                    cin >> fbChoice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    if (fbChoice == 'y' || fbChoice == 'Y') {
                        FeedbackX fb;
                        fb.feedbackID = generateFeedbackID();
                        fb.userID = users[userIndex].userID;
                        fb.userName = users[userIndex].name;
                        cout << "Enter your feedback: ";
                        getline(cin, fb.feedback);
                        fb.timestamp = getCurrentTimestamp();

                        saveFeedbackX(fb);
                        cout << "Thank you! Your feedback has been saved.\n";
                    }
                }
            }
            else {
                cout << "Login failed.\n";
            }
            break;
        }

        case 3: {
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
            saveUsersToFile(users);
            saveSessionsToFile(session);
            saveSeatsToFile(session);
            cout << "Thank you for using the system. Goodbye.\n";
            return 0;

        default:
            cout << "Invalid option. Please enter a number between 0 and 3.\n";
            break;
        }
    }

    return 0;
}
