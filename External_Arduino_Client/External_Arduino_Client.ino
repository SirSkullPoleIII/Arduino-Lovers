#include <WiFiS3.h>
#include <Wire.h>
#include "ExternalSecrets.h"

#define LCD_ADDR 0x3E

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi!");
  Wire.begin();
  lcdInit();
  
  
  delay(2000);
  lcdCommand(0x80);
  lcdCommand(0x01);
  Serial.println("cleared screen");

  getPubIP();


}

void getPubIP(){
  WiFiClient pubClient;
  if (pubClient.connect("api.ipify.org", 80)) {
    pubClient.println("GET /?format-text HTTP/1.1");
    pubClient.println("Host: api.ipify.org");
    pubClient.println("Connection: close");
    pubClient.println();

    String pubip = "";
    while (pubClient.connected() || pubClient.available()) {
      if (pubClient.available()) {
        char c = pubClient.read();
        pubip += c;
      }
    }
    pubClient.stop();
    //Serial.println("Public IP: " + pubip);
    processPubIP(pubip);
  }else{
    Serial.println("Pub IP connection failed");
  }
}

void processPubIP(String pubip) {
  int ipStart = pubip.indexOf("\r\n\r\n") +4;
  if (ipStart > 4) {
    String pubIPString = pubip.substring(ipStart);
    pubIPString.trim();
    Serial.println("Public IP: " + pubIPString);
    sendPubIP(pubIPString);
  }else{
    Serial.println("Failed to process public ip");
  }
}

void sendPubIP(String pubIPString){
  String pubIP = pubIPString;
  Serial.println("Sending " + pubIP);
  if (pubIP != ""){
    if(client.connect(hostName, hostPort)){
      Serial.println("Updating public IP");

      //Create post request
      String postRequest = "POST /recieve_ip HTTP/1.1\r\n";
      postRequest += "Host: " + String(hostName) + "\r\n";
      postRequest += "Content-Type: application/x-www-form-urlencoded\r\n";
      postRequest += "Connection: close\r\n";
      postRequest += "Content-Length: " + String(pubIPString.length() + 3) + "\r\n\r\n";
      postRequest += "ip=" + pubIPString;

      client.print(postRequest);

      client.stop();
      Serial.println("Disconnected From update");
    }else{
      Serial.println("Connection Failure");
    }
  } else{
    Serial.println("invalid IP");
  }
}

void sendGETRequest() {
  if (client.connect(hostName, hostPort)){
    Serial.println("Connected to server");

    // Send GET request
    client.println("GET /message HTTP/1.1");
    client.println("Host: " + String(hostName));
    client.println("Connection: close");
    client.println();

    String response = "";
    while (client.connected() || client.available()) {
      if (client.available()){
        char c = client.read();
        response += c;
      }
    }
    // Serial.println("Full response from server");
    // Serial.println(response);

    int bodyStart = response.indexOf("\r\n\r\n") + 4;  // Start of the body
    if (bodyStart > 4) {
      String body = response.substring(bodyStart);

      int endOfMessage = body.indexOf("\n");  // Find the newline character
      if (endOfMessage != -1) {
        String message = body.substring(0, endOfMessage);  // Extract the message
        message.trim();  // Remove any leading/trailing whitespace
        Serial.println("Message: ");
        Serial.println(message);
        processMessage(message);
      } else {
        body.trim();  // If no newline found, trim the body
        Serial.println("Message found (No newline character detected): ");
        Serial.println(body);  // Print the full message
      }
    } else {
      Serial.println("No message found on Server");
    }

    client.stop();
    Serial.println("Disconnected from server");
  } else {
    Serial.println("Connection to server failed");
  }
}


String processMessage(String message){
  if (message == ""){
    return "";
  }else{
    lcdPrint(message);
  }
}

void lcdPrint(String message) {
  Serial.println("Printing message to LCD: " + message);
  lcdCommand(0x01); //clear display before write.
  lcdCommand(0x80); //cursor to top left.
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x40);
  for (int i = 0; i < message.length(); i++){
    Wire.write(message[i]);
  }
  Wire.endTransmission();

  // play sound from speaker
  /*
  tone(speakerPin, 523.25);
  delay(200);
  tone(speakerPin, 400);
  delay(200);
  noTone(speakerPin);
*/
}


void lcdInit(){
  Serial.println("LCD Initialisation");
  lcdCommand(0x38); // Function set: 8-bit, 2 lines, 5x8 dots
  lcdCommand(0x39); // Function set: 8-bit, 2 lines, extended instruction set
  lcdCommand(0x14); // Internal OSC frequency
  lcdCommand(0x70); // Contrast setting
  lcdCommand(0x56); // Power/ICON control/Contrast set
  lcdCommand(0x6C); // Follower control
  delay(200);       // Wait for stabilization
  lcdCommand(0x38); // Function set: 8-bit, 2 lines, normal instruction set
  lcdCommand(0x0C); // Display ON, Cursor OFF, Blink OFF
  lcdCommand(0x01); // Clear display
  delay(2);
}

void lcdCommand(uint8_t cmd){
  Serial.println("Command sarted");
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x80);
  Wire.write(cmd);
  Wire.endTransmission();
  delay(2);
}

unsigned long lastGETRequestTime = 0;
unsigned long lastPubIPTime = 0;
const unsigned long getRequestInterval = 5000;    // 5 seconds
const unsigned long pubIPInterval = 10 * 60 * 1000; // 10 minutes (in milliseconds)

void loop() {
  unsigned long currentTime = millis();

  // Run sendGETRequest() every 5 seconds
  if (currentTime - lastGETRequestTime >= getRequestInterval) {
    sendGETRequest();
    lastGETRequestTime = currentTime;
  }

  // Run getPubIP() every 10 minutes
  if (currentTime - lastPubIPTime >= pubIPInterval) {
    getPubIP();
    lastPubIPTime = currentTime;
  }
}
