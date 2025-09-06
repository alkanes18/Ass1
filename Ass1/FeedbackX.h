#pragma once
#pragma once
#include <vector>
#include <string>
using namespace std;

struct FeedbackX {
    string userID;
    string userName;
    string feedback;
};

// Functions
void saveFeedbackX(const FeedbackX& fb);       // Save feedback to file
vector<FeedbackX> loadAllFeedbackX();         // Load all feedback from file
void viewAllFeedbackX();                       // Display all feedback