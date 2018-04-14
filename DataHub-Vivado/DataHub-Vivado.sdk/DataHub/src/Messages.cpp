#include "Messages.h"
#include <string.h>

bool CreateConnReqMsg(Message* msg)
{
    msg->header.sourceId = MY_NODE_ID;
    msg->header.msgType = (uint8_t)CONNECTION_REQ;
    return true;
}

bool CreateSysHealthMsg(Message* msg, uint8_t* payload)
{
    msg->header.sourceId = MY_NODE_ID;
    msg->header.msgType = (uint8_t)SYS_HEALTH;
    memcpy(msg->payload, payload, PAYLOAD_SIZE);
    return true;
}

bool CreateSysHealthACKMsg(Message* msg)
{
    msg->header.sourceId = MY_NODE_ID;
    msg->header.msgType = (uint8_t)SYS_HEALTH_ACK;
    return true;
}

bool CreateSensorRegMsg(Message* msg, uint8_t* payload)
{
    msg->header.sourceId = MY_NODE_ID;
    msg->header.msgType = (uint8_t)SENSOR_REG;
    memcpy(msg->payload, payload, PAYLOAD_SIZE);
	return true;
}

bool CreateSensorDataPubMsg(Message* msg, uint8_t* payload)
{
    msg->header.sourceId = MY_NODE_ID;
    msg->header.msgType = (uint8_t)SENSOR_DATA_PUB;
    memcpy(msg->payload, payload, PAYLOAD_SIZE);
    return true;
 }
