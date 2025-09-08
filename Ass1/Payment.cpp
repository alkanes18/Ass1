#include "Payment.h"

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <cctype>

using namespace std;

// Display payment methods
void displayPaymentMethods() {
    cout << "\n=== Payment Methods ===" << endl;
    cout << "1. Credit Card" << endl;
    cout << "2. Debit Card" << endl;
    cout << "3. Online Banking" << endl;
    cout << "4. E-Wallet" << endl;
    cout << "0. Cancel Payment" << endl;
}

// Process payment
void processPayment(Order& order, PaymentInfo& payment) {
    cout << "\n=== Payment Processing ===" << endl;
    cout << "Total Amount: RM" << fixed << setprecision(2) << order.totalAmount << endl;

    int methodChoice;
    string input;

    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear leftover input

    // loop until valid choice or cancel
    while (true) {
        displayPaymentMethods();
        cout << "Select payment method (0-4): ";
        getline(cin, input);

        if (input.empty()) {
            cout << "Error: Invalid input. Please enter a number between 0-4." << endl;
            continue;
        }

        try {
            methodChoice = stoi(input);
        }
        catch (...) {
            cout << "Error: Invalid input. Please enter a number between 0-4." << endl;
            continue;
        }

        if (methodChoice < 0 || methodChoice > 4) {
            cout << "Error: Invalid choice. Please enter a number between 0-4." << endl;
            continue;
        }

        if (methodChoice == 0) {
            cout << "Payment cancelled by user." << endl;
            order.paymentStatus = "Cancelled";
            order.paymentMethod = "Cancelled";
            return;
        }

        // assign method
        switch (methodChoice) {
        case 1: payment.paymentMethod = "Credit Card"; break;
        case 2: payment.paymentMethod = "Debit Card"; break;
        case 3: payment.paymentMethod = "Online Banking"; break;
        case 4: payment.paymentMethod = "E-Wallet"; break;
        }
        break; // valid choice ? exit loop
    }

    // === Payment details ===
    if (payment.paymentMethod == "Online Banking") {
        // Account number
        do {
            cout << "\nEnter account number (10-12 digits): ";
            getline(cin, payment.cardNumber);

            if (payment.cardNumber.empty()) {
                cout << "Error: Account number cannot be empty." << endl;
                continue;
            }
            if (payment.cardNumber.length() < 10 || payment.cardNumber.length() > 12) {
                cout << "Error: Account number must be 10-12 digits." << endl;
                continue;
            }
            bool validDigits = true;
            for (char c : payment.cardNumber) {
                if (!isdigit(c)) { validDigits = false; break; }
            }
            if (!validDigits) {
                cout << "Error: Account number must contain only digits." << endl;
                continue;
            }
            break;
        } while (true);

        // Account holder
        do {
            cout << "Enter account holder name: ";
            getline(cin, payment.cardHolder);

            if (payment.cardHolder.empty()) {
                cout << "Error: Account holder name cannot be empty." << endl;
                continue;
            }
            bool validName = true;
            for (char c : payment.cardHolder) {
                if (!isalpha(c) && c != ' ') { validName = false; break; }
            }
            if (!validName) {
                cout << "Error: Account holder name must contain only letters and spaces." << endl;
                continue;
            }
            break;
        } while (true);
    }
    else if (payment.paymentMethod == "E-Wallet") {
        cout << "\n=== E-Wallet Payment ===" << endl;
        cout << "Scan the QR code to proceed with payment..." << endl;

        string confirmInput;
        cout << "\nDid you complete the payment? (Y/N): ";
        getline(cin, confirmInput);

        if (!confirmInput.empty() && toupper(confirmInput[0]) == 'Y') {
            order.paymentMethod = "E-Wallet";
            order.paymentStatus = "Paid";
            payment.cardNumber = "-";
            payment.cardHolder = "-";
            cout << "E-Wallet payment successful!" << endl;
        }
        else {
            order.paymentStatus = "Failed";
            cout << "E-Wallet payment cancelled." << endl;
        }
        return;
    }
    else {
        // Card number
        do {
            cout << "\nEnter card number (13-19 digits): ";
            getline(cin, payment.cardNumber);

            if (payment.cardNumber.empty()) {
                cout << "Error: Card number cannot be empty." << endl;
                continue;
            }
            if (payment.cardNumber.length() < 13 || payment.cardNumber.length() > 19) {
                cout << "Error: Card number must be 13-19 digits." << endl;
                continue;
            }
            bool validDigits = true;
            for (char c : payment.cardNumber) {
                if (!isdigit(c)) { validDigits = false; break; }
            }
            if (!validDigits) {
                cout << "Error: Card number must contain only digits." << endl;
                continue;
            }
            break;
        } while (true);

        // Card holder
        do {
            cout << "Enter card holder name: ";
            getline(cin, payment.cardHolder);

            if (payment.cardHolder.empty()) {
                cout << "Error: Card holder name cannot be empty." << endl;
                continue;
            }
            bool validName = true;
            for (char c : payment.cardHolder) {
                if (!isalpha(c) && c != ' ') { validName = false; break; }
            }
            if (!validName) {
                cout << "Error: Card holder name must contain only letters and spaces." << endl;
                continue;
            }
            break;
        } while (true);
    }

    // Success
    cout << "\nProcessing payment..." << endl;
    cout << "Payment processed successfully!" << endl;
    order.paymentStatus = "Paid";
    order.paymentMethod = payment.paymentMethod;
}

