#include <WiFiS3.h>
#include <Wire.h>
#include "serverConfig.h"
#include <Arduino.h>



unsigned long previousMillis = 0;
const long interval = 5000;

const int speakerPin = 9;

String externalArduinoPubIP = "";

WiFiServer server(port);

void setup() { 

 
  Serial.begin(115200);
  delay(100);
  
  Wire.begin();
  lcdInit();

  
  
  delay(2000);
  lcdCommand(0x80);
  lcdCommand(0x01);
  Serial.println("cleared screen");

  // Disconnect from any previously connected network (no argument needed)
// Disconnect from any previously connected network (no argument needed)
WiFi.disconnect();

// Wait for connection
bool connected = false;
while (WiFi.status() != WL_CONNECTED) {
  for (int i = 0; i < sizeof(ssid) / sizeof(ssid[0]); i++) {
    lcdPrint("WiFi Connecting");
    Serial.println(ssid[i]);
    Serial.println(password[i]);

    WiFi.begin(ssid[i], password[i]);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
      if (millis() - startAttemptTime >= 5000) { // Timeout after 5 seconds
        Serial.println("Connection attempt timed out.");
        break; // Try the next SSID
      }
      delay(500);
      Serial.print(".");
    }
    
    // Check if connected, exit the loop
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
  }

  // If not connected after trying all SSIDs, display "Failed connect" and retry
  if (!connected) {
    lcdPrint("Failed connect");
    lcdPrint("Retrying...");
    delay(2000); // Delay before retrying the connection
    WiFi.disconnect(); // Disconnect to reset the connection attempt
  } else {
    // Successfully connected
    String message = "WiFi Connected";
    lcdPrint(message);
    Serial.println("\nConnected to WiFi");
    break;
  }
}


  // Start server
  server.begin();
  Serial.print("Server IP: ");
  Serial.println(WiFi.localIP());
  uint8_t mac[6]; // Array to hold the MAC address
  WiFi.macAddress(mac); // Populate the array with the MAC address

  Serial.print("Server MAC Address: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(mac[i], HEX); // Print each byte in hexadecimal format
    if (i < 5) Serial.print(":"); // Add colons between bytes
  }
  Serial.println();
  
}


String isLocalIP(WiFiClient client) {
  String clientIPString = client.remoteIP().toString();
  String arduinoPublicIP = externalArduinoPubIP;
  IPAddress localIP = WiFi.localIP();
  IPAddress clientIP = client.remoteIP();
  Serial.println("CLIENT IP ADDRESS " + clientIPString);
  Serial.println("ARDUINO IP ADDRESS " + arduinoPublicIP);

  if ((clientIP[0] == localIP[0]) && (clientIP[1] == localIP[1]) && (clientIP[2] == localIP[2])){
    return "client";
  }else if (
    (clientIPString.substring(0, clientIPString.indexOf('.')) == arduinoPublicIP.substring(0, arduinoPublicIP.indexOf('.'))) &&
    (clientIPString.substring(clientIPString.indexOf('.') + 1, clientIPString.indexOf('.', clientIPString.indexOf('.') + 1)) ==
    arduinoPublicIP.substring(arduinoPublicIP.indexOf('.') + 1, arduinoPublicIP.indexOf('.', arduinoPublicIP.indexOf('.') + 1)))
  ) {
     return "host";
  }else{
    return "host/client";
  }
}

IPAddress getExternalIP(){
  return WiFi.localIP();
}

