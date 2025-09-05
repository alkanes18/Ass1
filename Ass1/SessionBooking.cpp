#include "SessionBooking.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>

using namespace std;

void displaySessionMenu1() {
    cout << "\nSession Menu (ORGANIZER ONLY)" << endl;
    cout << "+----------------------------+" << endl;
    cout << "| 1. Display All Sessions    |" << endl;
    cout << "| 2. Add New Session         |" << endl;
    cout << "| 3. Delete Session          |" << endl;
    cout << "| 4. Reset Session           |" << endl;
    cout << "| 5. Report Menu             | " << endl;
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
        session[a].vipTicketsSold = 0;
        session[a].standardTicketsSold = 0;
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

void saveSeatsToFile(const vector<Session>& session) {
    ofstream file("seats.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open seats.txt for writing!\n";
        return;
    }

    for (const auto& s : session) {
        file << "SessionID:" << s.sessionID << "\n";

        // Save VIP seats
        for (int r = 0; r < 2; r++) {
            for (int c = 0; c < 15; c++) {
                file << s.vipSeats[r][c];
            }
            file << "\n";
        }

        // Save Standard seats
        for (int r = 0; r < 8; r++) {
            for (int c = 0; c < 15; c++) {
                file << s.standardSeats[r][c];
            }
            file << "\n";
        }

        file << "END\n";
    }

    file.close();
}

void loadSeatsFromFile(vector<Session>& session) {
    ifstream file("seats.txt");
    if (!file.is_open()) {
        cout << "No previous seat data found, starting fresh.\n";
        return;
    }

    string line;
    int sessionIndex = -1;
    int row = 0;

    while (getline(file, line)) {
        if (line.find("SessionID:") != string::npos) {
            string id = line.substr(line.find(":") + 1);

            id.erase(0, id.find_first_not_of(" \t"));

            for (int i = 0; i < session.size(); i++) {
                if (session[i].sessionID == id) {
                    sessionIndex = i;
                    row = 0;
                    break;
                }
            }
        }
        else if (line == "END") {
            sessionIndex = -1; // reset for next session
        }
        else if (sessionIndex != -1) {
            if (row < 2) { // VIP
                for (int c = 0; c < 15; c++) {
                    session[sessionIndex].vipSeats[row][c] = line[c];
                }
            }
            else { // Standard
                for (int c = 0; c < 15; c++) {
                    session[sessionIndex].standardSeats[row - 2][c] = line[c];
                }
            }
            row++;
        }
    }

    file.close();
    cout << "Seats loaded successfully!\n";
}

void saveSessionsToFile(const vector<Session>& session) {
    ofstream file("sessions.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open sessions.txt for writing!\n";
        return;
    }

    for (const auto& s : session) {
        file << s.sessionID << "\n";
        file << s.location << "\n";
        file << s.time << "\n";
        file << s.vipPrice << "\n";
        file << s.standardPrice << "\n";
        file << s.vipTicketsSold << "\n";
        file << s.standardTicketsSold << "\n";
        file << "---\n";
    }

    file.close();
}

vector<Session> loadSessionsFromFile() {
    vector<Session> sessions;
    ifstream file("sessions.txt");
    if (!file.is_open()) {
        return sessions;
    }

    while (true) {
        Session s;

        if (file.fail()) {
            cout << "Warning: Corrupted session data in file.\n";
            break;
        }

        if (!(file >> s.sessionID)) break;

        file.ignore();
        getline(file, s.location);
        getline(file, s.time);
        file >> s.vipPrice;
        file >> s.standardPrice;
        file >> s.vipTicketsSold;
        file >> s.standardTicketsSold;

        file.ignore();
        string separator;
        getline(file, separator); // read ---

        sessions.push_back(s);
    }

    file.close();
    return sessions;
}

void addSession(vector<Session>& session) {
    Session newSession;

    newSession.sessionID = generateSessionID(session);

    cout << "\n===== Add a Session =====" << endl;
    cin.ignore();

    do {
        cout << "Enter the location (Eg. Kuala Lumpur, Malaysia): ";
        getline(cin, newSession.location);

        if (newSession.location.empty()) {
            cout << "Error: Location cannot be empty! Please enter again.\n";
        }
    } while (newSession.location.empty());

    do {
        cout << "Enter the date & time (Eg. 17/9/2025 (7.30pm - 10.30pm)): ";
        getline(cin, newSession.time);

        if (newSession.time.empty()) {
            cout << "Error: Date & Time cannot be empty! Please enter again.\n";
        }
    } while (newSession.time.empty());

    int choice = 0;
    bool askPrice = true;
    while (askPrice) {
        cout << "\nPrice for Seats (" << newSession.location << " - " << newSession.time << ")" << endl;
        cout << "----------------------------------------------------------------" << endl;
        cout << "1. Default Price (VIP - RM400, Standard - RM150)" << endl;
        cout << "2. Custom Price" << endl;

        cout << "Your choice (1 - 2): ";
        if (!(cin >> choice)) {
            cout << "Error: Invalid input. Please enter a number (1-2)." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1:
            newSession.vipPrice = 400.00;
            newSession.standardPrice = 150.00;
            askPrice = false;
            break;
        case 2:
            do {
                cout << "\nEnter the VIP ticket price (RM): ";
                if (!(cin >> newSession.vipPrice)) {
                    cout << "Error: Invalid input. Please enter a valid number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (newSession.vipPrice <= 0) {
                    cout << "Price cannot be negative or zero! Please enter again." << endl;
                }
            } while (newSession.vipPrice <= 0);

            do {
                cout << "Enter the Standard ticket price (RM): ";
                if (!(cin >> newSession.standardPrice)) {
                    cout << "Error: Invalid input. Please enter a valid number." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (newSession.standardPrice <= 0) {
                    cout << "Price cannot be negative or zero! Please enter again." << endl;
                }
            } while (newSession.standardPrice <= 0);
            askPrice = false;
            break;
        default:
            cout << "Invalid option! Please type 1 - 2 only." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
    }
    initializeSeats(newSession);
    confirmAdd(session, newSession);
}

void deleteSession(vector<Session>& session) {
    cout << "\n\n===== Delete a Session =====" << endl;
    cout << "There are currently " << session.size() << " session(s) available." << endl << endl;

    displayAllSessions(session);

    int num;
    while (true) {
        cout << "Enter the session number to delete (1 - " << session.size() << "): ";
        if (!(cin >> num)) {
            cout << "Error: Invalid input. Please enter a valid session number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (num < 1 || num > session.size()) {
            cout << "Error: Number out of range. Please enter between 1 and " << session.size() << "." << endl;
            continue;
        }
        break;
    }

    int sold = session[num - 1].vipTicketsSold + session[num - 1].standardTicketsSold;
    if (sold > 0) {
        cout << "Cannot delete this session. " << sold << " tickets have already been booked." << endl;
        return;
    }

    if (hasBooking(session[num - 1])) {
        cout << "This session has bookings. Cannot delete the session." << endl;
        return;
    }

    confirmDelete(session, num);
}

bool hasBooking(const Session& session) {
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 15; c++) {
            if (session.vipSeats[r][c] == 'X') {
                return true;
            }
        }
    }

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 15; c++) {
            if (session.standardSeats[r][c] == 'X') {
                return true;
            }
        }
    }

    return false;
}