// Generate receipt
void generateReceipt(const Order& order, const PaymentInfo& payment) {
    cout << "\n=== Receipt ===" << endl;
    cout << "Order ID       : " << order.orderID << endl;
    cout << "User ID        : " << order.userID << endl;
    cout << "Concert        : " << order.concertName << endl;
    cout << "Date           : " << order.concertDate << " " << order.concertTime << endl;
    cout << "Location       : " << order.location << endl;
    cout << "Total Amount   : RM" << fixed << setprecision(2) << order.totalAmount << endl;
    cout << "Payment Method : " << payment.paymentMethod << endl;
    cout << "Payment Status : " << order.paymentStatus << endl;
    cout << "Order Date     : " << order.orderDate << endl;
    cout << "================" << endl;
}

// Save order to file
void saveOrderToFile(const Order& order) {
    ofstream file("order_history.txt", ios::app);
    if (file.is_open()) {
        file << "Order ID       : " << order.orderID << "\n";
        file << "User ID        : " << order.userID << "\n";
        file << "Concert        : " << order.concertName << "\n";
        file << "Date           : " << order.concertDate << " " << order.concertTime << "\n";
        file << "Location       : " << order.location << "\n";
        file << "Total Amount   : RM" << fixed << setprecision(2) << order.totalAmount << "\n";
        file << "Payment Method : " << order.paymentMethod << "\n";
        file << "Payment Status : " << order.paymentStatus << "\n";
        file << "Order Date     : " << order.orderDate << "\n";

        for (const auto& t : order.tickets) {
            file << "Ticket ID      : " << t.ticketID
                << " | Session ID: " << t.sessionID
                << " | Seat Type: " << t.seatType
                << " | Row: " << t.seatRow
                << " | Col: " << t.seatCol
                << " | Price: RM" << fixed << setprecision(2) << t.price
                << "\n";
        }

        file << "====================\n\n";
        file.close();
    }
    else {
        cout << "Error: Could not open file to save order history." << endl;
    }
}

// Display order history
void displayOrderHistory(const string& userID) {
    ifstream file("order_history.txt");
    if (!file.is_open()) {
        cout << "Error: Could not open order history file." << endl;
        return;
    }

    cout << "\n=== Order History for User ID: " << userID << " ===" << endl;

    vector<string> block;
    string line;
    bool anyPrinted = false;

    auto flushBlockIfMatch = [&]() {
        if (block.empty()) return;
        bool match = false;
        for (const auto& b : block) {
            if (b.find("User ID") != string::npos && b.find(userID) != string::npos) {
                match = true;
                break;
            }
        }
        if (match) {
            for (const auto& b : block) cout << b << endl;
            cout << endl;
            anyPrinted = true;
        }
        block.clear();
    };

    while (getline(file, line)) {
        block.push_back(line);
        if (line == "====================") {
            flushBlockIfMatch();
        }
    }
    // Flush last block if file didn't end with delimiter
    flushBlockIfMatch();

    if (!anyPrinted) {
        cout << "No orders found for User ID: " << userID << endl;
    }
}