#include "SessionBooking.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

void displaySessionMenu1() {
    cout << "\nSession Menu (ORGANIZER ONLY)" << endl;
    cout << "+----------------------------+" << endl;
    cout << "| 1. Display All Sessions    |" << endl;
    cout << "| 2. Add New Session         |" << endl;
    cout << "| 3. Delete Session          |" << endl;
    cout << "| 4. View Report             |" << endl;
    cout << "| 5. Write Report to File    |" << endl;
    cout << "| 6. Exit Session Menu       |" << endl;
    cout << "+----------------------------+" << endl;
    cout << "Your choice > ";
}

void displaySessionMenu2() {
    cout << "\n+----------------------------+" << endl;
    cout << "| 1. Display All Seats       |" << endl;
    cout << "| 2. Display Specific Seats  |" << endl;
    cout << "| 3. Back to Menu            |" << endl;
    cout << "+----------------------------+" << endl;
    cout << "Your choice > ";
}

void initializeSeats(Session& session) {
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 15; j++) {
            session.vipSeats[i][j] = 'O';
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 15; j++) {
            session.standardSeats[i][j] = 'O';
        }
    }
}

void initializeAllSeats(vector<Session>& session) {
    for (int a = 0; a < session.size(); a++) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 15; j++) {
                session[a].vipSeats[i][j] = 'O';
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 15; j++) {
                session[a].standardSeats[i][j] = 'O';
            }
        }
    }
}

void initializeSessionID(vector<Session>& session) {
    for (int i = 0; i < session.size(); i++) {
        stringstream ss;
        ss << "S" << setw(3) << setfill('0') << i + 1;
        session[i].sessionID = ss.str();
    }
}

string generateSessionID(vector<Session>& session) {
    size_t nextID = session.size() + 1;
    stringstream ss;
    ss << "S" << setw(3) << setfill('0') << nextID;
    return ss.str();
}

void addSession(vector<Session>& session) {
    Session newSession;

    newSession.sessionID = generateSessionID(session);

    cout << "\n\n===== Add a Session =====" << endl;
    cin.ignore();
    cout << "\nEnter the location (Eg. Kuala Lumpur, Malaysia): ";
    getline(cin, newSession.location);

    cout << "Enter the date & time (Eg. 17/9/2025 (7.30pm - 10.30pm)): ";
    getline(cin, newSession.time);

    initializeSeats(newSession);

    confirmAdd(session, newSession);
}

void deleteSession(vector<Session>& session) {
    cout << "\n\n===== Delete a Session =====" << endl;
    cout << "There are currently " << session.size() << " session(s) available." << endl << endl;

    displayAllSessions(session);

    cout << "Enter the session number to delete (1 - " << session.size() << "): ";

    int num;
    cin >> num;

    if (num < 1 || num > session.size()) {
        cout << "Invalid session number! Deletion cancelled." << endl;
    }
    else {
        confirmDelete(session, num);
    }
}

void confirmAdd(vector<Session>& session, Session newSession) {
    char choice;

    cout << "\nConfirm to add this session? (Y/N) > ";
    cin >> choice;

    if (toupper(choice) == 'Y') {
        session.push_back(newSession);
        cout << "Session added successfully!" << endl;
    }
    else {
        cout << "Operation cancelled." << endl;
    }
}

void confirmDelete(vector<Session>& session, int num) {
    char choice;

    cout << "\nConfirm to delete this session? (Y/N) > ";
    cin >> choice;

    if (toupper(choice) == 'Y') {
        session.erase(session.begin() + (num - 1));
        cout << "Session deleted successfully!" << endl;
    }
    else {
        cout << "Operation cancelled." << endl;
    }
}

void displaySeats(Session session) {
    char rowLabel = 'A';

    cout << "\nSeats for " << session.location << " - " << session.time << endl;
    cout << "======================================================================" << endl;
    cout << endl << "  ";
    for (int col = 0; col < 15; col++) {
        cout << right << setw(3) << col + 1;
    }
    cout << endl;

    for (int i = 0; i < 2; i++) {
        cout << rowLabel++ << " ";
        for (int j = 0; j < 15; j++) {
            cout << right << setw(3) << session.vipSeats[i][j];
        }
        cout << "\t\tVIP SEATS";
        cout << endl;
    }
    for (int i = 0; i < 8; i++) {
        cout << rowLabel++ << " ";
        for (int j = 0; j < 15; j++) {
            cout << right << setw(3) << session.standardSeats[i][j];
        }
        cout << "\t\tSTANDARD SEATS";
        cout << endl;
    }
}

