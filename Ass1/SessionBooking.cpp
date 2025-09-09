#include "SessionBooking.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

using namespace std;

void displaySessionMenu1() {
    cout << "\n===== Session Menu (ORGANIZER ONLY) =====" << endl;
    cout << "+----------------------------+" << endl;
    cout << "| 1. Display All Sessions    |" << endl;
    cout << "| 2. Add New Session         |" << endl;
    cout << "| 3. Delete Session          |" << endl;
    cout << "| 4. Edit Current Session    |" << endl;
    cout << "| 5. Reset Session           |" << endl;
    cout << "| 6. Report Menu             |" << endl;
    cout << "| 7. Exit Session Menu       |" << endl;
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

// assume all seats are static (2 rows of VIP, 8 rows of Standard, 15 seats each row))
// initialize seats for a single session
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

// initialize seats for all sessions
void initializeAllSeats(vector<Session>& session) {
    for (size_t a = 0; a < session.size(); a++) {
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

// initialize session IDs for all sessions
void initializeSessionID(vector<Session>& session) {
    for (size_t i = 0; i < session.size(); i++) {
        stringstream ss;
        ss << "S" << setw(3) << setfill('0') << i + 1;
        session[i].sessionID = ss.str();
    }
}

// generate next session ID based on current number of sessions
string generateSessionID(vector<Session>& session) {
    size_t nextID = session.size() + 1;
    stringstream ss;
    ss << "S" << setw(3) << setfill('0') << nextID;
    return ss.str();
}

// save seats of all sessions to file to ensure that when the system terminate, the seats availability are persistence 
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

// load seats of all sessions from file
void loadSeatsFromFile(vector<Session>& session) {
    ifstream file("seats.txt"); 
    if (!file.is_open()) { 
        cout << "No previous seat data found, starting fresh.\n"; 
        return; 
    } 

    string line; // to hold each line read from the file 
    int sessionIndex = -1; // to track which session we are loading seats for 
    int row = 0; // to track the current row being read 
    
    while (getline(file, line)) { 
        if (line.find("SessionID:") != string::npos) { 
            string id = line.substr(line.find(":") + 1); // get the session ID (behind of :) 
            
            id.erase(0, id.find_first_not_of(" \t")); // skip the whitespaces 
            
            for (size_t i = 0; i < session.size(); i++) { 
                if (session[i].sessionID == id) { 
                    sessionIndex = i; 
                    row = 0; break; 
                } 
            } 
        } 
        else if (line == "END") { 
            sessionIndex = -1; // reset for next session 
        } else if (sessionIndex != -1) { 
            if (row < 2) { // VIP 
                for (int c = 0; c < 15; c++) { 
                    session[sessionIndex].vipSeats[row][c] = line[c]; 
                } 
            } else { // Standard 
                for (int c = 0; c < 15; c++) { 
                    session[sessionIndex].standardSeats[row - 2][c] = line[c]; 
                } 
            } 
            row++; 
        } 
    } 
    
    file.close(); 
}

// save all session details to file to ensure that when the system terminate, the session details are persistence
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

// load all session details from file
vector<Session> loadSessionsFromFile() {
    vector<Session> sessions;
    ifstream file("sessions.txt");
    if (!file.is_open()) {
        return sessions;
    }

    while (true) {
        Session s;

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

// add a new session
void addSession(vector<Session>& session) {
    Session newSession;

    newSession.sessionID = generateSessionID(session);

    cout << "\n===== Add a Session =====" << endl;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // --- Location ---
    do {
        cout << "Enter the location (Eg. Kuala Lumpur, Malaysia): ";
        getline(cin, newSession.location);

        if (newSession.location.empty()) {
            cout << "Error: Location cannot be empty! Please enter again.\n";
        }
    } while (newSession.location.empty());

    // --- Date & Time ---
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
            cout << "Invalid input! Please enter a number (1-2).\n";
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
                    cout << "Invalid input! Please enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (newSession.vipPrice <= 0) {
                    cout << "Price cannot be negative or zero! Please enter again.\n";
                }
            } while (newSession.vipPrice <= 0);

            do {
                cout << "Enter the Standard ticket price (RM): ";
                if (!(cin >> newSession.standardPrice)) {
                    cout << "Invalid input! Please enter a number.\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                if (newSession.standardPrice <= 0) {
                    cout << "Price cannot be negative or zero! Please enter again.\n";
                }
            } while (newSession.standardPrice <= 0);

            askPrice = false;
            break;

        default:
            cout << "Invalid option! Please type 1 - 2 only.\n";
            break;
        }
    }

    initializeSeats(newSession);
    confirmAdd(session, newSession);
}

// delete an existing session
void deleteSession(vector<Session>& session) {
    cout << "\n\n===== Delete a Session =====" << endl;
    cout << "There are currently " << session.size() << " session(s) available." << endl << endl;

    displayAllSessions(session);

    cout << "Enter the session number to delete (1 - " << session.size() << "): ";
    int num;
    cin >> num;

    if (num < 1 || num > session.size()) {
        cout << "Invalid session number. Return to menu..." << endl;
        return;
    }

    if (hasBooking(session[num - 1])) {
        cout << "This session has bookings. Cannot delete the session." << endl;
        return;
    }

    confirmDelete(session, num);
}

// edit an existing session
void editCurrentSession(vector<Session>& session) {
    if (session.empty()) {
        cout << "No sessions available to edit.\n";
        return;
    }

    cout << "\n--- Edit Session ---\n";
    for (size_t i = 0; i < session.size(); ++i) {
        cout << i + 1 << ". " << session[i].sessionID
            << " | " << session[i].location
            << " | " << session[i].time
            << " | VIP: " << session[i].vipPrice
            << " | Standard: " << session[i].standardPrice << "\n";
    }

    int choice;
    cout << "\nSelect a session to edit (1-" << session.size() << "): ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice < 1 || choice >(int)session.size()) {
        cout << "Invalid choice.\n";
        return;
    }

    Session& s = session[choice - 1];

    int option;
    do {
        cout << "\n--- Editing Session: " << s.sessionID << " ---" << endl;
        cout << "1. Edit Location" << endl;
        cout << "2. Edit Time" << endl;
        cout << "3. Edit VIP Price" << endl;
        cout << "4. Edit Standard Price" << endl;
        cout << "5. Save and Exit" << endl;
        cout << "0. Exit without Saving" << endl;
        cout << "Choice: ";
        cin >> option;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string input;
        switch (option) {
        case 1:
            cout << "Enter new Location: ";
            getline(cin, input);
            if (!input.empty()) s.location = input;
            break;
        case 2:
            cout << "Enter new Time: ";
            getline(cin, input);
            if (!input.empty()) s.time = input;
            break;
        case 3:
            cout << "Enter new VIP Price: ";
            getline(cin, input);
            if (!input.empty()) {
                try {
                    double newPrice = stod(input);
                    if (newPrice < 0) {
                        cout << "Price cannot be negative. Value not updated.\n";
                    }
                    else {
                        s.vipPrice = newPrice;
                    }
                }
                catch (...) {
                    cout << "Invalid input. Price not updated.\n";
                }
            }
            break;
        case 4:
            cout << "Enter new Standard Price: ";
            getline(cin, input);
            if (!input.empty()) {
                try {
                    double newPrice = stod(input);
                    if (newPrice < 0) {
                        cout << "Price cannot be negative. Value not updated.\n";
                    }
                    else {
                        s.standardPrice = newPrice;
                    }
                }
                catch (...) {
                    cout << "Invalid input. Price not updated.\n";
                }
            }
            break;
        case 5:
            char confirm;
            cout << "Save changes? (y/n): ";
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                saveSessionsToFile(session);
                cout << "Changes saved successfully!\n";
            }
            else {
                cout << "Changes discarded.\n";
            }
            break;
        case 0:
            cout << "Exiting without saving changes.\n";
            break;
        default:
            cout << "Invalid choice.\n";
        }
    } while (option != 5 && option != 0);
}

