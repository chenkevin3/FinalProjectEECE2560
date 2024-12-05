#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip> // For fixed and setprecision
#include <algorithm> // For sorting

using namespace std;


// Constants
const string filename = "players_data.csv";

struct Player {
    string name;
    int id;
    int matchesPlayed;
    int matchesWon;
    double winRate;
    int kills, deaths, assists;
    double kda;
    int rankedScore;

    Player(string playerName, int playerId, int matches, int wins, int playerKills = 0, int playerDeaths = 0, int playerAssists = 0, int rankScore = 0)
        : name(playerName), id(playerId), matchesPlayed(matches), matchesWon(wins),
          kills(playerKills), deaths(playerDeaths), assists(playerAssists), rankedScore(rankScore) {
        winRate = (matches > 0) ? (static_cast<double>(wins) / matches) * 100 : 0.0;
        kda = (deaths > 0) ? static_cast<double>(kills + assists) / deaths : kills + assists;
    }

    double averageKda() const {
        return matchesPlayed > 0 ? kda / matchesPlayed : 0.0;
    }
};




// Save player data to file
void saveDataToFile(const vector<Player>& players, const string& filename) {
    ofstream file(filename);
    if (file.is_open()) {
        for (const auto& player : players) {
            file << player.name << "," << player.id << "," << player.matchesPlayed << "," << player.matchesWon << "," << fixed << setprecision(2) << player.winRate << "\n";
        }
        file.close();
    }
}

void loadDataFromFile(vector<Player>& players, const string& filename) {
    ifstream file(filename);
    if (file.is_open()) {
        string line;
        players.clear();
        while (getline(file, line)) {
            stringstream ss(line);
            string name;
            int id, matchesPlayed, matchesWon;
            double winRate;
            char delimiter;

            // Parse line
            getline(ss, name, ',');
            ss >> id >> delimiter >> matchesPlayed >> delimiter >> matchesWon >> delimiter >> winRate;

            // Create player and add to list
            players.push_back(Player(name, id, matchesPlayed, matchesWon));
        }
        file.close();
    }
}


// Display menu
void displayMenu() {
    cout << "\nTournament Ranking System\n";
    cout << "1. View Rankings (By Win Rate)\n";
    cout << "2. Add Player\n";
    cout << "3. Record Match Result\n";
    cout << "4. Save Rankings\n";
    cout << "5. Load Rankings\n";
    cout << "6. Exit\n";
    cout << "Choose an option: ";
}

// Forward declarations of comparator functions
bool compareByWinRate(const Player& a, const Player& b);
bool compareByRankedScore(const Player& a, const Player& b);
bool compareByAverageKDA(const Player& a, const Player& b);

// Function to update match results
void updateMatch(vector<Player> &players, int player1Index, int player2Index, bool p1Won, bool p2Won,
                 int p1Kills, int p1Deaths, int p1Assists, int p1RankChange,
                 int p2Kills, int p2Deaths, int p2Assists, int p2RankChange) {
    // Update stats for Player 1
    players[player1Index].matchesPlayed++;
    players[player1Index].kills += p1Kills;
    players[player1Index].deaths += p1Deaths;
    players[player1Index].assists += p1Assists;
    players[player1Index].rankedScore += p1RankChange;
    players[player1Index].kda = (players[player1Index].deaths > 0)
        ? static_cast<double>(players[player1Index].kills + players[player1Index].assists) / players[player1Index].deaths
        : players[player1Index].kills + players[player1Index].assists;

    if (p1Won) players[player1Index].matchesWon++;

    // Update stats for Player 2
    players[player2Index].matchesPlayed++;
    players[player2Index].kills += p2Kills;
    players[player2Index].deaths += p2Deaths;
    players[player2Index].assists += p2Assists;
    players[player2Index].rankedScore += p2RankChange;
    players[player2Index].kda = (players[player2Index].deaths > 0)
        ? static_cast<double>(players[player2Index].kills + players[player2Index].assists) / players[player2Index].deaths
        : players[player2Index].kills + players[player2Index].assists;

    if (p2Won) players[player2Index].matchesWon++;

    // Recalculate win rates
    players[player1Index].winRate = (players[player1Index].matchesPlayed > 0)
        ? static_cast<double>(players[player1Index].matchesWon) / players[player1Index].matchesPlayed * 100
        : 0.0;

    players[player2Index].winRate = (players[player2Index].matchesPlayed > 0)
        ? static_cast<double>(players[player2Index].matchesWon) / players[player2Index].matchesPlayed * 100
        : 0.0;

    // Sort players by win rate after the update
    sort(players.begin(), players.end(), compareByWinRate);
}


