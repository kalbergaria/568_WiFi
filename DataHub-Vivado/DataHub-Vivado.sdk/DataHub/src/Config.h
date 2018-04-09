#ifndef CONFIG_H
#define CONFIG_H

//#include "DEIPcK/utility/internetlayer.h"

// The IDs for each member of the system
enum SensorNodeIds
{
    LIGHTING,    // 0
    CLIMATE,     // 1
    POWER,       // 2
    PLANTS,      // 3
	HUB,         // 4
};

// This would not normarlly be static, however, to simplify and make it easy
// for groups to subscribe to each other's sensors it is currently implemented
// as static.
#define NUM_SENSORS 1
enum SensorIds
{
    // LIGHTING

    // CLIMATE

    // POWER

    // PLANTS

    // HUB
    GPS,
    RTCC,
};

// Structure containing sensor info fields
struct SensorInfo
{
    char* sensorId;
    char* sensorNodeId;
    char* sensorName;
    char* description;
    char* dataUnits;
    char* samplingRate;
    char* dataFormat;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
// Update the following settings to reflect the configuration
// for your board.
//--------------------------------------------------------------
//--------------------------------------------------------------

// Replace /*YOUR GROUP HERE*/ with one of the following in 
// accordance to your group:
// LIGHTING
// CLIMATE
// POWER
// PLANTS
// HUB
static const uint8_t MY_NODE_ID = (uint8_t)POWER/*YOUR GROUP HERE*/;

/*********************INTRUCTIONS**********************/
// 1) Copy the commented the section below enclosed with
// dashes (not this Instructions section enclosed in
// asterisks) and paste it below until the total number
// number of sections matches the number of sensors your
// sensor node has. Make sure to keep each of the
// sections you copy within leftmost set of brackets.
//
// 2) The following format is just to make it easy to
// register the sensors with the Data Hub, while not
// making the data static. When the data leaves the
// immediate system, it will be into the JSON format
// shown below. If you are unsure of what information
// should be included in a particular entry, refer to
// JSON schema below.
//
// "sensorID": "ID for there sensor from the agreed upon
//              sensor ID table",
// "owningDeviceID": "ID for the device that it belongs
//                    to form the agreed upon device ID
//                    table"
// "sensorName": "A unique name for the sensor",
// "description": "A description of the sensor"
// "dataUnits": "What are the units for the data this
//               sensor reports",
// "samplingRate": "The rate at which this sensor
//                  publishes data"
// "dataFormat": "Optional. If there is a special way of
//                interpreting the data (e.g. if it were
//                reporting time a format may be HH:MM:SS)"
//
// 3) WHEN FILLING OUT EACH FIELD, MAKE SURE NONE
// EXCEEDS 120 CHARACTERS!!!!!!!!!
//
/*******************END INTRUCTIONS********************/

static const SensorInfo sensorInfoCollection[] =
{
    //--------------------------------------------------
    // Sensor 1
    {
    /*sensorId*/        (char*)"",
    /*sensorNodeId*/    (char*)"",
    /*sensorName*/      (char*)"",
    /*description*/     (char*)"",
    /*dataUnits*/       (char*)"",
    /*samplingRate*/    (char*)"",
    /*dataFormat*/      (char*)"",
    }
    //--------------------------------------------------
};

//--------------------------------------------------------------
//--------------------------------------------------------------
// Network settings (DO NOT MODIFY)
//--------------------------------------------------------------
//--------------------------------------------------------------

// The SSID of the desired network
static const char* SSID = "ECE568";

// The static IP of the Data Hub
//static const char* HUB_IP = "192.168.100.50";
static const char* HUB_IP = "192.168.100.2";

// Port to be used for communication locally and by the Data Hub
static const uint16_t PORT = 44000;

// Select 1 for the security you want, or none for no security
//#define USE_WPA2_PASSPHRASE
#if defined(USE_WPA2_PASSPHRASE)

   const char* passPhrase = "myPassword";
   #define WiFiConnectMacro() deIPcK.wfConnect(SSID, passPhrase, status)

#else // No security - OPEN

   #define WiFiConnectMacro() deIPcK.wfConnect(SSID, status)

#endif

#endif //CONFIG_H
