#include "WiFi.h"
#include "Config.h"

// The following #include has to be placed after "WiFi.h" because
// of forward declarations
// TODO: see if I can make this include relative
#include "../../DataHub_bsp/microblaze_0/include/DEIPcK/DEIPcK.h"

////////////////////////////////////////////////////////////////
// 	Summary:
// 		Sets the static IP for the hub as 192.168.100.50
static const IPv4 MY_IP = {192,168,100,50};

////////////////////////////////////////////////////////////////
// 	Summary:
// 		Handles all initialization needed for the WiFi Pmod
//
// 	Returns:
// 		VOID
void InitWiFi()
{
	setPmodWifiAddresses(
		XPAR_PMODWIFI_0_AXI_LITE_SPI_BASEADDR,
		XPAR_PMODWIFI_0_AXI_LITE_WFGPIO_BASEADDR,
		XPAR_PMODWIFI_0_AXI_LITE_WFCS_BASEADDR,
		XPAR_PMODWIFI_0_S_AXI_TIMER_BASEADDR
	);
	setPmodWifiIntVector(XPAR_INTC_0_PMODWIFI_0_VEC_ID);

	// Prep the hub for listening
	if (deIPcK.udpStartListening(portServer, udpSocket, &status)) 
	{
		// Add the sockets for each node
		for (i = 0; i < MAX_CONNECTED_NODES; i++)
			udpServer.addSocket(nodeSockets[i]);
	}

	// Set the status of all nodes and sensors to
	// disconnected/unregistered
	for(int i = 0; i < MAX_CONNECTED_NODES; i++)
		currentSysHealth.connectedNodes[i] = false;
	for(int i = 0; i < NUM_SENSORS; i++)
		currentSysHealth.registeredSensors[i] = false;
}

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
bool Connect(UDPSocket* udpSocket, IPSTATUS* status)
{
	if (WiFiConnectMacro())
	{
		if(deIPcK.begin(MY_IP))
		{
			xil_printf("Connection established with network: %s\r\n", SSID);
			deIPcK.getMyIP(MY_IP);
			xil_printf("My IP address is: %d.%d.%d.%d", MY_IP);

			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////	
// 	NOTE: 
//		This function is not guaranteed to finish execution
//		within the specified time if it is busy responding
//		to a message it received.
void WiFiListenForXMillisAndRespond(UDPSocket* udpSocket, IPSTATUS* status, uint32_t waitTime_ms)
{
	// Variables init
	uint32_t bytesRead;
	Message message;

	// Start the timer
	uint32_t timeStart_ms = SYSGetMilliSecond();
    while(ElapsedMilliSeconds(timeStart_ms, SYSGetMilliSecond()) < waitTime_ms)
    {
		// Read incoming messages and check if they are of the desired type
	    if(HubCheckForMsg(&message))
		{
			PrintMsgType((MsgTypes)message.header.msgType);
			xil_printf(" message received!");

			switch(message.header.msgType)
			{
				case CONNECTION_REQ:
					HandleConnReq(msg, udpSocket, status);
					break;

				case EMERGENCY_REPORT:
					// TODO: Handle EMERGENCY_REPORT
					break;

				case SENSOR_REG:
					// TODO: Handle SENSOR_REG
					break;

				case SENSOR_DATA_PUB:
					// TODO: Handle SENSOR_DATA_PUB
					break;

				case SENSOR_SUB:
					// TODO: Handle SENSOR_SUB
					break;

				case SENSOR_SAMPLE_REQ:
					// TODO: Handle SENSOR_SAMPLE_REQ
					break;

				case SENSOR_UNSUB:
					// TODO: Handle SENSOR_UNSUB
					break;

				default:
					break;
			}
		}
	 }
}

////////////////////////////////////////////////////////////////
// 	
void HandleConnReq(Message* msg, UDPSocket* udpSocket, IPSTATUS* status)
{
	xil_printf("Received connection request from: ");
	PrintSensorNodeId(msg->header.sourceId);
	xil_printf("\r\n");
	// Make sure that the node requesting a connection is not
	// already connected, if it is ignore the request
	if(!connectedNodes[msg->header.sourceId])
	{
		// FIXME: Do i need this?
		// Set the current endpoint as the node who just sent
		// a connection request to the hub
		IPEndPoint epRemote;
		deIPcK.resolveEndPoint(HUB_IP, PORT, epRemote, status);
		deIPcK.udpSetEndPoint(epRemote, *udpSocket, PORT, status);

		// Send a system health msg to the new node
		SendHealth(&sysHealthMsg, udpSocket, status);

		// Allow 1 second for the new node to ACK the system health msg
		if(WaitXMillisForMsgOfSpecifiedType(1000, SYS_HEALTH_ACK, NULL, udpSocket))
		{
			// Record that the node has successfully connected to the hub
			numConnectedNodes++;
			connectedNodes[msg->header.sourceId] = true;
			xil_printf("A connection to ");
			PrintSensorNodeId(msg->header.sourceId);
			xil_printf(" has been established!\r\n");
		}
		else
		{
			// 1 second elapsed and no ACk was received
			xil_printf("Unable to establish a connection to ");
			PrintSensorNodeId(msg->header.sourceId);
			xil_printf(" they did not ACK the SYS_HEALTH\r\n");
		}
	}
	else
	{
		xil_printf("This node already has an active connection to the hub!");
		xil_printf("This request will be ignored!\r\n");
	}
}

////////////////////////////////////////////////////////////////
// 	
bool SendHealth(UDPSocket* udpSocket, IPSTATUS* status)
{
	Message sysHealthMsg;
	CreateSysHealthMsg(&sysHealthMsg, &currentSysHealth, MY_NODE_ID);
	SendMessage(&sysHealthMsg, udpSocket, status, );
	xil_printf("Sending SYS_HEALTH to: ");
	PrintSensorNodeId(msg->header.sourceId);
	xil_printf("\r\n");
}

////////////////////////////////////////////////////////////////
// 	 
bool DistribHealthToConnectedNodes(Message* msg, UDPSocket* udpSocket)
{
	// TODO: Handle system health msg distribution
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Helper Functions
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

// FIXME: There is an error with the UDP sending
bool SendMessage(Message* msg, UDPSocket* udpSocket, IPSTATUS* status, SensorNodeIds destId)
{
	if (deIPcK.isIPReady(status))
	{
        PrintMsgTypeAndDest((MsgTypes)msg->header.msgType, destId);
        udpSocket->writeDatagram((byte*)msg, sizeof(Message));
        return true;
 	}
 	else if (IsIPStatusAnError(*status))
 	{
        xil_printf("Unable to send ");
        PrintMsgType((MsgTypes)msg->header.msgType);
		xil_printf(", error: 0x%X\r\n", status);
        return false;
 	}
 	else
 	{
        xil_printf("Unable to send ");
        PrintMsgType((MsgTypes)msg->header.msgType);
				xil_printf("\r\n");
        return false;
 	}
}

bool HubCheckForMsg(Message* msg)
{
	// Wait for a message
	if ((hubServer.availableClients()) > 0)
	{
		xil_printf("Message available!\r\n");

		// Accept the client to get the message
		if ((socket = hubServer.acceptClient()) != NULL)
		{
			xil_printf("Message obtained!\r\n");

			// Attempt to read the message
			int bytesRead = 0;
			if ((bytesRead = socket->available()) > 0)
			{
				bytesRead = socket->readDatagram(*msg, MSG_SIZE);

				PrintMsgType(msg->header.msgType);
				xil_printf("received from ");
				PrintSensorNodeId(msg->header.sourceId);
				xil_printf("\r\n");

				return true;
			}
		}
	}

	return false;
}

uint32_t ElapsedMilliSeconds(uint32_t start, uint32_t current)
{
	if(current > start)
		return current - start;
	else
		// Only here if the timer rolled over
		// 4294967295 is the max value of an unsigned 32-bit int
		return (uint32_t)4294967295 - start + current;
}

void PrintMsgTypeAndDest(MsgTypes msgType, SensorNodeIds destId)
{
	xil_printf("Sending ");
	PrintMsgType(msgType);
	xil_printf(" to ");
	PrintSensorNodeId(destId);
	xil_printf("\r\n");
}

void PrintMsgType(MsgTypes msgType)
{
	switch(msgType)
	{
	    case CONNECTION_REQ:
	    	xil_printf("CONNECTION_REQ");
	    	break;
	    case SYS_HEALTH:
	    	xil_printf("SYS_HEALTH");
	    	break;
	    case EMERGENCY_REPORT:
	    	xil_printf("EMERGENCY_REPORT");
	    	break;
	    case EMERGENCY_BROADCAST:
	    	xil_printf("EMERGENCY_BROADCAST");
	    	break;
	    case SENSOR_REG:
	    	xil_printf("SENSOR_REG");
	    	break;
	    case REG_ACK:
	    	xil_printf("REG_ACK");
	    	break;
	    case SENSOR_DATA_PUB:
	    	xil_printf("SENSOR_DATA_PUB");
	    	break;
	    case SENSOR_SUB:
	    	xil_printf("SENSOR_SUB");
	    	break;
	    case SENSOR_DATA_FORW:
	    	xil_printf("SENSOR_DATA_FORW");
	    	break;
	    case SENSOR_SAMPLE_REQ:
	    	xil_printf("SENSOR_SAMPLE_REQ");
	    	break;
	    case SENSOR_UNSUB:
	    	xil_printf("SENSOR_UNSUB");
	    	break;
	    case SENSOR_SUB_INV:
	    	xil_printf("SENSOR_SUB_INV");
	    	break;
	    case UNSUB_ACK:
	    	xil_printf("UNSUB_ACK");
	    	break;
	    default:
	    	break;
	}
}

void PrintSensorNodeId(SensorNodeIds id)
{
	switch(id)
	{
	    case LIGHTING:
	    	xil_printf("LIGHTING");
	    	break;
	    case CLIMATE:
	    	xil_printf("CLIMATE");
	    	break;
	    case POWER:
	    	xil_printf("POWER");
	    	break;
	    case PLANTS:
	    	xil_printf("PLANTS");
	    	break;
	    case HUB:
	    	xil_printf("DATAHUB");
	    	break;
	    default:
	    	break;
	}
}
