#include <WiFiS3.h>
#inclide <secrets.h>
// Network credentials
//const char* ssid = "Your SSID";
//const char* password = "Your Password";


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
void handleClient(WiFiClient client) {
  String currentLine = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
       Serial.write(c);

      if (c == '\n') {
       // If the line is empty, it’s the end of the HTTP request
        if (currentLine.length() == 0) {
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
          currentLine = "";
        }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }


/*void parseMessageRequest(WiFiClient client) {
  
}
*/
void loop() {
    WiFiClient client = server.available();
    if (client) {
      Serial.println("New Client connected");
      isLocalIP(client);
      handleClient(client);
    }
}
