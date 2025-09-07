#include "FeedbackX.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

// scan file once to determine next numeric id, then cache it
static int& nextFeedbackNumRef() {
    static bool initialized = false;
    static int nextNum = 1;

    if (!initialized) {
        std::ifstream fin("feedbackX.txt");
        std::string line;
        int maxNum = 0;
        while (std::getline(fin, line)) {
            std::stringstream ss(line);
            std::string id;
            if (!std::getline(ss, id, '|')) continue; // id field
            // id format FB#### — extract digits
            if (id.size() >= 3 && id.rfind("FB", 0) == 0) {
                int n = 0;
                try {
                    n = std::stoi(id.substr(2));
                    if (n > maxNum) maxNum = n;
                }
                catch (...) { /* ignore malformed lines */ }
            }
        }
        nextNum = maxNum + 1;
        initialized = true;
    }
    return nextNum;
}

std::string generateFeedbackID() {
    int& n = nextFeedbackNumRef();
    std::ostringstream os;
    os << "FB" << std::setw(4) << std::setfill('0') << n++;
    return os.str();
}

std::string getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buf);
}

void saveFeedbackX(const FeedbackX& fbIn) {
    // make a copy so we can auto-fill missing fields without changing caller's object
    FeedbackX fb = fbIn;
    if (fb.feedbackID.empty())  fb.feedbackID = generateFeedbackID();
    if (fb.timestamp.empty())   fb.timestamp = getCurrentTimestamp();

    std::ofstream fout("feedbackX.txt", std::ios::app);
    if (!fout) {
        std::cerr << "Failed to open feedbackX.txt for writing\n";
        return;
    }
    // format: feedbackID|userID|userName|feedback|timestamp
    fout << fb.feedbackID << '|'
        << fb.userID << '|'
        << fb.userName << '|'
        << fb.feedback << '|'
        << fb.timestamp << '\n';
}

std::vector<FeedbackX> loadAllFeedbackX() {
    std::vector<FeedbackX> all;
    std::ifstream fin("feedbackX.txt");
    if (!fin) return all;

    std::string line;
    while (std::getline(fin, line)) {
        std::stringstream ss(line);
        FeedbackX fb;
        // read first 4 fields with delimiter; final field can read to end
        std::getline(ss, fb.feedbackID, '|');
        std::getline(ss, fb.userID, '|');
        std::getline(ss, fb.userName, '|');
        std::getline(ss, fb.feedback, '|');
        std::getline(ss, fb.timestamp); // to end-of-line
        if (!fb.feedbackID.empty()) all.push_back(fb);
    }
    return all;
}

void viewAllFeedbackX() {
    auto all = loadAllFeedbackX();
    if (all.empty()) {
        std::cout << "No feedback available.\n";
        return;
    }
    std::cout << "\n--- All Feedback ---\n";
    for (const auto& fb : all) {
        std::cout << "ID: " << fb.feedbackID
            << " | User: " << fb.userName
            << " (" << fb.userID << ")\n";
        std::cout << "Feedback: " << fb.feedback << '\n';
        std::cout << "Time: " << fb.timestamp << '\n';
        std::cout << "-----------------------------\n";
    }
}

bool removeFeedbackX(const std::string& feedbackID) {
    auto all = loadAllFeedbackX();
    bool removed = false;

    // filter out the one to remove
    std::vector<FeedbackX> kept;
    kept.reserve(all.size());
    for (const auto& fb : all) {
        if (fb.feedbackID == feedbackID) {
            removed = true;
            continue;
        }
        kept.push_back(fb);
    }

    if (!removed) {
        std::cout << "Feedback ID " << feedbackID << " not found.\n";
        return false;
    }

    // rewrite file
    std::ofstream fout("feedbackX.txt", std::ios::trunc);
    if (!fout) {
        std::cerr << "Failed to rewrite feedbackX.txt\n";
        return false;
    }
    for (const auto& fb : kept) {
        fout << fb.feedbackID << '|'
            << fb.userID << '|'
            << fb.userName << '|'
            << fb.feedback << '|'
            << fb.timestamp << '\n';
    }
    std::cout << "Feedback " << feedbackID << " removed successfully.\n";
    return true;
}