// check if a session has any bookings (if has, cannot delete a session)
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

// confirm addition of a new session
void confirmAdd(vector<Session>& session, Session newSession) {
    char choice;

    // input validation
    do {
        cout << "\nConfirm to add this session? (Y/N) > ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear input buffer
        choice = toupper(choice);

        if (choice != 'Y' && choice != 'N') {
            cout << "Invalid choice. Please enter Y or N.\n";
        }
    } while (choice != 'Y' && choice != 'N');

    // add session when confirmed
    if (choice == 'Y') {
        session.push_back(newSession);
        cout << "Session added successfully!\n";
        saveSessionsToFile(session);
        saveSeatsToFile(session);
    }
    else {
        cout << "Operation cancelled.\n";
    }
}

// confirm deletion of a session
void confirmDelete(vector<Session>& session, int num) {
    char choice;

    cout << "\nAre you sure to delete this session?" << endl;
    cout << "ID          : " << session[num - 1].sessionID << endl;
    cout << "Location    : " << session[num - 1].location << endl;
    cout << "Date & Time : " << session[num - 1].time << endl;

    // input validation
    do {
        cout << "(Y/N) > ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear input buffer
        choice = toupper(choice);

        if (choice != 'Y' && choice != 'N') {
            cout << "Invalid choice. Please enter Y or N." << endl;
        }
    } while (choice != 'Y' && choice != 'N');

    // perform deletion when confirmed
    if (choice == 'Y') {
        session.erase(session.begin() + (num - 1));
        cout << "Session deleted successfully!\n";
        saveSessionsToFile(session);
        saveSeatsToFile(session);
    }
    else {
        cout << "Operation cancelled.\n";
    }
}

