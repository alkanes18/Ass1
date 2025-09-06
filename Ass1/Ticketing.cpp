#include "Ticketing.h"
#include "User.h"
#include "SessionBooking.h"
#include "Payment.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>

using namespace std;

// Initialize merchandise items
void initializeMerchandise(vector<Merchandise>& merchandise) {
    merchandise = {
        {"M001", "Light Stick", "Official concert light stick", 25.0, 100, 100},
        {"M002", "Fan Banner", "Concert fan banner", 15.0, 50, 50},
        {"M003", "T-Shirt", "Official concert T-shirt", 45.0, 75, 75},
        {"M004", "Poster", "Concert poster", 10.0, 200, 200},
        {"M005", "Keychain", "Concert keychain", 8.0, 150, 150},
        {"M006", "Sticker Pack", "Concert stickers", 5.0, 300, 300}
    };
}

// Generate unique ticket ID
string generateTicketID(const string& seatType) {
    static int ticketCounter = 1;
    stringstream ss;
    ss << seatType.substr(0, 3) << "2025" << setfill('0') << setw(4) << ticketCounter++;
    return ss.str();
}

// Generate unique order ID
string generateOrderID() {
    ifstream file("order_history.txt");
    string line, lastOrderID = "ORD000001";
    while (getline(file, line)) {
        if (line.find("Order ID") != string::npos) {
            lastOrderID = line.substr(line.find(":") + 2);
            lastOrderID.erase(remove_if(lastOrderID.begin(), lastOrderID.end(), ::isspace), lastOrderID.end());
        }
    }
    file.close();

    int number = stoi(lastOrderID.substr(3)); // "ORDXXXXXX" -> XXXXXX
    stringstream ss;
    ss << "ORD" << setfill('0') << setw(6) << number + 1;
    return ss.str();
}

string getCurrentDateTime() {
    time_t now = time(0);
    tm ltm;
    localtime_s(&ltm, &now);

    stringstream ss;
    ss << setfill('0') << setw(2) << ltm.tm_mday << "/"
        << setfill('0') << setw(2) << (1 + ltm.tm_mon) << "/"
        << (1900 + ltm.tm_year) << " "
        << setfill('0') << setw(2) << ltm.tm_hour << ":"
        << setfill('0') << setw(2) << ltm.tm_min;

    return ss.str();
}

// Display ticket prices for a session
void displayTicketPrices(const Session& session) {
    cout << "\n=== Ticket Prices for " << session.location << " ===" << endl;
    cout << "+---------------------------------+" << endl;
    cout << "| VIP Ticket      : RM " << fixed << setprecision(2) << setw(11) << session.vipPrice << "|" << endl;
    cout << "| Standard Ticket : RM " << fixed << setprecision(2) << setw(11) << session.standardPrice << "|" << endl;
    cout << "+---------------------------------+" << endl;
}

// Enhanced ticket purchase menu
void displayTicketPurchaseMenu(int userIndex, vector<User>& users, vector<Session>& sessions, vector<Merchandise>& merchandise) {
    cout << "Welcome, " << users[userIndex].name << "!" << endl;

    // Step 1: Select Session
    int idx = selectSession(sessions);
    if (idx == -1) return;

    string chosenSessionID = sessions[idx].sessionID;

    // Step 2: Display ticket prices
    displayTicketPrices(sessions[idx]);

    // Step 3: Select seat type
    string seatType;
    selectSeatType(seatType);
    if (seatType.empty()) return;


    // Step 4: Select seat
    int row, col;
    selectSeat(sessions[idx], seatType, row, col);
    if (row == -1 || col == -1) return;

    // Step 5: Create ticket
    Ticket ticket;
    ticket.ticketID = generateTicketID(seatType);
    ticket.userID = users[userIndex].userID;
    ticket.sessionID = chosenSessionID;
    ticket.seatType = seatType;
    ticket.seatRow = row;
    ticket.seatCol = col;
    ticket.price = (seatType == "VIP") ? sessions[idx].vipPrice : sessions[idx].standardPrice;
    ticket.purchaseDate = getCurrentDateTime();
    ticket.isPaid = false;

    // Step 6: Select merchandise
    vector<Merchandise> selectedMerchandise;
    displayMerchandiseMenu(merchandise, selectedMerchandise);

    // Step 7: Create order
    Order order;
    order.orderID = generateOrderID();
    order.userID = users[userIndex].userID;
    order.username = users[userIndex].name;
    order.fullName = users[userIndex].name;
    order.concertName = "Spatula & Guitar Concert";
    order.concertDate = sessions[idx].time.substr(0, sessions[idx].time.find(" ("));
    order.concertTime = sessions[idx].time.substr(sessions[idx].time.find("(") + 1);
    order.concertTime = order.concertTime.substr(0, order.concertTime.find(")"));
    order.location = sessions[idx].location;
    order.tickets.push_back(ticket);
    order.merchandise = selectedMerchandise;

    // Calculate total amount
    order.totalAmount = ticket.price;
    for (const auto& item : selectedMerchandise) {
        order.totalAmount += item.price;
    }

    order.orderDate = getCurrentDateTime();
    order.paymentStatus = "Pending";

    // Step 8: Process payment
    PaymentInfo payment;
    processPayment(order, payment);

    // Check if payment was cancelled or failed
    if (order.paymentStatus == "Cancelled") {
        cout << "\n=== Purchase Cancelled ===" << endl;
        cout << "Payment was cancelled. Ticket booking cancelled." << endl;
        return;
    }

    if (order.paymentStatus == "Failed") {
        cout << "\n=== Purchase Failed ===" << endl;
        cout << "Payment failed. Ticket booking cancelled." << endl;
        return;
    }

    if (seatType == "VIP") {
        sessions[idx].vipTicketsSold++;
    }
    else {
        sessions[idx].standardTicketsSold++;
    }

    // Step 9: Generate receipt
    generateReceipt(order, payment);

    // Step 10: Save to file
    saveOrderToFile(order);
    saveUsersToFile(users);
    saveSeatsToFile(sessions);

    cout << "\n=== Purchase Complete! ===" << endl;
    cout << "Your ticket has been booked successfully!" << endl;
    cout << "Order ID: " << order.orderID << endl;
    cout << "Total Amount: RM" << fixed << setprecision(2) << order.totalAmount << endl;
}