void displayAllSeats(vector<Session> session) {
    for (int a = 0; a < session.size(); a++) {
        char rowLabel = 'A';

        cout << "\n\nSeats for Session (" << a + 1 << "): " << session[a].location << endl;
        cout << "======================================================================" << endl;

        cout << endl << "  ";
        for (int col = 0; col < 15; col++) {
            cout << right << setw(3) << col + 1;
        }
        cout << endl;

        for (int i = 0; i < 2; i++) {
            cout << rowLabel++ << " ";
            for (int j = 0; j < 15; j++) {
                cout << right << setw(3) << session[a].vipSeats[i][j];
            }
            cout << "\t\tVIP SEATS";
            cout << endl;
        }
        for (int i = 0; i < 8; i++) {
            cout << rowLabel++ << " ";
            for (int j = 0; j < 15; j++) {
                cout << right << setw(3) << session[a].standardSeats[i][j];
            }
            cout << "\t\tSTANDARD SEATS";
            cout << endl;
        }
    }
}

bool isSeatAvailable(Session session, string seatType, int row, int col) {
    if (seatType == "VIP") {
        if (row >= 0 && row < 2 && col >= 0 && col < 15) {
            return session.vipSeats[row][col] == 'O';
        }
    }
    else if (seatType == "Standard") {
        if (row >= 0 && row < 8 && col >= 0 && col < 15) {
            return session.standardSeats[row][col] == 'O';
        }
    }
    return false; // Invalid input
}

bool bookSeat(Session& session, Attendee& user, string seatType, int row, int col) {
    if (!isSeatAvailable(session, seatType, row, col)) {
        return false;
    }

    if (seatType == "VIP") {
        session.vipSeats[row][col] = 'X';
    }
    else if (seatType == "Standard") {
        session.standardSeats[row][col] = 'X';
    }
    return true;
}

void cancelBooking(Attendee& user) {

}

void displayAllSessions(vector<Session> session) {
    Session searchSession;

    cout << endl << left << setw(5) << "No." << setw(14) << "Session ID" << setw(40) << "Location" << setw(20) << "Date (Time)" << endl;
    cout << "==================================================================================================" << endl;
    for (int i = 0; i < session.size(); i++) {
        cout << left << setw(5) << i + 1 << setw(14) << session[i].sessionID << setw(40) << session[i].location << setw(20) << session[i].time << endl;
        cout << "--------------------------------------------------------------------------------------------------" << endl;
    }
}

void runSessionMenu2(vector<Session> session) {
    int sessionMenuOption2 = 0;
    bool runningSessionMenu2 = true;
    int indexSession = -1;

    while (runningSessionMenu2) {
        displaySessionMenu2();
        cin >> sessionMenuOption2;

        switch (sessionMenuOption2) {
        case 1:
            displayAllSeats(session);
            break;
        case 2:
            indexSession = searchForASession(session);
            if (indexSession != -1) {
                cout << endl;
                displaySeats(session[indexSession]);
            }
            else {
                cout << "Session not found!" << endl;
            }
            break;
        case 3:
            runningSessionMenu2 = false;
            break;
        default:
            cout << "Invalid option! Please type 1 - 2 only." << endl;
            continue;
        }
    }
}

int searchForASession(vector<Session> session) {
    Session searchSession;
    string sessionID;

    cout << "\nEnter the session ID: ";
    cin >> sessionID;

    for (int i = 0; i < session.size(); i++) {
        if (sessionID == session[i].sessionID) {
            return i;
        }
    }
    return -1;
}

void countRemainingSeats(Session session) {
    int standardCount = 0, vipCount = 0;

    cout << "Seats for " << session.location << endl;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 15; j++) {
            if (session.vipSeats[i][j] == 'O') {
                vipCount++;
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 15; j++) {
            if (session.standardSeats[i][j] == 'O') {
                standardCount++;
            }
        }
    }
    cout << "\nVIP Seats Available Now: " << vipCount << endl;
    cout << "Standard Seats Available Now: " << standardCount << endl << endl;
}

void countAllRemainingSeats(vector<Session> session) {
    int standardCount = 0, vipCount = 0;

    for (int a = 0; a < session.size(); a++) {
        cout << "Seats for " << session[a].location << endl;
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 15; j++) {
                if (session[a].vipSeats[i][j] == 'O') {
                    vipCount++;
                }
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 15; j++) {
                if (session[a].vipSeats[i][j] == 'O') {
                    standardCount++;
                }
            }
        }
        cout << "\nVIP Seats Available Now: " << vipCount << endl;
        cout << "Standard Seats Available Now: " << standardCount << endl << endl;
    }
    cout << endl;
}