void displayPlayers(const vector<Player>& players) {
    cout << left << setw(5) << "ID" 
         << setw(15) << "Name" 
         << setw(10) << "Matches" 
         << setw(10) << "Wins" 
         << setw(12) << "WinRate(%)" 
         << setw(8) << "Kills" 
         << setw(8) << "Deaths" 
         << setw(8) << "Assists" 
         << setw(8) << "KDA" 
         << setw(10) << "AvgKDA" 
         << setw(12) << "RankedScore" 
         << endl;

    cout << string(100, '-') << endl; // Add a separator line for clarity

    for (const auto& player : players) {
        cout << left << setw(5) << player.id 
             << setw(15) << player.name 
             << setw(10) << player.matchesPlayed 
             << setw(10) << player.matchesWon 
             << setw(12) << fixed << setprecision(2) << player.winRate 
             << setw(8) << player.kills 
             << setw(8) << player.deaths 
             << setw(8) << player.assists 
             << setw(8) << fixed << setprecision(2) << player.kda 
             << setw(10) << fixed << setprecision(2) << player.averageKda() 
             << setw(12) << player.rankedScore 
             << endl;
    }
}


bool compareByWinRate(const Player& a, const Player& b) {
    return a.winRate > b.winRate;
}

bool compareByRankedScore(const Player& a, const Player& b) {
    return a.rankedScore > b.rankedScore;
}

bool compareByAverageKDA(const Player& a, const Player& b) {
    return a.averageKda() > b.averageKda();
}

// Replace lambdas with these named functions
void sortPlayers(vector<Player>& players) {
    int choice;
    cout << "Choose a sorting criterion:\n"
         << "1. Win Rate\n"
         << "2. Ranked Score\n"
         << "3. Average KDA\n";
    cin >> choice;

    switch (choice) {
        case 1:
            sort(players.begin(), players.end(), compareByWinRate);
            break;
        case 2:
            sort(players.begin(), players.end(), compareByRankedScore);
            break;
        case 3:
            sort(players.begin(), players.end(), compareByAverageKDA);
            break;
        default:
            cout << "Invalid choice. Sorting by default (Win Rate).\n";
            sort(players.begin(), players.end(), compareByWinRate);
    }
}


// Find player by ID
int findPlayerIndex(const vector<Player>& players, int playerId) {
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].id == playerId)
            return i;
    }
    return -1;
}

int main() {
    vector<Player> players;
    players.push_back(Player("Alice", 1, 10, 7, 50, 10, 15, 1000));
    players.push_back(Player("Bob", 2, 15, 10, 60, 5, 10, 900));
    players.push_back(Player("Charlie", 3, 12, 8, 40, 12, 20, 800));
    players.push_back(Player("Dave", 4, 8, 4, 20, 8, 12, 700));


    cout << "Initial Player List:\n";
    displayPlayers(players);

    sortPlayers(players);

    cout << "\nSorted Player List:\n";
    displayPlayers(players);

    int choice;

    // Start the main menu loop
    while (true) {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1: {
                sort(players.begin(), players.end(), compareByWinRate);
                cout << "\nCurrent Rankings (By Win Rate):\n";
                cout << "Name\tID\tMatches Played\tMatches Won\tWin Rate (%)\n";
                for (const auto& player : players) {
                    cout << player.name << "\t" << player.id << "\t" << player.matchesPlayed
                         << "\t\t" << player.matchesWon << "\t\t" << fixed << setprecision(2) << player.winRate << "\n";
                }
                break;
            }

            case 2: {
                string name;
                int id;
                cout << "Enter player name: ";
                cin >> name;
                cout << "Enter player ID: ";
                cin >> id;

                // Check for duplicate ID
                if (findPlayerIndex(players, id) != -1) {
                    cout << "Error: Player ID already exists. Try again.\n";
                    break;
                }

                players.push_back(Player(name, id, 0, 0));
                cout << "Player added.\n";
                break;
            }

            case 3: {
                int p1, p2, p1Kills, p1Deaths, p1Assists, p2Kills, p2Deaths, p2Assists, p1RankChange, p2RankChange;
                char result;
            
                cout << "Enter index of Player 1: ";
                cin >> p1;
                cout << "Enter index of Player 2: ";
                cin >> p2;
            
                cout << "Did Player 1 win? (y/n): ";
                cin >> result;
                bool p1Won = (result == 'y');
                bool p2Won = !p1Won;
            
                cout << "Enter Player 1 Kills, Deaths, Assists, and Rank Change: ";
                cin >> p1Kills >> p1Deaths >> p1Assists >> p1RankChange;
            
                cout << "Enter Player 2 Kills, Deaths, Assists, and Rank Change: ";
                cin >> p2Kills >> p2Deaths >> p2Assists >> p2RankChange;
            
                updateMatch(players, p1, p2, p1Won, p2Won, p1Kills, p1Deaths, p1Assists, p1RankChange,
                            p2Kills, p2Deaths, p2Assists, p2RankChange);
            
                cout << "Match updated and rankings adjusted.\n";
                break;
            }

            case 4:
                saveDataToFile(players, filename);
                cout << "Rankings saved to file.\n";
                break;

            case 5:
                loadDataFromFile(players, filename);
                cout << "Rankings loaded from file.\n";
                break;

            case 6:
                cout << "Exiting...\n";
                saveDataToFile(players, filename); // Auto-save before exit
                return 0;

            default:
                cout << "Invalid option. Please try again.\n";
        }
    }
}