// Select session for booking
int selectSession(vector<Session>& sessions) {
    cout << "\nAvailable Sessions:" << endl;
    cout << "==================" << endl;

    for (int i = 0; i < sessions.size(); i++) {
        cout << (i + 1) << ". " << sessions[i].location << " - " << sessions[i].time << endl;
    }

    int choice;
    cout << "\nSelect session (1-" << sessions.size() << "): ";
    cin >> choice;

    if (choice < 1 || choice > sessions.size()) {
        cout << "Invalid session selection." << endl;
        return -1;
    }

    return choice - 1;
}

// Select seat type
void selectSeatType(string& seatType) {
    cout << "\nSelect Seat Type:" << endl;
    cout << "1. VIP (Premium seating)" << endl;
    cout << "2. Standard (Regular seating)" << endl;
    cout << "Choice: ";

    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        seatType = "VIP";
        break;
    case 2:
        seatType = "Standard";
        break;
    default:
        cout << "Invalid choice." << endl;
        seatType = "";
        return;
    }

    cout << "Selected: " << seatType << " seats" << endl;
}

// Select specific seat
void selectSeat(Session& session, string seatType, int& row, int& col) {
    bool displayBookingMenu = true;

    cout << "\n=== Seat Selection ===" << endl;
    displaySeats(session);

    while (displayBookingMenu) {
        // Step 1: Get row (A-J) - Single character only
        char rowChar;
        string rowInput;

        do {
            cout << "\nEnter seat row (A-J): ";
            cin >> rowInput;

            // Check if input is exactly one character
            if (rowInput.length() != 1) {
                cout << "Error: Please enter only one character (A-J)." << endl;
                continue;
            }

            rowChar = toupper(rowInput[0]);
            if (rowChar < 'A' || rowChar > 'J') {
                cout << "Error: Invalid row. Please enter A-J only." << endl;
                continue;
            }
            break;
        } while (true);

        int originalRow = rowChar - 'A';

        // Step 2: Get column (1-15)
        do {
            cout << "Enter seat column (1-15): ";
            string colInput;
            cin >> colInput;

            // Check if input is numeric
            bool validNumber = true;
            for (char c : colInput) {
                if (!isdigit(c)) {
                    validNumber = false;
                    break;
                }
            }

            if (!validNumber) {
                cout << "Error: Invalid column. Please enter a number between 1-15." << endl;
                continue;
            }

            col = stoi(colInput);
            if (col < 1 || col > 15) {
                cout << "Error: Invalid column. Please enter a number between 1-15." << endl;
                continue;
            }

            col--; // Convert to 0-based index
            break;
        } while (true);

        int adjustedRow = -1;

        // Validate seat selection
        if (seatType == "VIP") {
            if (originalRow < 0 || originalRow >= 2) {
                cout << "\nError: Invalid VIP seat selection. VIP seats are in rows A-B only." << endl;
                continue;
            }
            adjustedRow = originalRow;
        }
        else {
            if (originalRow < 2 || originalRow >= 10) {
                cout << "\nError: Invalid Standard seat selection. Standard seats are in rows C-J only." << endl;
                continue;
            }
            adjustedRow = originalRow - 2;
        }

        // Check if seat is available
        if (!isSeatAvailable(session, seatType, adjustedRow, col)) {
            cout << "Error: Seat is already taken! Please choose another seat." << endl;
            continue;
        }

        // Book the seat
        if (bookSeat(session, seatType, adjustedRow, col)) {
            cout << "Seat " << seatName(originalRow, col) << " selected successfully!" << endl;
            row = originalRow;
            return;
        }
        else {
            cout << "Error: Booking failed, please try again.\n";
        }
    }
}

