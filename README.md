# Arduino-Lovers
Here I shall guid you through how to setup your own Arduino-Lovers:
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

Now delete the line:

`#define LCD_ADDR 0x3E` 

and input:

`#define LCD_ADDR (your address)` 

in the same fashion as the original.

Now upload the code to the arduino.
#### Networking:
Now that the code is running on the arduino switch the baud rate on the serial monitor to 115200 and you should see it attempting to connect to the internet. If it doesnt work initially try pressing the rest button on your arduino as you may have missed it whilst changing over to the correct baud rate.
