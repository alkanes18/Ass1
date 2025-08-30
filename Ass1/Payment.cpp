#include "Payment.h"

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

// Display payment methods
void displayPaymentMethods() {
    cout << "\n=== Payment Methods ===" << endl;
    cout << "1. Credit Card" << endl;
    cout << "2. Debit Card" << endl;
    cout << "3. Online Banking" << endl;
    cout << "4. E-Wallet" << endl;
}

// Validate payment information
bool validatePaymentInfo(const PaymentInfo& payment) {
    // Basic validation
    if (payment.cardNumber.length() < 13 || payment.cardNumber.length() > 19) {
        cout << "Invalid card number length." << endl;
        return false;
    }

    if (payment.cardHolder.empty()) {
        cout << "Card holder name is required." << endl;
        return false;
    }

    if (payment.expiryDate.length() != 5 || payment.expiryDate[2] != '/') {
        cout << "Invalid expiry date format (MM/YY)." << endl;
        return false;
    }

    if (payment.cvv.length() < 3 || payment.cvv.length() > 4) {
        cout << "Invalid CVV." << endl;
        return false;
    }

    return true;
}

// Process payment
void processPayment(Order& order, PaymentInfo& payment) {
    cout << "\n=== Payment Processing ===" << endl;
    cout << "Total Amount: RM" << fixed << setprecision(2) << order.totalAmount << endl;

    displayPaymentMethods();
    cout << "Select payment method (1-4): ";
    int methodChoice;
    cin >> methodChoice;
    cin.ignore();

    switch (methodChoice) {
    case 1: payment.paymentMethod = "Credit Card"; break;
    case 2: payment.paymentMethod = "Debit Card"; break;
    case 3: payment.paymentMethod = "Online Banking"; break;
    case 4: payment.paymentMethod = "E-Wallet"; break;
    default: payment.paymentMethod = "Credit Card"; break;
    }

    cout << "\nEnter card number: ";
    getline(cin, payment.cardNumber);

    cout << "Enter card holder name: ";
    getline(cin, payment.cardHolder);

    cout << "Enter expiry date (MM/YY): ";
    getline(cin, payment.expiryDate);

    cout << "Enter CVV: ";
    getline(cin, payment.cvv);

    if (validatePaymentInfo(payment)) {
        cout << "\nProcessing payment..." << endl;
        // Simulate payment processing
        cout << "Payment processed successfully!" << endl;
        order.paymentStatus = "Paid";
        order.paymentMethod = payment.paymentMethod;
    }
    else {
        cout << "Payment validation failed. Please try again." << endl;
        order.paymentStatus = "Failed";
    }
}

// Generate receipt
void generateReceipt(const Order& order, const PaymentInfo& payment) {
    string receiptID = generateReceiptID();
    string filename = "receipt_" + receiptID + ".txt";

    ofstream receiptFile(filename);
    if (!receiptFile) {
        cout << "Error creating receipt file." << endl;
        return;
    }

    receiptFile << "================================================" << endl;
    receiptFile << "              CONCERT TICKET RECEIPT" << endl;
    receiptFile << "================================================" << endl;
    receiptFile << "Receipt ID: " << receiptID << endl;
    receiptFile << "Date: " << order.orderDate << endl;
    receiptFile << "Order ID: " << order.orderID << endl;
    receiptFile << endl;
    receiptFile << "Customer Information:" << endl;
    receiptFile << "Username: " << order.username << endl;
    receiptFile << "Full Name: " << order.fullName << endl;
    receiptFile << endl;
    receiptFile << "Concert Details:" << endl;
    receiptFile << "Concert Name: " << order.concertName << endl;
    receiptFile << "Date & Time: " << order.concertDate << " (" << order.concertTime << ")" << endl;
    receiptFile << "Location: " << order.location << endl;
    receiptFile << endl;
    receiptFile << "Ticket Details:" << endl;
    for (const auto& ticket : order.tickets) {
        receiptFile << "- " << ticket.seatType << " Ticket: " << ticket.ticketID << endl;
        receiptFile << "  Seat: " << seatName(ticket.seatRow, ticket.seatCol) << endl;
        receiptFile << "  Price: RM" << fixed << setprecision(2) << ticket.price << endl;
    }
    receiptFile << endl;

    if (!order.merchandise.empty()) {
        receiptFile << "Merchandise:" << endl;
        for (const auto& item : order.merchandise) {
            receiptFile << "- " << item.name << ": RM" << fixed << setprecision(2) << item.price << endl;
        }
        receiptFile << endl;
    }

    receiptFile << "Payment Information:" << endl;
    receiptFile << "Payment Method: " << order.paymentMethod << endl;
    receiptFile << "Payment Status: " << order.paymentStatus << endl;
    receiptFile << endl;
    receiptFile << "Total Amount: RM" << fixed << setprecision(2) << order.totalAmount << endl;
    receiptFile << endl;
    receiptFile << "================================================" << endl;
    receiptFile << "Thank you for your purchase!" << endl;
    receiptFile << "================================================" << endl;

    receiptFile.close();

    cout << "\nReceipt generated: " << filename << endl;
    cout << "You can print this receipt for your records." << endl;
}

// Save order to file
void saveOrderToFile(const Order& order) {
    ofstream orderFile("orders.txt", ios::app);
    if (!orderFile) {
        cout << "Error saving order to file." << endl;
        return;
    }

    orderFile << order.orderID << "," << order.userID << "," << order.username << ","
        << order.concertName << "," << order.concertDate << "," << order.concertTime << ","
        << order.location << "," << order.totalAmount << "," << order.paymentMethod << ","
        << order.paymentStatus << "," << order.orderDate << endl;

    orderFile.close();
}

// Display order history
void displayOrderHistory(const string& userID) {
    ifstream orderFile("orders.txt");
    if (!orderFile) {
        cout << "No order history found." << endl;
        return;
    }

    cout << "\n=== Order History ===" << endl;
    cout << left << setw(12) << "Order ID" << setw(30) << "Concert" << setw(15) << "Location"
        << setw(15) << "Amount" << setw(15) << "Status" << endl;
    cout << string(80, '-') << endl;

    string line;
    bool found = false;
    while (getline(orderFile, line)) {
        stringstream ss(line);
        string orderID, uid, username, concert, date, time, location, amount, method, status, orderDate;

        getline(ss, orderID, ',');
        getline(ss, uid, ',');
        getline(ss, username, ',');
        getline(ss, concert, ',');
        getline(ss, date, ',');
        getline(ss, time, ',');
        getline(ss, location, ',');
        getline(ss, amount, ',');
        getline(ss, method, ',');
        getline(ss, status, ',');
        getline(ss, orderDate);

        if (uid == userID) {
            cout << left << setw(12) << orderID << setw(30) << concert << setw(15) << location
                << setw(3) << "RM" << setw(12) << amount << setw(15) << status << endl;
            found = true;
        }
    }

    if (!found) {
        cout << "No orders found for this user." << endl;
    }

    orderFile.close();
}