void handleClient(WiFiClient client) {
  String currentLine = "";
  String postData = "";
  bool isPost = false;

  String ip = isLocalIP(client);

  int contentLength = 0;
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
       Serial.write(c);

      if (c == '\n') {
       if (currentLine.length() == 0){

        if(isPost && contentLength > 0) {
          int bytesRead = 0;
          while (bytesRead < contentLength && client.available()) {
            char bodyChar = client.read();
            postData += bodyChar;
            bytesRead++;
          }
          Serial.print("POST body:");
          Serial.println(postData);


          processPostData(client, postData, ip);
        }
      
       
        
          // Send HTTP response
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();

          // HTML content
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head><title>Lovers Entwined</title></head>");
          client.println("<body>");
          client.println("<h1>Send a message to your loved one?</h1>");
          client.println("<form action='/submit' method='POST'>");
          client.println("<label for='message'>Message:</label><br>");
          client.println("<input type='text' id='message' name='message' size='50'><br>");
          client.println("<input type='submit' value='Send Message'>");
          client.println("</form>");
          client.println("</body>");
          client.println("</html>");
          client.println();

          

          break;

        } else {
          if (currentLine.startsWith("POST")) {
            isPost = true;
            Serial.println("POST received");
          }else if (currentLine.startsWith("Content-Length:")) {
            contentLength = currentLine.substring(15).toInt();
            Serial.println("Content Length recieved");
          }else if (currentLine.startsWith("GET /message")) {
            Serial.println("Get /message request recieved");
            sendMessageToClient(client);
          }

          currentLine = "";
        }
      } else if (c != '\r') {
        currentLine += c;  // Add characters to the current line
      }
    }
  }
    client.stop();
    Serial.println("Client disconnected");
}


void processPostData(WiFiClient client, String postData, String isLocal) {
  int ipIndex = postData.indexOf("ip=");
  if (ipIndex != -1){
    externalArduinoPubIP = postData.substring(ipIndex +3);
    externalArduinoPubIP.trim();
    Serial.println(externalArduinoPubIP);
  }else{
  String message = postData.substring(8);
  message.replace("+", " ");
  message.replace("%3C", "<");
  message.replace("%27", "'");
  Serial.println(message);
  sendWhere(message, isLocal);
  }
}


void sendWhere(String message, String isLocal){
  if (isLocal == "host"){
    lcdPrint(message);
  }else if (isLocal == "client"){
    addMessageQueue(message);
  }else if (isLocal == "host/client"){
    lcdPrint(message);
    addMessageQueue(message);
  }
}

void sendMessageToClient(WiFiClient client) {
  String message = getMessageQueue();
  Serial.println("Sending message to client: " + message);
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println();
  client.println(message.length() > 0 ? message : "\n");
}

void lcdPrint(String message) {
  lcdCommand(0x01); //clear display before write.
  lcdCommand(0x80); //cursor to top left.
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x40);
  for (int i = 0; i < message.length(); i++){
    Wire.write(message[i]);
  }
  Wire.endTransmission();

  // play sound from speaker
  tone(speakerPin, 523.25);
  delay(200);
  tone(speakerPin, 400);
  delay(200);
  noTone(speakerPin);

}

void lcdIP(){ // this is because we are using dhcp on the server so it allows you to see the current ip of the server and use that to communicate with the external Arduino
  IPAddress ip = WiFi.localIP();
  String ipString = ip.toString();
  lcdCommand(0xC0);
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x40);
  for (int i = 0; i < ipString.length(); i++){
    Wire.write(ipString[i]);
  }
  Wire.endTransmission();
}

void lcdCommand(uint8_t cmd){
  //Serial.println("Command sarted");
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(0x80);
  Wire.write(cmd);
  Wire.endTransmission();
  delay(2);
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

String messageQueue[10];
int messageCount = 0;

void addMessageQueue(String message){
  Serial.println("message added to queue " + message);
  if (messageCount <10){
    messageQueue[messageCount++] = message;
  }else{
    Serial.println("Message queue Full");
  }
}

String getMessageQueue(){
  if (messageCount > 0) {
    String message = messageQueue[0]; // first message in queue

    for (int i =1; i < messageCount; i++){
      messageQueue[i-1] = messageQueue[i];
    }
    messageCount--;
    return message;
  }
  return "";
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("New Client connected");
      isLocalIP(client);
      handleClient(client);
    }
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval){
      lcdIP();
    }
}