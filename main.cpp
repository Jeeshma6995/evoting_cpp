#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

const string VOTER_FILE = "voters.txt";
const string VOTE_FILE = "votes.txt";

const string ADMIN_ID = "admin";
const string ADMIN_PASSWORD = "admin123";

const int CANDIDATE_COUNT = 3;

struct Vote {
    string voterID;
    int candidateID;
};

// Class 1: VotingSystem
class VotingSystem {
public:
    string candidates[CANDIDATE_COUNT] = {
        "Candidate A",
        "Candidate B",
        "Candidate C"
    };

    // Create vote file if it does not exist.
    bool initializeFiles() {
        ifstream voteCheck(VOTE_FILE);

        if (!voteCheck) {
            ofstream voteFile(VOTE_FILE);

            if (!voteFile) {
                cout << "Error creating vote file!\n";
                return false;
            }

            voteFile.close();
        }

        return true;
    }

    // Load and validate saved voting records.
    bool loadVotes(Vote votes[], int &count) {
        ifstream file(VOTE_FILE);
        string line;

        count = 0;

        if (!file) {
            return false;
        }

        while (getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            istringstream record(line);
            Vote vote;
            string extra;

            if (!(record >> vote.voterID >> vote.candidateID)
                || (record >> extra)) {
                return false;
            }

            if (vote.voterID.empty()
                || vote.candidateID < 1
                || vote.candidateID > CANDIDATE_COUNT
                || count >= 1000) {
                return false;
            }

            // Reject duplicate voter IDs.
            for (int i = 0; i < count; i++) {
                if (votes[i].voterID == vote.voterID) {
                    return false;
                }
            }

            votes[count] = vote;
            count++;
        }

        return !file.bad();
    }

    // Check whether the voter has already voted.
    bool hasVoted(string id, Vote votes[], int count) {
        for (int i = 0; i < count; i++) {
            if (votes[i].voterID == id) {
                return true;
            }
        }

        return false;
    }

    // Display candidates.
    void displayCandidates() {
        cout << "\n--- CANDIDATES ---\n";

        for (int i = 0; i < CANDIDATE_COUNT; i++) {
            cout << i + 1 << ". "
                 << candidates[i] << "\n";
        }
    }

    // Display election results.
    void displayResults() {
        Vote votes[1000];
        int count = 0;
        int totals[CANDIDATE_COUNT] = {0};

        if (!loadVotes(votes, count)) {
            cout << "Vote file error! Results cannot be trusted.\n";
            return;
        }

        for (int i = 0; i < count; i++) {
            totals[votes[i].candidateID - 1]++;
        }

        cout << "\n===== ELECTION RESULTS =====\n";

        for (int i = 0; i < CANDIDATE_COUNT; i++) {
            cout << candidates[i] << ": "
                 << totals[i] << " votes\n";
        }

        int maxVotes = 0;

        for (int i = 0; i < CANDIDATE_COUNT; i++) {
            if (totals[i] > maxVotes) {
                maxVotes = totals[i];
            }
        }

        if (maxVotes == 0) {
            cout << "No votes have been cast yet.\n";
            return;
        }

        int winners = 0;

        for (int i = 0; i < CANDIDATE_COUNT; i++) {
            if (totals[i] == maxVotes) {
                winners++;
            }
        }

        if (winners > 1) {
            cout << "The election is tied!\n";
        } else {
            for (int i = 0; i < CANDIDATE_COUNT; i++) {
                if (totals[i] == maxVotes) {
                    cout << "Winner: "
                         << candidates[i] << "\n";
                }
            }
        }
    }
};


// Class 2: Voter
class Voter {
private:
    VotingSystem &system;

public:
    // Connect this voter object to the voting system.
    Voter(VotingSystem &s) : system(s) {}

    // Authenticate voter.
    bool authenticateVoter(string id, string password) {
        ifstream file(VOTER_FILE);
        string storedID, storedPassword;

        if (!file) {
            return false;
        }

        while (file >> storedID >> storedPassword) {
            if (storedID == id && storedPassword == password) {
                return true;
            }
        }

        return false;
    }

