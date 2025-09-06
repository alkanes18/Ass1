#pragma once

#include <string>
#include <vector>

#include "Ticketing.h"

struct PaymentInfo {
    std::string cardNumber;
    std::string cardHolder;
    std::string paymentMethod;
};

void displayPaymentMethods();
void processPayment(Order& order, PaymentInfo& payment);
void generateReceipt(const Order& order, const PaymentInfo& payment);

// Order history
void saveOrderToFile(const Order& order);
void displayOrderHistory(const std::string& userID);