// display seats for a specific session
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

// display seats for all sessions
void displayAllSeats(vector<Session> session) {
    for (size_t a = 0; a < session.size(); a++) {
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

// check if a specific seat is available
bool isSeatAvailable(const Session& session, string seatType, int row, int col) {
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

// purchase a specific seat
bool purchaseSeat(Session& session, string seatType, int row, int col) {
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

// reset all seats in the file to 'O' (available)
void resetSeats(vector<Session>& session) {
    initializeAllSeats(session);
    saveSeatsToFile(session);
    cout << "All seats have been reset to 'O'.\n";
}

// display all sessions in a tabular format
void displayAllSessions(vector<Session> session) {
    cout << endl << left << setw(5) << "No." << setw(14) << "Session ID" << setw(40) << "Location" << setw(20) << "Date (Time)" << endl;
    cout << "==================================================================================================" << endl;
    for (size_t i = 0; i < session.size(); i++) {
        cout << left << setw(5) << i + 1 << setw(14) << session[i].sessionID << setw(40) << session[i].location << setw(20) << session[i].time << endl;
        cout << "--------------------------------------------------------------------------------------------------" << endl;
    }
}

// a while loop to run the session menu 2
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
            cout << "Invalid option! Please type 1 - 2 only." << endl;
            continue;
        }
    }
}

// search for a session by its ID, return the index if found, otherwise return -1
int searchForASession(vector<Session> session) {
    Session searchSession;
    string sessionID;

    cout << "\nEnter the session ID: ";
    cin >> sessionID;

    for (size_t i = 0; i < session.size(); i++) {
        if (toupper(sessionID[0]) == toupper(session[i].sessionID[0]) &&
            sessionID.substr(1) == session[i].sessionID.substr(1)) {
            return i;
        }
    }
    return -1;
}

// count remaining seats for a specific session
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

// count remaining seats for all sessions
void countAllRemainingSeats(vector<Session> session) {
    int standardCount = 0, vipCount = 0;

    for (size_t a = 0; a < session.size(); a++) {
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