    // Check whether voter ID exists.
    bool voterExists(string id) {
        ifstream voterCheck(VOTER_FILE);

        if (!voterCheck) {
            ofstream voterFile(VOTER_FILE);

            if (!voterFile) {
                cout << "Error creating voter file!\n";
                return false;
            }

            voterFile << "V101 pass101\n";
            voterFile << "V102 pass102\n";
            voterFile << "V103 pass103\n";
            voterFile << "V104 pass104\n";

            voterFile.close();
        }

        ifstream file(VOTER_FILE);
        string storedID, storedPassword;

        if (!file) {
            return true;
        }

        while (file >> storedID >> storedPassword) {
            if (storedID == id) {
                return true;
            }
        }

        return false;
    }

    // Add a new voter.
    void addVoter() {
        string id, password;

        cout << "\nEnter new Voter ID: ";
        getline(cin, id);

        cout << "Enter new Voter Password: ";
        getline(cin, password);

        if (id.empty() || password.empty()) {
            cout << "Voter ID and password cannot be empty!\n";
            return;
        }

        if (voterExists(id)) {
            cout << "Voter ID already exists!\n";
            return;
        }

        ofstream file(VOTER_FILE, ios::app);

        if (!file) {
            cout << "Unable to save voter details!\n";
            return;
        }

        file << id << " " << password << "\n";
        file.close();

        if (!file) {
            cout << "Error saving voter details!\n";
            return;
        }

        cout << "New voter registered successfully!\n";
    }

    // Voter login and voting.
    void voterLogin() {
        string id, password;

        cout << "\nEnter Voter ID: ";
        getline(cin, id);

        cout << "Enter Password: ";
        getline(cin, password);

        if (!authenticateVoter(id, password)) {
            cout << "Invalid voter ID or password!\n";
            return;
        }

        Vote votes[1000];
        int count = 0;

        if (!system.loadVotes(votes, count)) {
            cout << "Vote file error! Voting is disabled.\n";
            return;
        }

        if (system.hasVoted(id, votes, count)) {
            cout << "You have already voted!\n";
            return;
        }

        system.displayCandidates();

        cout << "Enter candidate number: ";
        string choice;
        getline(cin, choice);

        if (choice != "1" && choice != "2" && choice != "3") {
            cout << "Invalid candidate choice!\n";
            return;
        }

        int candidateID = choice[0] - '0';

        ofstream file(VOTE_FILE, ios::app);

        if (!file) {
            cout << "Unable to save vote!\n";
            return;
        }

        file << id << " " << candidateID << "\n";
        file.close();

        if (!file) {
            cout << "Error saving vote. Check the vote file.\n";
            return;
        }

        cout << "Vote recorded successfully!\n";
    }
};


// Class 3: Admin
class Admin {
private:
    VotingSystem &system;
    Voter &voter;

public:
    // Connect the admin to the voting system and voter class.
    Admin(VotingSystem &s, Voter &v)
        : system(s), voter(v) {}

    // Admin login, voter registration, and results.
    void adminLogin() {
        string id, password;

        cout << "\nEnter Admin ID: ";
        getline(cin, id);

        cout << "Enter Admin Password: ";
        getline(cin, password);

        if (id != ADMIN_ID || password != ADMIN_PASSWORD) {
            cout << "Invalid admin credentials!\n";
            return;
        }

        cout << "\n--- ADMIN MENU ---\n";
        cout << "1. Add New Voter\n";
        cout << "2. View Election Results\n";
        cout << "3. Logout\n";
        cout << "Enter your choice: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            voter.addVoter();
            return;
        }

        if (choice == "3") {
            cout << "Admin logged out successfully!\n";
            return;
        }

        if (choice != "2") {
            cout << "Invalid admin menu choice!\n";
            return;
        }

        system.displayResults();
    }
};


// Main function
int main() {
    VotingSystem system;

    if (!system.initializeFiles()) {
        return 1;
    }

    Voter voter(system);
    Admin admin(system, voter);

    string choice;

    while (true) {
        cout << "\n============================\n";
        cout << "      E-VOTING SYSTEM\n";
        cout << "============================\n";
        cout << "1. Voter Login\n";
        cout << "2. Admin Login / Results\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";

        getline(cin, choice);

        if (choice == "1") {
            voter.voterLogin();
        } else if (choice == "2") {
            admin.adminLogin();
        } else if (choice == "3") {
            cout << "Thank you for using E-Voting System!\n";
            break;
        } else {
            cout << "Invalid menu choice!\n";
        }
    }

    return 0;
}