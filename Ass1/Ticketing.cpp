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
#include <algorithm>
#include <ctime>
#include <cctype>

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
    // Check that this user has any orders
    vector<string> anyLines;
    string line;
    {
        ifstream checkFile("order_history.txt");
        while (getline(checkFile, line)) {
            anyLines.push_back(line);
        }
    }

    bool hasOrderForUser = false;
    for (const auto& l : anyLines) {
        if (l.find("User ID") != string::npos && l.find(userID) != string::npos) {
            hasOrderForUser = true;
            break;
        }
    }
    if (!hasOrderForUser) {
        cout << "No orders found for User ID: " << userID << ". Nothing to refund.\n";
        return;
    }

    displayOrderHistory(userID);

    cout << "\nEnter Order ID to refund (or 0 to abort): ";
    string orderID;
    getline(cin, orderID);
    if (orderID == "0") return;

    // Load entire file
    vector<string> lines = move(anyLines);

    // Locate the selected order block and ensure it belongs to user
    size_t orderStart = string::npos;
    size_t orderEnd = string::npos;
    for (size_t i = 0; i < lines.size(); i++) {
        if (lines[i].find("Order ID") != string::npos && lines[i].find(orderID) != string::npos) {
            // Verify belongs to user and find end
            size_t j = i + 1;
            bool belongs = false;
            while (j < lines.size() && lines[j].find("====================") == string::npos) {
                if (lines[j].find("User ID") != string::npos && lines[j].find(userID) != string::npos) {
                    belongs = true;
                }
                j++;
            }
            if (!belongs) {
                cout << "Error: Order does not belong to current user.\n";
                return;
            }
            orderStart = i;
            orderEnd = (j < lines.size() ? j : lines.size() - 1);
            break;
        }
    }

    if (orderStart == string::npos) {
        cout << "Error: Order ID not found.\n";
        return;
    }

    // Collect ticket lines within this order
    vector<size_t> ticketLineIdx;
    for (size_t k = orderStart + 1; k < orderEnd; k++) {
        if (lines[k].find("Ticket ID") != string::npos) {
            ticketLineIdx.push_back(k);
        }
    }

    if (ticketLineIdx.empty()) {
        cout << "No tickets found in this order to refund.\n";
        return;
    }

    // Display tickets with numbering
    cout << "\nTickets in Order " << orderID << ":\n";
    for (size_t t = 0; t < ticketLineIdx.size(); t++) {
        cout << (t + 1) << ") " << lines[ticketLineIdx[t]] << "\n";
    }

    // Choose a ticket to refund
    cout << "Select ticket number to refund (1-" << ticketLineIdx.size() << ") or 0 to abort: ";
    string selInput;
    getline(cin, selInput);
    if (selInput == "0") return;
    size_t choice = 0;
    try {
        choice = static_cast<size_t>(stoi(selInput));
    }
    catch (...) {
        cout << "Invalid selection.\n";
        return;
    }
    if (choice < 1 || choice > ticketLineIdx.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    size_t lineIdx = ticketLineIdx[choice - 1];
    const string& ticketLine = lines[lineIdx];

    auto extractField = [&](const string& label) -> string {
        size_t pos = ticketLine.find(label);
        if (pos == string::npos) return "";
        pos += label.size();
        size_t delim = ticketLine.find(" |", pos);
        if (delim == string::npos) delim = ticketLine.size();
        string value = ticketLine.substr(pos, delim - pos);
        // trim leading/trailing spaces
        size_t first = value.find_first_not_of(' ');
        size_t last = value.find_last_not_of(' ');
        if (first == string::npos) return "";
        return value.substr(first, last - first + 1);
    };

    string sessionID = extractField("Session ID: ");
    string seatType = extractField("Seat Type: ");
    string rowStr = extractField("Row: ");
    string colStr = extractField("Col: ");

    // Parse row/col to integers
    int rowOriginal = -1;
    int col = -1;
    try {
        rowOriginal = stoi(rowStr);
        col = stoi(colStr);
    }
    catch (...) {
        cout << "Error parsing ticket seat info.\n";
        return;
    }

    // Translate to storage indices and release seat
    bool released = false;
    for (auto& s : sessions) {
        if (s.sessionID == sessionID) {
            if (seatType == "VIP") {
                int r = rowOriginal; // VIP rows stored as 0-1
                if (r >= 0 && r < 2 && col >= 0 && col < 15) {
                    s.vipSeats[r][col] = 'O';
                    released = true;
                }
            } else {
                // Standard rows stored as 0-7, original A-J => C-J => 2-9
                int r = rowOriginal - 2;
                if (r >= 0 && r < 8 && col >= 0 && col < 15) {
                    s.standardSeats[r][col] = 'O';
                    released = true;
                }
            }
            break;
        }
    }

    if (!released) {
        cout << "Failed to release seat for the selected ticket.\n";
        return;
    }

    // Remove just this ticket line from the order. If no tickets remain after removal, remove the whole order block.
    vector<string> outLines;
    size_t ticketCountRemain = 0;
    for (size_t i = 0; i < lines.size(); i++) {
        if (i == lineIdx) {
            // skip the selected ticket line
            continue;
        }
        if (i > orderStart && i < orderEnd && lines[i].find("Ticket ID") != string::npos) {
            ticketCountRemain++;
        }
        outLines.push_back(lines[i]);
    }

    // If no tickets remain, delete the whole block from outLines
    if (ticketCountRemain == 0) {
        vector<string> tmp;
        bool skipping = false;
        for (size_t i = 0; i < outLines.size(); i++) {
            if (!skipping && outLines[i].find("Order ID") != string::npos && outLines[i].find(orderID) != string::npos) {
                skipping = true;
                continue;
            }
            if (skipping) {
                if (outLines[i].find("====================") != string::npos) {
                    skipping = false;
                }
                continue;
            }
            tmp.push_back(outLines[i]);
        }
        outLines.swap(tmp);
    }

    // Persist file and seats
    {
        ofstream fout("order_history.txt", ios::trunc);
        for (const auto& l : outLines) fout << l << "\n";
    }
    saveSeatsToFile(sessions);

    if (ticketCountRemain == 0) cout << "Order " << orderID << " removed (all tickets refunded).\n";
    cout << "Selected ticket refunded and seat released successfully.\n";
}