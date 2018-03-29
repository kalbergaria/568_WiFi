# 568_WiFi
This repository contains a **Vivado 2017.4** project that implements the **Wi-Fi interface** for all of the subsystems within the **ECE 568 Home IoT** project. The project is configured to target the **Nexys 4 DDR**.

**This README will be updated regularly to reflect what portions of the Wi-Fi interface have/haven't been implemented.**

## Currently Functioning (i.e. will work as soon as its put on your board)
- Establishes a connection to a router with no security.

## Soon to be functioning (i.e. the code is there with some bugs...)
*NOTE: Everyting in this section will be fully functional by Monday, April 2nd or sooner.*
- Establishes a connection to the Data Hub
- Registers all of a subsystem's sensors (all those specified in Config.h)
- WPA2 Security (connection established with the use of a passphrase)

## Future Development
*NOTE: Located after each item in brackets is the date the implementation of the item is intended to be complete.*
- Sensor Data Publishing                      [Monday, April 2nd]
- Sensor Subscriptions and Data Forwarding    [Monday, April 9th]
- Emergency Reporting and Broadcasting        [Monday, April 9th]
- CoAP (using specifically the [microcoap implementation](https://github.com/1248/microcoap)) [as time permits...]

## About the Wi-Fi Interface
This interface is being developed to support all of the messaging capabilities shown in the sequence diagram below.

![Sequence Diagram](/ReadMeImages/SequenceDiagram.png)

## Block Diagram
If you wish to use the block diagram in this repository as a starting point, I recommend following the guide we followed to arrive at the current diagram which can be found [HERE](https://www.youtube.com/watch?v=KTeTMv3oiPw&t=5s).

**OR**

If you are using your own block diagram and only wish to augment it with the elements you need in order to get the Wi-Fi Pmod to work, then open the project within this repository, view the block diagram, and add any elements that are within it that are not yet present in your block diagram.

## How to use the ECE 568 Wi-Fi Interface Libraries
*NOTE: The best way to verify if your block diagram and SDK setting are compatible with our libraries, is to check if you are able to successfully compile and run the Scan.c program that is also located in this folder. This Scan.c file simply contains the code in the WiFiScan Digilent Wi-Fi Pmod example.*

The library files are located in the [src directory](https://github.com/kalbergaria/568_WiFi/tree/master/DataHub-Vivado/DataHub-Vivado.sdk/DataHub/src). Copy the files listed below into your SDK project (if you already have developed your own main.cpp then you may want to exclude this file). 

**Files to Copy**
1) main.cpp
2) Config.h
3) WiFi.h
4) WiFi.cpp
5) Messages.h
6) Messages.cpp

Reference the main.cpp file as an example on how to use the functions developed. You may also want to view the comments present in the WiFi.h and Messages.h files for information regarding a specific function (i.e. parameters, what it does, and what it returns).

## Reminders
As you run into issues we encourage you to open issues so that we can address ASAP!
