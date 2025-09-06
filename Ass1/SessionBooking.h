#pragma once

#include <string>
#include <vector>

// Session structure
struct Session {
    std::string sessionID;
    std::string location;
    std::string time;
    double vipPrice;
    double standardPrice;
    int vipTicketsSold = 0;
    int standardTicketsSold = 0;
    char vipSeats[2][15] = { 'O' };
    char standardSeats[8][15] = { 'O' };
};

// Display menus
void displaySessionMenu1();
void displaySessionMenu2();
void runSessionMenu2(std::vector<Session> session);

// Initialization
void initializeSeats(Session& session);
void initializeAllSeats(std::vector<Session>& session);
void initializeSessionID(std::vector<Session>& session);

// Session ID + seat naming
std::string generateSessionID(std::vector<Session>& session);

// File
void saveSeatsToFile(const std::vector<Session>& session);
void loadSeatsFromFile(std::vector<Session>& session);
void saveSessionsToFile(const std::vector<Session>& session);
std::vector<Session> loadSessionsFromFile();

// Session CRUD
void addSession(std::vector<Session>& session);
void deleteSession(std::vector<Session>& session);
void editCurrentSession(std::vector<Session>& session);
bool hasBooking(const Session& session);
void confirmAdd(std::vector<Session>& session, Session newSession);
void confirmDelete(std::vector<Session>& session, int num);

// Seat management
void displaySeats(Session session);
void displayAllSeats(std::vector<Session> session);

bool isSeatAvailable(const Session& session, std::string seatType, int row, int col);
bool bookSeat(Session& session, std::string seatType, int row, int col);
void resetSeats(std::vector<Session>& session);

// Session info
void displayAllSessions(std::vector<Session> session);
int searchForASession(std::vector<Session> session);
void countAllRemainingSeats(std::vector<Session> session);
void countRemainingSeats(Session session);