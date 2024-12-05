#include <WiFiS3.h>
#include <Wire.h>
#include "secrets.h"
#include <Arduino.h>

#define LCD_ADDR 0x3E // use your I2C address
// Network credentials
//const char* ssid = "Your SSID";
//const char* password = "Your Password";

unsigned long previousMillis = 0;
const long interval = 5000;

const int speakerPin = 9;


WiFiServer server(80);

void setup() { 

 
  Serial.begin(115200);
  delay(100);

  // Disconnect from any previously connected network (no argument needed)
  WiFi.disconnect();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

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
  
  Wire.begin();
  lcdInit();
  
  
  delay(2000);
  lcdCommand(0x80);
  lcdCommand(0x01);
  Serial.println("cleared screen");
}


bool isLocalIP(WiFiClient client) {
  IPAddress clientIP = client.remoteIP();
  IPAddress localIP = WiFi.localIP();
  if ((clientIP[0] == localIP[0]) && (clientIP[1] == localIP[1]) && (clientIP[2] == localIP[2])) {
     Serial.println(clientIP);
     return true;
  }else{
    return false;
  }
}

IPAddress getExternalIP(){
  return WiFi.localIP();
}

void handleClient(WiFiClient client) {
  String currentLine = "";
  String postData = "";
  bool isPost = false;

  bool ip = isLocalIP(client);

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


void processPostData(WiFiClient client, String postData, bool isLocal) {
  String message = postData.substring(8);
  message.replace("+", " ");
  message.replace("%3C", "<");
  Serial.println(message);
  sendWhere(message, isLocal);
}


void sendWhere(String message, bool isLocal){
  if (!isLocal){
    lcdPrint(message);
  }else{
    sendToExternal(message);
  }
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

void lcdIP(){
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
  Serial.println("Command sarted");
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

void sendToExternal(String message){
  Serial.println(message);
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