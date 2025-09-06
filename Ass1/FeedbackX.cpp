#include "FeedbackX.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <format>
using namespace std;

// Save feedback
void saveFeedbackX(const FeedbackX& fb) {
    ofstream fout("feedbackX.txt", ios::app);
    fout << fb.userID << "|" << fb.userName << "|" << fb.feedback << endl;
    fout.close();
}

// Load all feedback
vector<FeedbackX> loadAllFeedbackX() {
    vector<FeedbackX> allFb;
    ifstream fin("feedbackX.txt");
    string line;
    while (getline(fin, line)) {
        stringstream ss(line);
        FeedbackX temp;
        getline(ss, temp.userID, '|');
        getline(ss, temp.userName, '|');
        getline(ss, temp.feedback);
        allFb.push_back(temp);
    }
    return allFb;
}

// View all feedback
void viewAllFeedbackX() {
    vector<FeedbackX> allFb = loadAllFeedbackX();
    if (allFb.empty()) {
        cout << "No feedback found.\n";
        return;
    }
    cout << "\n--- All Feedback (FeedbackX) ---\n";
    for (const auto& fb : allFb) {
        cout << format("User ID: {:<8} | Name: {:<15} | Feedback: {}\n", fb.userID, fb.userName, fb.feedback);
    }
}
