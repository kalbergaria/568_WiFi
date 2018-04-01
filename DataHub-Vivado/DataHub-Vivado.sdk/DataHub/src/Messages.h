#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdint.h>
#include "Config.h"


//--------------------------------------------------------------
// IDs and Types
//--------------------------------------------------------------

enum MsgTypes
{
    CONNECTION_REQ,      // 0
    SYS_HEALTH,          // 1
    EMERGENCY_REPORT,    // 2
    EMERGENCY_BROADCAST, // 3
    SENSOR_REG,          // 4
    REG_ACK,             // 5
    SENSOR_DATA_PUB,     // 6
    SENSOR_SUB,          // 7
    SENSOR_DATA_FORW,    // 8
    SENSOR_SAMPLE_REQ,   // 9
    SENSOR_UNSUB,        // 10
    SENSOR_SUB_INV,      // 11
    UNSUB_ACK            // 12
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
