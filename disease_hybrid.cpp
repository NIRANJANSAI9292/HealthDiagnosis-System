#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

class DiseasePredictor {
private:
    map<string, vector<string>> diseaseSymptoms;
    map<string, int> diseaseCount;
    map<string, vector<string>> doctorDetails;
    map<string, vector<string>> diseasePrecautions;
    map<string, vector<string>> diseaseTablets;
    map<string, vector<string>> diseaseReasons;
    int totalDiseases = 0;

public:
    void loadDataset(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening dataset!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string disease, symptom;
            getline(ss, disease, ',');

            while (getline(ss, symptom, ',')) {
                diseaseSymptoms[disease].push_back(symptom);
            }
            diseaseCount[disease]++;
            totalDiseases++;
        }
        file.close();
    }

    void loadDoctorData(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening doctor dataset!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string disease, doctor, hospital, address;
            getline(ss, disease, ',');
            getline(ss, doctor, ',');
            getline(ss, hospital, ',');
            getline(ss, address, ',');

            doctorDetails[disease] = {doctor, hospital, address};
        }
        file.close();
    }

    void loadPrecautionsData(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening precautions dataset!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string disease, precaution;
            getline(ss, disease, ',');

            while (getline(ss, precaution, ',')) {
                diseasePrecautions[disease].push_back(precaution);
            }
        }
        file.close();
    }

    void loadTabletsData(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening tablets dataset!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string disease, tablet;
            getline(ss, disease, ',');

            while (getline(ss, tablet, ',')) {
                diseaseTablets[disease].push_back(tablet);
            }
        }
        file.close();
    }

    void loadReasonsData(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening reasons dataset!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string disease, reason;
            getline(ss, disease, ',');

            while (getline(ss, reason, ',')) {
                diseaseReasons[disease].push_back(reason);
            }
        }
        file.close();
    }

    vector<string> getMatchingDiseases(vector<string> userSymptoms) {
        vector<string> possibleDiseases;
        for (auto &entry : diseaseSymptoms) {
            bool allMatch = true;
            for (string symptom : userSymptoms) {
                if (find(entry.second.begin(), entry.second.end(), symptom) == entry.second.end()) {
                    allMatch = false;
                    break;
                }
            }
            if (allMatch) {
                possibleDiseases.push_back(entry.first);
            }
        }
        return possibleDiseases;
    }
    string predictWithNaiveBayes(vector<string> userSymptoms) {
        string bestDisease = "Unknown";
        double bestProb = -1e9;

        for (auto &entry : diseaseSymptoms) {
            string disease = entry.first;
            double prob = log(static_cast<double>(diseaseCount[disease]) / totalDiseases);

            for (const string &symptom : userSymptoms) {
                if (find(diseaseSymptoms[disease].begin(), diseaseSymptoms[disease].end(), symptom) != diseaseSymptoms[disease].end()) {
                prob += log(1.0 / (static_cast<double>(diseaseSymptoms[disease].size()) + 1.0));
            }
        }
            if (prob > bestProb) {
                bestProb = prob;
                bestDisease = disease;
            }
        }
        return bestDisease;
    }

   
    string predictWithKNN(vector<string> userSymptoms, int k = 3) {
        vector<pair<int, string>> distances;

        for (auto &entry : diseaseSymptoms) {
            int matchCount = 0;
            for (const string &symptom : userSymptoms) {
                if (find(entry.second.begin(), entry.second.end(), symptom) != entry.second.end()) {
                    matchCount++;
                }
            }
            distances.emplace_back(matchCount, entry.first);
        }

        sort(distances.rbegin(), distances.rend());

        map<string, int> votes;
        for (size_t i = 0; i < min(static_cast<size_t>(k), distances.size()); i++) {
            votes[distances[i].second]++;
        }

        string bestDisease;
        int maxVotes = 0;
        for (auto &vote : votes) {
            if (vote.second > maxVotes) {
                maxVotes = vote.second;
                bestDisease = vote.first;
            }
        }

        return bestDisease;
    }
    string refinePrediction(vector<string> userSymptoms, vector<string> possibleDiseases = {}) {
        if (possibleDiseases.empty()) {
            possibleDiseases = getMatchingDiseases(userSymptoms);
        }

        if (possibleDiseases.empty()) {
            return "No matching disease found.";
        }

        if (possibleDiseases.size() == 1) {
            return possibleDiseases[0];
        }

        cout << "\nMore than one disease matches your symptoms:\n";
        for (string disease : possibleDiseases) {
            cout << "- " << disease << endl;
        }

        cout << "\nDo you want to enter more symptoms? (yes/no): ";
        string choice;
        cin >> choice;
        cin.ignore();

        if (choice == "yes") {
            cout << "Enter additional symptoms (comma-separated): ";
            string input, userSymptom;
            getline(cin, input);
            stringstream ss(input);
            
            string symptom;  // Declare symptom before using it
            while (getline(ss, symptom, ',')) {
                userSymptoms.push_back(symptom);
            }
            
            
            vector<string> newMatchingDiseases;
            for (string disease : possibleDiseases) {
                bool allMatch = true;
                for (const auto &symptom : userSymptoms) {

                    if (find(diseaseSymptoms[disease].begin(), diseaseSymptoms[disease].end(), symptom) == diseaseSymptoms[disease].end()) {
                        allMatch = false;
                        break;
                    }
                }
                if (allMatch) {
                    newMatchingDiseases.push_back(disease);
                }
            }

            return refinePrediction(userSymptoms, newMatchingDiseases);
        }

        return predictFinalDisease(userSymptoms, possibleDiseases);
    }

    string predictFinalDisease(vector<string> userSymptoms, vector<string> possibleDiseases = {}) {
        if (possibleDiseases.empty()) {
            possibleDiseases = getMatchingDiseases(userSymptoms);
        }

        if (possibleDiseases.size() == 1) {
            return possibleDiseases[0];
        }

        string nbResult = predictWithNaiveBayes(userSymptoms);
        string knnResult = predictWithKNN(userSymptoms);

        if (nbResult == knnResult) {
            return nbResult;
        }

        return refinePrediction(userSymptoms, possibleDiseases);
    }

    void recommendDoctor(string disease) {
        if (doctorDetails.find(disease) != doctorDetails.end()) {
            cout << "\nDoctor Recommendation for " << disease << ":\n";
            cout << "Doctor Name: " << doctorDetails[disease][0] << endl;
            cout << "Hospital: " << doctorDetails[disease][1] << endl;
            cout << "Address: " << doctorDetails[disease][2] << endl;
        } else {
            cout << "\nNo doctor data available for " << disease << "." << endl;
        }
    }

