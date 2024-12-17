#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <list>
using namespace std;

void homeArduinoSetup();
void clientArduinoSetup();
void writeToFile();


void chooseArduino() {
    bool chosen = false;
    while (!chosen){
        cout << "Which arduino will you configure (Server/Client) >";
        string arduino;
        cin >> arduino;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');


        transform(arduino.begin(), arduino.end(), arduino.begin(),
        ::toupper);

        if (arduino == "SERVER"){
            chosen = true;
            homeArduinoSetup();
        }else if (arduino == "CLIENT"){
            chosen = true;
            clientArduinoSetup();
        }else{
            cout << "Invalid response please input either Server or Client to contiue!" << endl;
            chosen = false;
        }
    }
}

vector<string> splitString(const string& input){
    vector<string> result;
    stringstream ss(input);
    string token;

    while(getline(ss, token, ',')){
        result.push_back(token);
    }
    return result;
}

string vectorToString(const std::vector<std::string>& vec) {
    std::string result;
    for (const auto& str : vec) {
        result += str;  // Concatenate each string without separators
    }
    return result;
}

void homeArduinoSetup(){
    string arduinoType = "SERVER";

    vector<string> SSID;
    vector<string> password;

    list<string> SSIDList;
    list<string> passwordList;
    
    string SSIDPassword;
    bool inputting = true;
    cout << "Please input your SSID (Name of your WiFi network) and password split by (, )" << endl;
    cout << "You may add as many Networks as you would like to end double tap *ENTER*" << endl;

    while (inputting){
        getline(cin, SSIDPassword);
        if (SSIDPassword.empty()){
            break;
        }else{
            vector<string> credentials = splitString(SSIDPassword);
            if (credentials.size() == 2){
            
                SSID.push_back(credentials[0]);
                password.push_back(credentials[1]);
            }else{
                cout << "Invalid input." << endl;
            }
            
            string SSIDString = vectorToString(SSID);
            string passwordString = vectorToString(password);
            
            SSIDList.push_back(SSIDString);

            passwordString.erase(passwordString.begin(), std::find_if(passwordString.begin(), passwordString.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));

            passwordList.push_back(passwordString);
            cout << SSIDString << endl;
            cout << passwordString << endl;

        }
    }
    writeToFile(arduinoType, SSIDList, passwordList);
}

void clientArduinoSetup(){
    cout << "Client arduino" << endl;
}

void writeToFile(string arduinoType, list<string> SSIDList, list<string> passwordList) {
    if (arduinoType == "SERVER") {
        ofstream serverFile("serverConfig.h");

        if (serverFile.is_open()) {
            serverFile << "#ifndef SERVERCONFIG_H\n#define SERVERCONFIG_H\n";

            // Write SSID array
            serverFile << "const char* ssid[] = {";
            for (auto it = SSIDList.begin(); it != SSIDList.end(); ++it) {
                serverFile << "\"" << *it << "\"";
                if (next(it) != SSIDList.end()) {
                    serverFile << ", ";
                }
            }
            serverFile << "};\n";

            // Write password array
            serverFile << "const char* password[] = {";
            for (auto it = passwordList.begin(); it != passwordList.end(); ++it) {
                serverFile << "\"" << *it << "\"";
                if (next(it) != passwordList.end()) {
                    serverFile << ", ";
                }
            }
            serverFile << "};\n";

            serverFile << "#endif // SERVERCONFIG_H\n";
            serverFile.close();
            cout << "Server configuration written to serverConfig.h" << endl;
        } else {
            cout << "Error opening file!" << endl;
        }
    }
}

int main() {
    chooseArduino();
    return 0;
}