# Arduino-Lovers
## What is Arduino-Lovers:
Arduino lovers is a fun little gift idea for couples that want to have a fun way of sending messages to one another. You can send a message from your phone or computer at home and have it show up on their little love box, or they can send a message from their phone or computer and it will show up on yours. IF sending messages from neither network it will show up on both of your boxes.


Here I shall guide you through how to setup your own Arduino-Lovers:
## What you will need:
 - A 3D printer
 - 2 Arduino Uno R4 Wifi
 - 2 I2C 16x2 LCD (personally I used the WaveShare LCD1602 I2C Module) **however as I used the wire library any should work**
 - 2 AdaFruit STEMMA Speaker it is a speaker an amplifier in one you will also need their STEMMA JST PH 3 pin Male header cable
 - Some jumper cables

## Getting started:
### Input SSID and password:
Once you have downloaded or cloned the git repsitory you will want to go into the `Home_Arduino_Server.ino` and `External_Arduino_Client.ino` file and input the SSID (name of the network) and password for each network.

 
![External ssid password](https://github.com/user-attachments/assets/755ce229-4dbd-42c9-ba5f-e28c9238e1ea)
*External arduino code*


![home ssid password](https://github.com/user-attachments/assets/3c7ee42c-df5d-4c66-a2b9-0489bb5b3b4a)
*home arduino code*

### Setup home server:
The first thing you will want to do is wire up the LCD to the arduino you want to use as your home server. There should be 4 pins on the LCD **VCC, GND, SCL, SDA** which should each be connected to the following pins on your arduino respectively 5/3.3V (Depends on spec of your lcd), GND, SCL and SDA.

Once the LCD is connected run upload the `LCD_I2C_Scanner.ino` file and watch the Serial monitor on 9600 baud. Once the program has completed look through the serial monitor until you find a line something like this:

```
Checking address 0x3E
I2C device found at address 0x3E
```
this will be your LCD's address.


Now go back to `Home_Arduino_Server.ino` delete the line:

`#define LCD_ADDR 0x3E` 

and input:

`#define LCD_ADDR (your address)` 

in the same fashion as the original.

Now upload the code to the arduino.
#### Networking:
Now that the code is running on the arduino switch the baud rate on the serial monitor to 115200 and you should see it attempting to connect to the internet. If it doesnt work initially try pressing the rest button on your arduino as you may have missed it whilst changing over to the correct baud rate.

```
Connected to WiFi

Server IP: ***.***.*.***

Server MAC Address: **:**:**:**:**:**

LCD Initialisation

cleared screen
```

Go to your routers console and find the tab that lets you see all connected devices I am on BT where you will find it under my devices

![BT HUB](https://github.com/user-attachments/assets/f3c189ba-6c16-4975-a7e3-c77d272bf2ea)


You will see in the serial monitor `Server IP: ***.***.*.***` which will have actual numbers rather than stars find this IP address and rename it to something sensible like ArduinoHomeWebServer or something.

**You will now need to do some portforwarding.**

Go to the portforwarding tab again for me it is under *advanced settings*, *firewall* and then *portforwarding* forward whichever port you would like I personally used 8040 as my ISP blocks port 80 as that is the standard for webservers and yours probably will too. 

![Port forwarding](https://github.com/user-attachments/assets/420c7fa9-1091-4caf-a200-9955af61ae9f)

If you are struggling to figure this out there are hundreds of tutorials on portforwarding but just set the protocol to both UDP and TCP.

Now go back to `Home_Arduino_Server.ino` and find the line:

`WiFiServer server(8040);`

Near the top around line 17, And change out 8040 for whichever port you decided to forward.

#### Setting up DDNS:
This bit is very easy I used noIP as my DDNS provider which is as simple as making an account and using your free hostname to, well create a host name just use whichever DDNS provider you would prefer and follow their instructions. You will then use 

`yoururl.ddns.net:port` to get to the server from external networks and `serverprivateip:port` from the home network.
### Final steps:
You now just need to connect the other LCD to the external arduino and change the address in `External_Arduino_Client.ino` file to the same as the address in the home server. 

Set `//const char* hostName = "Your hostName eg. arduino-lovers.ddns.net";` to your host name

and set

`//const int hostPort = The port you forwarded eg. 8080;` to the port you forwarded dont forget to uncomment the lines.
now uncomment that code and you should be able to send messages from your phone or computer to the devices. 

### Speaker:
If you want to connect the speaker just connect power and ground and connect the signal wire to digital pin 9 on the arduino and it should work no problem.
