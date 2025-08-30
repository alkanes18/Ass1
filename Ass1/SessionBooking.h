#pragma once

#include <string>
#include <vector>

// Session structure
struct Session {
    std::string sessionID;
    std::string location;
    std::string time;
    char vipSeats[2][15] = { 0 };
    char standardSeats[8][15] = { 0 };
    double vipPrice;
    int vipTicketsSold = 0;
    double standardPrice;
    int standardTicketsSold = 0;
};

struct BookingInfo {
    std::string attendeeName;
    std::string userID;
    std::string location;
    std::string time;
    std::string seatType;
    int seatRow, seatCol;
    double ticketPrice;
};

struct Attendee {

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

// Session CRUD
void addSession(std::vector<Session>& session);
void deleteSession(std::vector<Session>& session);
void confirmAdd(std::vector<Session>& session, Session newSession);
void confirmDelete(std::vector<Session>& session, int num);

// Seat management
void displaySeats(Session session);
void displayAllSeats(std::vector<Session> session);

bool isSeatAvailable(Session session, std::string seatType, int row, int col);
bool bookSeat(Session& session, Attendee& user, std::string seatType, int row, int col);
void cancelBooking(Attendee& user);

// Session info
void displayAllSessions(std::vector<Session> session);
int searchForASession(std::vector<Session> session);
void countAllRemainingSeats(std::vector<Session> session);
void countRemainingSeats(Session session);