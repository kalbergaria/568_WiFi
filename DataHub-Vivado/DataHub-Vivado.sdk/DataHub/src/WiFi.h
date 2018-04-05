#ifndef WIFI_H
#define WIFI_H

#include "Messages.h"

// Forward declarations
class UDPSocket;
#define IPSTATUS uint32_t

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
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// System Health
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// A maximum of 4 nodes will attempt to connect to the HUB
#define MAX_CONNECTED_NODES 4

// TODO: check if these need to be extern
// Stores the number of nodes that are connected to the Hub
static uint8_t numConnectedNodes;

struct SystemHealthPayload
{
    // A bool array to track which nodes have connected
    // The node ID for each node is its index into the array below
    // which tracks if that particular node is connected to the HUB
    // or not.
    // According to SensorNodeIds (defined in Config.h) the indexes would be:
    // LIGHTING -> 0
    // CLIMATE  -> 1
    // POWER    -> 2
    // PLANTS   -> 3
    bool connectedNodes[MAX_CONNECTED_NODES];

    // A bool array to track which sensors have been registered
    // TRUE -> sensor registered and can be subscribed to
    // FALSE -> sensor unregistered
    // The sensor ID for each sensor is its index into the array below
    // which tracks if that particular node is registered with the HUB
    // or not.
    // The Sensor IDs can be seen in the SensorIds enum defined earlier
    // in this file
    bool registeredSensors[NUM_SENSORS];
};

// A data structure to keep the current system health
static SystemHealthPayload currentSysHealth;

// TODO: Need a data structure to keep track of the IP addresses of each of the connected nodes
// TODO: Need a data structure that will keep track of how many health msgs a particular node has missed

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Sensor Data
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// 	Summary:
// 		The struct and size to be used for sensor data
//
// 	Returns:
// 		N/A
static const uint16_t SENSOR_DATA_SIZE = 1019;
typedef uint8_t SensorData[SENSOR_DATA_SIZE];
struct SensorDataPayload
{
    uint8_t sensorID;
    SensorData sensorData;
};

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Sensor-node WiFi Interface Functions
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////
// 	Summary:
// 		Handles all initialization needed for the WiFi Pmod
//
// 	Returns:
// 		VOID
void InitWiFi();

////////////////////////////////////////////////////////////////
// 	Summary:
// 		IF DATA HUB -> An attempt at establishing a connection
//		to the network specified in Config.h will be made
//
// 		IF SENSOR NODE -> An attempt at establishing a
//		connection to the network specified in Config.h will be
// 		made, and attempt to establish a connection with the
//		Data Hub
//
// 	Returns:
// 		TRUE -> if a connection to the Data Hub was established
// 		FALSE -> if a connection to the Data Hub was not established
bool Connect(UDPSocket* udpSocket, IPSTATUS* status);

////////////////////////////////////////////////////////////////
// 	Summary:
// 		Sends a connection request to the Data Hub. If the hub
// 		does not respond with a system health message within 1
//		second, then the connection attempt will be deemed a
//		failure
//
// 	Returns:
// 		TRUE -> if a connection to the Data Hub was established
// 		FALSE -> if a connection to the Data Hub was not
//		established
bool ConnectToHub(UDPSocket* udpSocket, IPSTATUS* status);

////////////////////////////////////////////////////////////////
// 	Summary:
// 		Register each sensor belonging to the current node
// 		one-by-one allowing 250ms for the Data Hub to respond
// 		to confirm the registration of each sensor. Any
// 		registration is only deemed a success if the
//		registration is acknowledged by the Data Hub.
//
// 	Returns:
// 		TRUE -> if all sensors were successfully registered
// 		FALSE -> if any sensor could not be registered
bool RegisterSensors(UDPSocket* udpSocket, IPSTATUS* status);

////////////////////////////////////////////////////////////////
// 	Summary:
// 		The data and the ID of its sensor provided to this
//      function will be sent to the Data Hub.
//
// 	Returns:
// 		TRUE -> if the sensor data and ID was successfully sent
// 		FALSE -> if there was an issue sending the data provided
bool SensorDataPub(UDPSocket* udpSocket, IPSTATUS* status, SensorData sensorData, uint8_t sensorID);

bool CheckForSystemHealthMsg(Message* msg, UDPSocket* udpSocket);
bool SubToSensor(Message* msg, UDPSocket* udpSocket);
bool ReportEmergency(Message* msg, UDPSocket* udpSocket);

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Hub WiFi Interface Functions
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////	
// 	NOTE: 
//		This function is not guaranteed to finish execution
//		within the specified time if it is busy responding
//		to a message it received.
bool WiFiListenForXMillisAndRespond(UDPSocket* udpSocket, uint32_t waitTime_ms);

void HandleConnReq(Message* msg, UDPSocket* udpSocket);
bool SendHealth(Message* msg, UDPSocket* udpSocket); 	 
bool DistribHealthToConnectedNodes(Message* msg, UDPSocket* udpSocket);

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
bool SendMessage(Message* msg, UDPSocket* udpSocket, IPSTATUS* status, SensorNodeIds destId);
bool WaitXMillisForMsgOfSpecifiedType(uint32_t waitTime_ms, MsgTypes typeWanted, Message* msg, UDPSocket* udpSocket);
uint32_t RecvMessage(Message* msg, UDPSocket* udpSocket);
uint32_t ElapsedMilliSeconds(uint32_t start, uint32_t current);
void PrintSensorNodeId(SensorNodeIds id);
void PrintMsgTypeAndDest(MsgTypes msgType, SensorNodeIds destId);
void PrintMsgType(MsgTypes msgType);

#endif // WIFI_H
