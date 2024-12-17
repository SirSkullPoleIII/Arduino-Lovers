#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <list>
using namespace std;

void homeArduinoSetup();
void clientArduinoSetup();
void writeToFile(string arduinoType, list<string> SSIDList, list<string> passwordList, string i2cAdress, string port);


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

void homeArduinoSetup() {
    string arduinoType = "SERVER";

    list<string> SSIDList;
    list<string> passwordList;

    string SSIDPassword;
    string i2cAddress;
    string port;
    bool inputting = true;
    cout << "Please input your SSID (Name of your WiFi network) and password split by (, )" << endl;
    cout << "You may add as many Networks as you would like to, end with an empty line." << endl;

    while (inputting) {
        getline(cin, SSIDPassword);
        if (SSIDPassword.empty()) {
            break;
        } else {
            vector<string> credentials = splitString(SSIDPassword);
            
            if (credentials.size() == 2) { 

                for (auto& cred : credentials) {
                    cred.erase(cred.begin(), find_if(cred.begin(), cred.end(), [](unsigned char ch) {
                        return !isspace(ch);
                    }));
                    cred.erase(find_if(cred.rbegin(), cred.rend(), [](unsigned char ch) {
                        return !isspace(ch);
                    }).base(), cred.end());
                }

                SSIDList.push_back(credentials[0]);
                passwordList.push_back(credentials[1]);

            } else {
                cout << "Invalid input. Please input SSID and password separated by a comma." << endl;
            }
        }
    }
    cout << "Please enter port that you have forwared > ";
    getline(cin, port);
    cout << "Please enter the I2C address of your LCD > ";
    getline(cin, i2cAddress);
    writeToFile(arduinoType, SSIDList, passwordList, i2cAddress, port);
}

void clientArduinoSetup(){
    cout << "Client arduino" << endl;
}

void writeToFile(string arduinoType, list<string> SSIDList, list<string> passwordList, string i2cAdress, string port) {
    if (arduinoType == "SERVER") {
        ofstream serverFile("F:/Arduino-Lovers/Home_Arduino_Server/serverConfig.h");

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
            serverFile << "const int port = " << port << ";" << endl;
            serverFile << "#define LCD_ADDR " << i2cAdress<< ";" << endl;

            serverFile << "#endif" << endl;
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