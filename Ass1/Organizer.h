#pragma once

#include <string>
#include <vector>

#include "User.h"
#include "SessionBooking.h"
#include "Ticketing.h"

// Defines the credentials for the system organizer.
struct SystemCredentials {
    const std::string organizerID = "11";
    const std::string organizerPW = "11";
};

bool confirmOrganizerPassword(const SystemCredentials& creds);
void organizerMenu(std::vector<User>& users, const SystemCredentials& creds, std::vector<Session> session, std::vector<Merchandise> merchandise);
void viewAllUsers(std::vector<User>& users, const SystemCredentials& creds);
void printUserList(const std::vector<User>& users, const SystemCredentials& creds);