#pragma once
#include <string>
#include <vector>

struct FeedbackX {
    std::string feedbackID;   // unique id (e.g., FB0001)
    std::string userID;       // who submitted
    std::string userName;     // display name
    std::string feedback;     // message text
    std::string timestamp;    // yyyy-mm-dd HH:MM:SS
};

// core API
void saveFeedbackX(const FeedbackX& fb);
std::vector<FeedbackX> loadAllFeedbackX();
void viewAllFeedbackX();
bool removeFeedbackX(const std::string& feedbackID);

// helpers
std::string generateFeedbackID();
std::string getCurrentTimestamp();