// Display merchandise menu
void displayMerchandiseMenu(vector<Merchandise>& merchandise, vector<Merchandise>& selectedItems) {
    cout << "\n=== Add-on Merchandise ===" << endl;
    cout << "Would you like to add merchandise to your order? (y/n): ";
    char choice;
    cin >> choice;

    if (toupper(choice) != 'Y') {
        cout << "No merchandise selected." << endl;
        return;
    }

    while (true) {
        cout << "\nAvailable Merchandise:" << endl;
        cout << "=====================" << endl;
        for (int i = 0; i < merchandise.size(); i++) {
            cout << (i + 1) << ". " << merchandise[i].name << " - RM"
                << fixed << setprecision(2) << merchandise[i].price
                << " (Stock: " << merchandise[i].stock << ")" << endl;
            cout << "   " << merchandise[i].description << endl;
        }
        cout << "0. Finish selection" << endl;

        cout << "\nSelect item (0-" << merchandise.size() << "): ";
        int selection;
        cin >> selection;

        if (selection == 0) break;

        if (selection < 1 || selection > merchandise.size()) {
            cout << "Invalid selection." << endl;
            continue;
        }

        int index = selection - 1;
        if (merchandise[index].stock > 0) {
            selectedItems.push_back(merchandise[index]);
            merchandise[index].stock--;
            cout << merchandise[index].name << " added to cart!" << endl;
        }
        else {
            cout << "Sorry, this item is out of stock." << endl;
        }
    }

    if (!selectedItems.empty()) {
        cout << "\nSelected Items:" << endl;
        double total = 0;
        for (const auto& item : selectedItems) {
            cout << "- " << item.name << ": RM" << fixed << setprecision(2) << item.price << endl;
            total += item.price;
        }
        cout << "Merchandise Total: RM" << fixed << setprecision(2) << total << endl;
    }
}

string seatName(int row, int col) {
    char rowLetter = 'A' + row;
    int colNumber = col + 1;
    return string(1, rowLetter) + to_string(colNumber);
}

void processTicketRefund(const string& userID, vector<Session>& sessions) {
    // Load all orders for the user and check if any exist
    vector<string> allOrdersForUser;
    ifstream fin("order_history.txt");
    string line;
    while (getline(fin, line)) {
        if (line.find("User ID") != string::npos && line.find(userID) != string::npos) {
            allOrdersForUser.push_back(line);
        }
    }
    fin.close();

    // if no orders found, return
    if (allOrdersForUser.empty()) {
        cout << "No orders found for User ID: " << userID << ". Nothing to cancel.\n";
        return;
    }

    displayOrderHistory(userID);

    cout << "Enter Order ID to cancel (or 0 to abort): ";
    string orderToCancel;
    getline(cin, orderToCancel);
    if (orderToCancel == "0") return;

    vector<string> lines;
    while (getline(fin, line)) lines.push_back(line);
    fin.close();

    vector<string> newLines;
    bool skipBlock = false;

    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].find("Order ID") != string::npos &&
            lines[i].find(orderToCancel) != string::npos) {
            skipBlock = true; // start skipping this order

            // release seats
            size_t j = i + 1;
            while (j < lines.size() && lines[j] != "====================") {
                if (lines[j].find("Ticket ID") != string::npos) {
                    string sessionID, seatType;
                    int row, col;

                    // use find and substr to extract details
                    size_t posSession = lines[j].find("Session ID: ");
                    size_t posSeat = lines[j].find("Seat Type: ");
                    size_t posRow = lines[j].find("Row: ");
                    size_t posCol = lines[j].find("Col: ");

                    if (posSession != string::npos && posSeat != string::npos &&
                        posRow != string::npos && posCol != string::npos) {

                        sessionID = lines[j].substr(posSession + 12, posSeat - (posSession + 15));
                        seatType = lines[j].substr(posSeat + 11, posRow - (posSeat + 14));
                        row = stoi(lines[j].substr(posRow + 5, posCol - (posRow + 5) - 3));
                        col = stoi(lines[j].substr(posCol + 5));

                        for (auto& s : sessions) {
                            if (s.sessionID == sessionID) {
                                if (seatType == "VIP") s.vipSeats[row][col] = 'O';
                                else s.standardSeats[row - 2][col] = 'O';
                            }
                        }
                    }
                }
                j++;
            }
        }

        if (!skipBlock) newLines.push_back(lines[i]);
        if (lines[i].find("====================") != string::npos) skipBlock = false; // end skipping
    }

    ofstream fout("order_history.txt", ios::trunc);
    for (const auto& l : newLines) fout << l << "\n";
    fout.close();

    saveSeatsToFile(sessions);

    cout << "Order " << orderToCancel << " cancelled and seats released successfully.\n";
}