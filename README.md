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

## Getting Started with the Wi-Fi Pmod
If you wish to use the block diagram in this repository as a starting point, I recommend following the guide we followed to arrive at the current diagram which can be found [HERE](https://www.youtube.com/watch?v=KTeTMv3oiPw&t=5s).

If you are using your own block diagram and only wish to incorporate the elements you need 

As you run into issues we encourage you to open issues so that we can address ASAP!

Once you have the only thing you will want to be updating in your own projects is the source code

## Running this Project on Your Board
If you wish to see the project within this repository function on your board, open the porject then 

