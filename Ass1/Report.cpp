#include "Report.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

void viewReport(vector<Session> session, vector<Merchandise> merchandise) {
    int totalTickets = 0;

    int highestIndex = 0;
    int highestTicket = session[0].vipTicketsSold + session[0].standardTicketsSold;
    int lowestIndex = 0;
    int lowestTicket = session[0].vipTicketsSold + session[0].standardTicketsSold;

    double ticketIncome = 0;
    double merchandiseIncome = calculateMerchIncome(merchandise);
    int overallIncome = 0;

    cout << "===== Concert Ticket Sales Summary =====" << endl;
    for (int i = 0; i < session.size(); i++) {
        if ((session[i].vipTicketsSold + session[i].standardTicketsSold) > highestTicket) {
            highestTicket = session[i].vipTicketsSold + session[i].standardTicketsSold;
            highestIndex = i;
        }
        else if (session[i].vipTicketsSold + session[i].standardTicketsSold < lowestTicket) {
            lowestTicket = session[i].vipTicketsSold + session[i].standardTicketsSold;
            lowestIndex = i;
        }

        totalTickets += session[i].vipTicketsSold + session[i].standardTicketsSold;
        ticketIncome += session[i].vipPrice * session[i].vipTicketsSold + session[i].standardPrice * session[i].standardTicketsSold;
        cout << session[i].location << " (" << session[i].sessionID << ")" << ": " << (session[i].vipTicketsSold + session[i].standardTicketsSold) << endl;
    }
    cout << "----------------------------------------" << endl;
    cout << "Total Tickets Sold        : " << totalTickets << endl;
    cout << "Total Income (Ticket)     : RM " << ticketIncome << endl;
    cout << "Total Income (Merchandise): RM " << merchandiseIncome << endl;
    cout << "Overall Income            : RM " << overallIncome << endl;
    cout << "Average Tickets           : " << (totalTickets / session.size()) << endl;
    cout << "Average Revenue           : RM " << (overallIncome / session.size()) << endl;
    cout << "Highest Sales             : " << session[highestIndex].location << " (" << (session[highestIndex].vipTicketsSold + session[highestIndex].standardTicketsSold) <<
        " tickets, RM " << (session[highestIndex].vipPrice * session[highestIndex].vipTicketsSold + session[highestIndex].standardPrice * session[highestIndex].standardTicketsSold) << ")" << endl;
    cout << "Lowest Sales              : " << session[lowestIndex].location << " (" << (session[lowestIndex].vipTicketsSold + session[lowestIndex].standardTicketsSold) <<
        " tickets, RM " << (session[lowestIndex].vipPrice * session[lowestIndex].vipTicketsSold + session[lowestIndex].standardPrice * session[highestIndex].standardTicketsSold) << ")" << endl;
    cout << "========================================" << endl;
}

void writeReportToFile(vector<Session> session, vector<Merchandise> merchandise) {
    ofstream outFile("report.txt"); // write in

    if (!outFile) {
        cout << "Error opening report file.\n";
        return;
    }

    int totalTickets = 0;
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

        totalTickets += session[i].vipTicketsSold + session[i].standardTicketsSold;
        ticketIncome += session[i].vipPrice * session[i].vipTicketsSold + session[i].standardPrice * session[i].standardTicketsSold;
        outFile << session[i].location << " (" << session[i].sessionID << ")"
            << ": " << (session[i].vipTicketsSold + session[i].standardTicketsSold) << endl;
    }

    overallIncome = ticketIncome + merchandiseIncome;

    outFile << "----------------------------------------" << endl;
    outFile << "Total Tickets Sold        : " << totalTickets << endl;
    outFile << "Total Income (Ticket)     : RM " << ticketIncome << endl;
    outFile << "Total Income (Merchandise): RM " << merchandiseIncome << endl;
    outFile << "Overall Income            : RM " << overallIncome << endl;
    outFile << "Average Tickets           : " << (totalTickets / session.size()) << endl;
    outFile << "Average Revenue           : RM " << (overallIncome / session.size()) << endl;
    outFile << "Highest Sales             : " << session[highestIndex].location << " ("
        << (session[highestIndex].vipTicketsSold + session[highestIndex].standardTicketsSold)
        << " tickets, RM "
        << (session[highestIndex].vipPrice * session[highestIndex].vipTicketsSold + session[highestIndex].standardPrice * session[highestIndex].standardTicketsSold) << ")" << endl;
    outFile << "Lowest Sales              : " << session[lowestIndex].location << " ("
        << (session[lowestIndex].vipTicketsSold + session[lowestIndex].standardTicketsSold)
        << " tickets, RM "
        << (session[lowestIndex].vipPrice * session[lowestIndex].vipTicketsSold + session[lowestIndex].standardPrice * session[lowestIndex].standardTicketsSold) << ")" << endl;
    outFile << "========================================" << endl;

    outFile.close();
    cout << "Report generated and saved to report.txt\n";
}

double calculateMerchIncome(vector<Merchandise> merchandise) {
    double total = 0;

    for (int i = 0; i < merchandise.size(); i++) {
        int sold = merchandise[i].initialStock - merchandise[i].stock;
        total += merchandise[i].price * sold;
    }

    return total;
}