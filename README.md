# 568_WiFi
This repository contains a **Vivado 2017.4** project that implements the **Wi-Fi interface** for all of the subsystems within the **ECE 568 Home IoT** project. The project is configured to target the **Nexys 4 DDR**.

This README will be updated regularly to

## Currently Functioning (i.e. will work as soon as its put on your board)
- Connects to a router (with no security)

## Soon to be functioning (i.e. the code is there with some bugs...)
*NOTE: Everyting in this section will be fully functional by Monday, April 2nd or sooner.*
- Establishes a connection to the Data Hub
- Registers all of a subsystem's sensors (all those specified in Config.h)
- WPA2 Security (connection established with the use of a passphrase)

## Future Development
- CoAP (specifically the [microcoap implementation](https://github.com/1248/microcoap))

## About the Wi-Fi Interface
This interface is being developed to contain all of the capabilities shown in the message sequence diagram below.
