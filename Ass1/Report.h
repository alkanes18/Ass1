#pragma once

#include <string>
#include <vector>

#include "User.h"
#include "Ticketing.h"
#include "Payment.h"

void viewReport(std::vector<Session> session, std::vector<Merchandise> merchandise);
void writeReportToFile(std::vector<Session> session, std::vector<Merchandise> merchandise);
double calculateMerchIncome(std::vector<Merchandise> merchandise);