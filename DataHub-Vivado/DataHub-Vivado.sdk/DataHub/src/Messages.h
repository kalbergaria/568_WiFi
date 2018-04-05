#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdint.h>
#include "Config.h"


//--------------------------------------------------------------
// IDs and Types
//--------------------------------------------------------------

enum MsgTypes
{
    CONNECTION_REQ,
    SYS_HEALTH,
    SYS_HEALTH_ACK,
    EMERGENCY_REPORT,
    EMERGENCY_BROADCAST,
    SENSOR_REG,
    REG_ACK,
    SENSOR_DATA_PUB,
    SENSOR_SUB,
    SENSOR_DATA_FORW,
    SENSOR_SAMPLE_REQ,
    SENSOR_UNSUB,
    SENSOR_SUB_INV,
    UNSUB_ACK
};

//--------------------------------------------------------------
// Header, Message, and Constants definitions
//--------------------------------------------------------------

static const uint16_t PAYLOAD_SIZE = 1020;
static const uint16_t MSG_SIZE = 1024;

struct Header
{
	uint8_t sourceId;
	uint8_t msgType;
    // While it is typically standard to have the message
    // size within the header, all messages are 1024 bytes
    // in size, therefore it has been omitted.
};

struct Message
{
    Header header;
    uint8_t payload[PAYLOAD_SIZE];
};

//--------------------------------------------------------------
// System Health Payload
//--------------------------------------------------------------

// A maximum of 4 nodes will attempt to connect to the HUB
#define MAX_CONNECTED_NODES 4

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

//--------------------------------------------------------------
// Message creation functions
//--------------------------------------------------------------

bool CreateConnReqMsg(Message* msg, uint8_t nodeId);
bool CreateSysHealthMsg(Message* msg, uint8_t* payload, uint8_t nodeId);
bool CreateSensorRegMsg(Message* msg, uint8_t* payload, uint8_t nodeId);
bool CreateSensorDataPubMsg(Message* msg, uint8_t* payload, uint8_t nodeId);

//--------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------
MsgTypes CheckMsgType(Message* msg);

#endif // MESSAGES_H
