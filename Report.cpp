#include "Report.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

// display report menu
void displayReportMenu() {
    cout << "\n===== Report Menu (ORGANIZER ONLY) =====" << endl;
    cout << "+----------------------------------+" << endl;
    cout << "| 1. View Summary Report           |" << endl;
    cout << "| 2. View Merchandise Report       |" << endl;
    cout << "| 3. View Session Detailed Report  |" << endl;
    cout << "| 4. Write Report to File          | " << endl;
    cout << "| 5. Back to Menu                  |" << endl;
    cout << "+----------------------------------+" << endl;
    cout << "Your choice > ";
}

// a while loop to run the report menu
void runReportMenu(const vector<Session>& session, const vector<Merchandise>& merchandise) {
    int reportOption = 0;
    bool runningReportMenu = true;

    while (runningReportMenu) {
        displayReportMenu();
        cin >> reportOption;

        switch (reportOption) {
        case 1:
            viewReport(session, merchandise);
            break;
        case 2:
            viewMerchReport(merchandise);
            break;
        case 3:
            viewSessionDetailedReport(session);
            break;
        case 4:
            writeReportToFile(session, merchandise);
            break;
        case 5:
            runningReportMenu = false;
            break;
        default:
            cout << "Invalid option! Please type 1 - 5 only." << endl;
            continue;
        }
    }
}

// report function 1: view summary report
void viewReport(const vector<Session>& session, const vector<Merchandise>& merchandise) {
    if (session.empty()) {
        cout << "No sessions available to generate report." << endl;
        return;
    }

    int totalVIPSeats = session.size() * 2 * 15;      // 30 VIP seats
    int totalStandardSeats = session.size() * 8 * 15; // 120 Standard seats

    int totalTickets = 0;
    int soldVip = 0;
    int soldStandard = 0;

    int highestIndex = 0;
    int highestTicket = session[0].vipTicketsSold + session[0].standardTicketsSold;
    int lowestIndex = 0;
    int lowestTicket = session[0].vipTicketsSold + session[0].standardTicketsSold;

    double ticketIncome = 0;
    double merchandiseIncome = calculateMerchIncome(merchandise);
    double overallIncome = 0;

    cout << "\n\t===== Concert Ticket Sales Summary =====\n" << endl;
    for (int i = 0; i < session.size(); i++) {
        if ((session[i].vipTicketsSold + session[i].standardTicketsSold) > highestTicket) {
            highestTicket = session[i].vipTicketsSold + session[i].standardTicketsSold;
            highestIndex = i;
        }
        else if (session[i].vipTicketsSold + session[i].standardTicketsSold < lowestTicket) {
            lowestTicket = session[i].vipTicketsSold + session[i].standardTicketsSold;
            lowestIndex = i;
        }
        soldVip += session[i].vipTicketsSold;
        soldStandard += session[i].standardTicketsSold;
        totalTickets += session[i].vipTicketsSold + session[i].standardTicketsSold;
        ticketIncome += session[i].vipPrice * session[i].vipTicketsSold + session[i].standardPrice * session[i].standardTicketsSold;
        cout << "Session " << i + 1 << " - " << session[i].location << " (" << session[i].sessionID << ")" << ": " << (session[i].vipTicketsSold + session[i].standardTicketsSold) << endl;
    }
    overallIncome = ticketIncome + merchandiseIncome;

    cout << "\nTotal Tickets Sold         : " << totalTickets << endl;
    cout << "Total Income (Ticket)      : RM " << fixed << setprecision(2) << ticketIncome << endl;
    cout << "Total Income (Merchandise) : RM " << fixed << setprecision(2) << merchandiseIncome << endl;
    cout << "Overall Income             : RM " << fixed << setprecision(2) << overallIncome << endl;
    cout << "Average Tickets            : " << fixed << setprecision(2) << (double)totalTickets / session.size() << endl;
    cout << "Average Revenue            : RM " << fixed << setprecision(2) << (double)overallIncome / session.size() << endl;
    cout << "Highest Sales              : " << session[highestIndex].location << " (" << (session[highestIndex].vipTicketsSold + session[highestIndex].standardTicketsSold) <<
        " tickets, RM " << fixed << setprecision(2) << (session[highestIndex].vipPrice * session[highestIndex].vipTicketsSold + session[highestIndex].standardPrice * session[highestIndex].standardTicketsSold) << ")" << endl;
    cout << "Lowest Sales               : " << session[lowestIndex].location << " (" << (session[lowestIndex].vipTicketsSold + session[lowestIndex].standardTicketsSold) <<
        " tickets, RM " << fixed << setprecision(2) << (session[lowestIndex].vipPrice * session[lowestIndex].vipTicketsSold + session[lowestIndex].standardPrice * session[lowestIndex].standardTicketsSold) << ")" << endl;
    cout << "\n--- Additional Insights ---" << endl;
    cout << "VIP Tickets Sold        : " << soldVip << " (" << (static_cast<double>(soldVip) / totalVIPSeats) * 100 << "% occupancy)" << endl;
    cout << "Standard Tickets Sold   : " << soldStandard << " (" << (static_cast<double>(soldStandard) / totalStandardSeats) * 100 << "% occupancy)" << endl;
    cout << "Overall Occupancy      : " << fixed << setprecision(2) << (static_cast<double>(soldVip + soldStandard) / (totalVIPSeats + totalStandardSeats)) * 100.0 << "%" << endl;
}