void showPrecautions(string disease) {
    if (diseasePrecautions.find(disease) != diseasePrecautions.end()) {
        cout << "\nPrecautions for " << disease << ":\n";
        for (const string &precaution : diseasePrecautions[disease]) {
            cout << "- " << precaution << endl;
        }
    } else {
        cout << "\nNo precautions available for " << disease << "." << endl;
    }
}
void showTablets(string disease) {
    if (diseaseTablets.find(disease) != diseaseTablets.end()) {
        cout << "\nRecommended Tablets for " << disease << ":\n";
        for (const string &tablet : diseaseTablets[disease]) {
            cout << "- " << tablet << endl;
        }
    } else {
        cout << "\nNo tablet recommendations available for " << disease << "." << endl;
    }
}
void showReasons(string disease) {
    if (diseaseReasons.find(disease) != diseaseReasons.end()) {
        cout << "\nPossible Reasons for " << disease << ":\n";
        for (const string &reason : diseaseReasons[disease]) {
            cout << "- " << reason << endl;
        }
    } else {
        cout << "\nNo reasons available for " << disease << "." << endl;
    }
}
};


int main() {
    DiseasePredictor predictor;
    predictor.loadDataset("disease_data.csv");
    predictor.loadDoctorData("doctors_data.csv");
    predictor.loadPrecautionsData("precautions_data.csv");
    predictor.loadTabletsData("tablets_data.csv");
    predictor.loadReasonsData("reasons_data.csv");
   

    vector<string> userSymptoms;
    string input, symptom;

    cout << "Enter symptoms (comma-separated): ";
    getline(cin, input);
    stringstream ss(input);
    while (getline(ss, symptom, ',')) {
        userSymptoms.push_back(symptom);
    }

    string predictedDisease = predictor.predictFinalDisease(userSymptoms);
    cout << "Predicted Disease: " << predictedDisease << endl;
    predictor.showPrecautions(predictedDisease);
    predictor.showTablets(predictedDisease);

    cout << "Do you want hospital and doctor prescription? (yes/no): ";
    string choice;
    cin >> choice;
    cin.ignore();

    if (choice == "yes") {
        predictor.recommendDoctor(predictedDisease);
    }
    cout << "Do you want to know the reasons for this disease? (yes/no): ";
    cin >> choice;
    cin.ignore();
    if (choice == "yes") {
        predictor.showReasons(predictedDisease);
    }
    

    return 0;

}
