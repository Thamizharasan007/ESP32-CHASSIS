ESP32 Web BLE 2WD Chassis Controller

This project transforms an ESP32 into a remote-controlled 2WD robotics chassis that you can drive using a web browser. It uses the Web Bluetooth API (BLE), meaning you do not need to download or install any apps—just open the webpage and connect!

Features

🎮 No App Required: Uses Web Bluetooth directly from Chrome or Edge.

🏎️ Differential Drive Math: Blends X and Y joystick inputs so the robot can curve and steer smoothly just like a real RC car.

📱 Touch-Friendly UI: Modern, dark-mode virtual joystick interface optimized for both desktop and mobile screens.

Hardware Required

ESP32 Development Board

Motor Driver (L298N, L293D, or similar)

2x BO Motors and Chassis

Power Bank (for the ESP32)

Battery Pack (for the Motors)

Wiring Diagram

Crucial Setup Note: Always use separate power supplies for the ESP32 and the Motors to prevent "Brownout Resets" when the motors draw heavy current.

ESP32 Pin

Motor Driver (L298N)

Function

Pin 25

ENA

Left Motor Speed (PWM)

Pin 26

IN1

Left Motor Direction

Pin 27

IN2

Left Motor Direction

Pin 13

ENB

Right Motor Speed (PWM)

Pin 14

IN3

Right Motor Direction

Pin 12

IN4

Right Motor Direction

GND

GND

Crucial Common Ground

Power the ESP32 via its Micro-USB/USB-C port using a power bank.

Power the Motor Driver by connecting an external battery (e.g., 9V or 18650s) to the 12V and GND terminals.

How to Install and Run

1. Flash the ESP32

Open chassis_firmware.ino in the Arduino IDE.

Make sure you have the ESP32 Board package installed (Version 3.0.0+ recommended).

Connect your ESP32 and click Upload.

2. Host the Web Controller (GitHub Pages)

For Web Bluetooth to work on mobile devices (Android/iOS), the HTML file must be hosted on a secure https:// server. GitHub Pages does this for free!

Upload index.html to a new repository on your GitHub account.

Go to your repository Settings -> Pages.

Under Build and deployment, set the Source to "Deploy from a branch".

Set the branch to main (or master) and click Save.

Wait a minute or two, and GitHub will give you a live link (e.g., https://yourusername.github.io/your-repo-name/).

3. Connect and Drive

Open the live link on your phone or laptop.

Android/Windows/Mac: Use Google Chrome. (Ensure Location/GPS is turned ON for Android).

iOS (iPhone): Download the free Bluefy browser from the App Store and open the link there (Safari does not support Web BLE).

Click Connect BLE.

Select ESP32-Chassis from the popup list.

Drive your robot!
