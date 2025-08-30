#pragma once

#include <string>
#include <vector>

struct Session;
struct Merchandise;

struct User {
    std::string userID;
    std::string name;
    std::string phone;
    std::string email;
    std::string password;
    bool isBlocked = false;
    std::string bookingInfo = "None";
};

// Helper
bool isValidName(const std::string& name);
bool isValidPhone(const std::string& phone);
bool isValidEmail(const std::string& email);
bool isValidPassword(const std::string& pw);

// User management
void saveUsersToFile(const std::vector<User>& users);
std::vector<User> loadUsersFromFile();
std::string generateUserID(int count);
std::string getMaskedPassword();
int findUserIndex(const std::vector<User>& users, const std::string& id);

// Sorting users
bool compareByID(const User& a, const User& b);
bool compareByName(const User& a, const User& b);

// Core features
void registerUser(std::vector<User>& users);
void editUserInfo(int userIndex, std::vector<User>& users);
void userMenu(int userIndex, std::vector<User>& users, std::vector<Session>& sessions, std::vector<Merchandise> merchandise);