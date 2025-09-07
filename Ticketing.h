#pragma once

#include <string>
#include <vector>

#include "User.h"
#include "SessionBooking.h"

// Additional structures for enhanced ticket system
struct Merchandise {
    std::string itemID;
    std::string name;
    std::string description;
    double price;
    int stock;
    int initialStock;
};

struct Ticket {
    std::string ticketID;
    std::string userID;
    std::string sessionID;
    std::string seatType;
    int seatRow, seatCol;
    double price;
    std::string purchaseDate;
    bool isPaid;
};

struct Order {
    std::string orderID;
    std::string userID;
    std::string username;
    std::string fullName;
    std::string concertName;
    std::string concertDate;
    std::string concertTime;
    std::string location;
    std::vector<Ticket> tickets;
    std::vector<Merchandise> merchandise;
    double totalAmount;
    std::string paymentMethod;
    std::string paymentStatus;
    std::string orderDate;
};

// Initialization
void initializeMerchandise(std::vector<Merchandise>& merchandise);

// Ticket/Order IDs
std::string generateTicketID(const std::string& seatType);
std::string generateOrderID();
std::string getCurrentDateTime();

// Ticket purchase flow
void displayTicketPrices(const Session& session);
void displayTicketPurchaseMenu(int userIndex, std::vector<User>& users, std::vector<Session>& sessions, std::vector<Merchandise>& merchandise);
int selectSession(std::vector<Session>& sessions);
void selectSeatType(std::string& seatType);
void selectSeat(Session& session, std::string seatType, int& row, int& col);

// Merchandise
void displayMerchandiseMenu(std::vector<Merchandise>& merchandise, std::vector<Merchandise>& selectedItems);
std::string seatName(int row, int col);
void processTicketRefund(const std::string& userID, std::vector<Session>& sessions);