void confirmAdd(vector<Session>& session, Session newSession) {
    char choice;
    while (true) {
        cout << "\nConfirm to add this session? (Y/N) > ";
        cin >> choice;

        if (toupper(choice) == 'Y') {
            session.push_back(newSession);
            cout << "Session added successfully!" << endl;
            saveSessionsToFile(session);
            break;
        }
        else if (toupper(choice) == 'N') {
            cout << "Operation cancelled." << endl;
            break;
        }
        else {
            cout << "Error: Invalid choice. Please enter Y or N." << endl;
        }
    }
}

void confirmDelete(vector<Session>& session, int num) {
    char choice;
    while (true) {
        cout << "\nAre you sure to delete this session?" << endl;
        cout << "ID          : " << session[num - 1].sessionID << endl;
        cout << "Location    : " << session[num - 1].location << endl;
        cout << "Date & Time : " << session[num - 1].time << endl;
        cout << "(Y/N) > ";
        cin >> choice;

        if (toupper(choice) == 'Y') {
            session.erase(session.begin() + (num - 1));
            cout << "Session deleted successfully!" << endl;
            break;
        }
        else if (toupper(choice) == 'N') {
            cout << "Operation cancelled." << endl;
            break;
        }
        else {
            cout << "Error: Invalid choice. Please enter Y or N." << endl;
        }
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

bool bookSeat(Session& session, string seatType, int row, int col) {
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

void resetSeats(vector<Session>& session, const string& filename) {
    initializeAllSeats(session);

    ifstream inFile(filename);
    if (!inFile) {
        cout << "Failed to open file.\n";
        return;
    }

    string content;
    string line;
    while (getline(inFile, line)) {
        for (char& c : line) {
            if (c == 'X') c = 'O';
        }
        content += line + "\n";
    }
    inFile.close();

    ofstream outFile(filename);
    outFile << content;
    outFile.close();

    cout << "All seats in file " << filename << " have been reset to 'O'.\n";
}

void displayAllSessions(vector<Session> session) {
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
        if (!(cin >> sessionMenuOption2)) {
            cout << "Error: Invalid input. Please enter a number (1-3)." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (sessionMenuOption2) {
        case 1:
            displayAllSeats(session);
            countAllRemainingSeats(session);
            break;
        case 2:
            indexSession = searchForASession(session);
            if (indexSession != -1) {
                cout << endl;
                displaySeats(session[indexSession]);
                countRemainingSeats(session[indexSession]);
            }
            else {
                cout << "Session not found!" << endl;
            }
            break;
        case 3:
            runningSessionMenu2 = false;
            break;
        default:
            cout << "Invalid option! Please type 1 - 3 only." << endl;
            continue;
        }
    }
}

int searchForASession(vector<Session> session) {
    string sessionID;
    cout << "\nEnter the session ID: ";
    cin >> sessionID;

    for (int i = 0; i < session.size(); i++) {
        if (sessionID == session[i].sessionID) {
            return i;
        }
    }
    cout << "Error: Session ID not found." << endl;
    return -1;
}

void countRemainingSeats(Session session) {
    int standardCount = 0, vipCount = 0;

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
    cout << "Standard Seats Available Now: " << standardCount;
}

void countAllRemainingSeats(vector<Session> session) {
    int standardCount = 0, vipCount = 0;

    for (int a = 0; a < session.size(); a++) {
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 15; j++) {
                if (session[a].vipSeats[i][j] == 'O') {
                    vipCount++;
                }
            }
        }
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 15; j++) {
                if (session[a].standardSeats[i][j] == 'O') {
                    standardCount++;
                }
            }
        }
    }
    cout << "\nVIP Seats Available Now: " << vipCount << endl;
    cout << "Standard Seats Available Now: " << standardCount << endl;
    cout << endl;
}