// report function 2: view merchandise report
void viewMerchReport(const vector<Merchandise>& merchandise) {
    struct ItemSummary {
        string name;
        int sold;
        double income;
    };

    vector<ItemSummary> summary;

    cout << "\n\t===== Merchandise Sales Summary =====" << endl;

    for (int i = 0; i < merchandise.size(); i++) {
        int sold = merchandise[i].initialStock - merchandise[i].stock;
        double income = merchandise[i].price * sold;

        summary.push_back({ merchandise[i].name, sold, income });

        cout << left << setw(18) << merchandise[i].name << ": Sold " << sold << ", Remaining " << merchandise[i].stock << ", Income RM " << fixed << setprecision(2) << income << endl;
    }

    sort(summary.begin(), summary.end(), [](const ItemSummary& a, const ItemSummary& b) {
        return a.sold > b.sold;
        });

    cout << "-------------------------------------------------------" << endl;
    cout << "Top-Selling Item: " << summary[0].name << " (" << summary[0].sold << " units, RM " << fixed << setprecision(2) << summary[0].income << ")" << endl;

    cout << "\nRanking of Merchandise by Sales:" << endl;
    for (int i = 0; i < summary.size(); i++) {
        cout << i + 1 << ". " << summary[i].name << " (" << summary[i].sold << " units, RM " << fixed << setprecision(2) << summary[i].income << ")" << endl;
    }
    cout << endl;
}

// report function 3: view session detailed report
void viewSessionDetailedReport(const vector<Session>& session) {
    cout << "\n\t===== Session-wise Detailed Report =====" << endl;

    int totalVIPSeats = 2 * 15;      // 30
    int totalStandardSeats = 8 * 15; // 120

    for (int i = 0; i < session.size(); i++) {
        cout << "\n--- " << session[i].location
            << " (" << session[i].sessionID << ") ---" << endl;

        int soldVIP = session[i].vipTicketsSold;
        int soldStandard = session[i].standardTicketsSold;
        double vipIncome = soldVIP * session[i].vipPrice;
        double standardIncome = soldStandard * session[i].standardPrice;

        cout << left << setw(19) << "VIP Tickets      : " << soldVIP << "/" << totalVIPSeats
            << " (" << fixed << setprecision(2)
            << (static_cast<double>(soldVIP) / totalVIPSeats) * 100 << "%)"
            << " | Income: RM " << vipIncome << endl;

        cout << left << setw(19) << "Standard Tickets : " << soldStandard << "/" << totalStandardSeats
            << " (" << fixed << setprecision(2)
            << (static_cast<double>(soldStandard) / totalStandardSeats) * 100 << "%)"
            << " | Income: RM " << standardIncome << endl;

        cout << left << setw(19) << "Total Tickets    : " << (soldVIP + soldStandard)
            << "/" << (totalVIPSeats + totalStandardSeats)
            << " | Total Income: RM " << (vipIncome + standardIncome) << endl;
    }

    cout << "\n\t=============================================\n";
}

