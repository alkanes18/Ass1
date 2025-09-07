#pragma once

#include <string>
#include <vector>

#include "User.h"
#include "Ticketing.h"
#include "Payment.h"

void displayReportMenu();
void runReportMenu(const std::vector<Session>& session, const std::vector<Merchandise>& merchandise);
void viewReport(const std::vector<Session>& session, const std::vector<Merchandise>& merchandise);
void viewMerchReport(const std::vector<Merchandise>& merchandise);
void viewSessionDetailedReport(const std::vector<Session>& session);
void writeReportToFile(const std::vector<Session>& session, const std::vector<Merchandise>& merchandise);
double calculateMerchIncome(const std::vector<Merchandise>& merchandise);