// write the summary report to a text file
void writeReportToFile(const vector<Session>& session, const vector<Merchandise>& merchandise) {
    if (session.empty()) {
        cout << "No sessions available to generate report." << endl;
        return;
    }

    ofstream outFile("report.txt"); // write in

    if (!outFile) {
        cout << "Error opening report file.\n";
        return;
    }

    int totalVIPSeats = session.size() * 2 * 15;      // 30 VIP seats
    int totalStandardSeats = session.size() * 8 * 15; // 120 Standard seats

    int totalTickets = 0;
    int soldVip = 0;
    int soldStandard = 0;

    int highestIndex = 0;
    int highestTicket = session[0].vipTicketsSold + session[0].standardTicketsSold;
    int lowestIndex = 0;
    int lowestTicket = session[0].vipTicketsSold + session[0].standardTicketsSold;

    double ticketIncome = 0;
    double merchandiseIncome = calculateMerchIncome(merchandise);
    double overallIncome = 0;

    // print header
    outFile << left << setw(25) << "=== Concert Ticket Sales Summary ===" << endl << endl;
    for (int i = 0; i < session.size(); i++) {
        if ((session[i].vipTicketsSold + session[i].standardTicketsSold) > highestTicket) {
            highestTicket = session[i].vipTicketsSold + session[i].standardTicketsSold;
            highestIndex = i;
        }
        else if (session[i].vipTicketsSold + session[i].standardTicketsSold < lowestTicket) {
            lowestTicket = session[i].vipTicketsSold + session[i].standardTicketsSold;
            lowestIndex = i;
        }

        soldVip += session[i].vipTicketsSold;
        soldStandard += session[i].standardTicketsSold;
        totalTickets += session[i].vipTicketsSold + session[i].standardTicketsSold;
        ticketIncome += session[i].vipPrice * session[i].vipTicketsSold + session[i].standardPrice * session[i].standardTicketsSold;
        outFile << session[i].location << " (" << session[i].sessionID << ")"
            << ": " << (session[i].vipTicketsSold + session[i].standardTicketsSold) << endl;
    }

    overallIncome = ticketIncome + merchandiseIncome;

    outFile << "\nTotal Tickets Sold         : " << totalTickets << endl;
    outFile << "Total Income (Ticket)      : RM " << fixed << setprecision(2) << ticketIncome << endl;
    outFile << "Total Income (Merchandise) : RM " << fixed << setprecision(2) << merchandiseIncome << endl;
    outFile << "Overall Income             : RM " << fixed << setprecision(2) << overallIncome << endl;
    outFile << "Average Tickets            : " << fixed << setprecision(2) << (double)totalTickets / session.size() << endl;
    outFile << "Average Revenue            : RM " << fixed << setprecision(2) << (double)overallIncome / session.size() << endl;
    outFile << "Highest Sales              : " << session[highestIndex].location << " ("
        << (session[highestIndex].vipTicketsSold + session[highestIndex].standardTicketsSold)
        << " tickets, RM " << fixed << setprecision(2) << (session[highestIndex].vipPrice * session[highestIndex].vipTicketsSold + session[highestIndex].standardPrice * session[highestIndex].standardTicketsSold) << ")" << endl;
    outFile << "Lowest Sales              : " << session[lowestIndex].location << " ("
        << (session[lowestIndex].vipTicketsSold + session[lowestIndex].standardTicketsSold)
        << " tickets, RM " << fixed << setprecision(2) << (session[lowestIndex].vipPrice * session[lowestIndex].vipTicketsSold + session[lowestIndex].standardPrice * session[lowestIndex].standardTicketsSold) << ")" << endl;
    outFile << "\n--- Additional Insights ---" << endl;
    outFile << "VIP Tickets Sold        : " << soldVip << " (" << (static_cast<double>(soldVip) / totalVIPSeats) * 100 << "% occupancy)" << endl;
    outFile << "Standard Tickets Sold   : " << soldStandard << " (" << (static_cast<double>(soldStandard) / totalStandardSeats) * 100 << "% occupancy)" << endl;
    outFile << "Overall Occupancy      : " << fixed << setprecision(2) << (static_cast<double>(soldVip + soldStandard) / (totalVIPSeats + totalStandardSeats)) * 100.0 << "%" << endl;

    outFile.close();
    cout << "Report generated and saved to report.txt\n";
}

// a assistive function to calculate total merchandise income
double calculateMerchIncome(const vector<Merchandise>& merchandise) {
    double total = 0;

    for (int i = 0; i < merchandise.size(); i++) {
        int sold = merchandise[i].initialStock - merchandise[i].stock;
        total += merchandise[i].price * sold;
    }
    return total;
}