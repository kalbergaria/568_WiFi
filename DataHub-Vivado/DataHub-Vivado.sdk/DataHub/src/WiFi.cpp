#include "WiFi.h"
#include "Config.h"

// The following #include has to be placed after "WiFi.h" because
// of forward declarations
// TODO: see if I can make this include relative
#include "../../DataHub_bsp/microblaze_0/include/DEIPcK/DEIPcK.h"

////////////////////////////////////////////////////////////////
// 	Summary:
// 		Sets a static IP if operating as the HUB
// 		else allow DHCP to set the IP
#if MY_NODE_ID != 4 // 4 - denotes HUB
	// Set to {0,0,0,0} for DHCP
	// or set each IPV4 segment
	// Example: 192.168.100.10 = {192,168,100,10}
	static const IPv4 MY_IP = {0,0,0,0};
#else
	// The static IP for the data hub is 192.168.100.50
	static const IPv4 MY_IP = {192,168,100,50};
#endif

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

// If you want this to run/not-run, make sure MY_NODE_ID is properly set in Config.h
#if MY_NODE_ID != 4
			xil_printf("Attempting to connect to HUB...\r\n", SSID);
			if(ConnectToHub(udpSocket, status))
			{
				xil_printf("Preparing to register sensors...\r\n", SSID);
				if(RegisterSensors(udpSocket, status))
					xil_printf("All sensors have been registered!");
				else
					return false;
			}
			else
				return false;
#endif

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
// Sensor-node WiFi Interface Functions
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

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
bool ConnectToHub(UDPSocket* udpSocket, IPSTATUS* status)
{
	// Resolve the end point (i.e. the Data Hub)
	IPEndPoint epRemote;
	deIPcK.resolveEndPoint(HUB_IP, PORT, epRemote, status);
	deIPcK.udpSetEndPoint(epRemote, *udpSocket, PORT, status);
	// Always check the status and get out on error
	if (IsIPStatusAnError(*status))
	{
		xil_printf("Unable to resolve endpoint(Data Hub), error: 0x%X\r\n", status);
		return false;
	}

	// Send the connection request and allow up to 1 second for a SYS_HEALTH response
	Message connReqMsg;
	Message sysHealthMsg;
	CreateConnReqMsg(&connReqMsg, MY_NODE_ID);
	if(SendMessage(&connReqMsg, udpSocket, status, HUB))
	{
		xil_printf("Waiting up to 1 second for a response from the Data Hub...\r\n");
		if(WiFiListenXMillisForMsgOfType(1000/*msec*/, SYS_HEALTH, &sysHealthMsg, udpSocket))
		{
			StoreSystemHealthState(&sysHealthMsg);
			xil_printf("A connection to the Data Hub has been established!\r\n");
			return true;
		}
	}
	xil_printf("Unable to connect to the Data Hub!\r\n");
	return false;
}

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
bool RegisterSensors(UDPSocket* udpSocket, IPSTATUS* status)
{
	// Calculate the number of sensor that need to be registered
	// based on the size of the array of sensorInfo structures
	uint8_t numSensors = sizeof(sensorInfoCollection) / sizeof(SensorInfo);
	xil_printf("Attempting to register %d sensors\r\n", numSensors);

	// Attempt to register each sensor
	bool successfulRegistration = true;
	Message sensorRegMsg;
	Message regAckMsg;
	for(int i = 0; i < numSensors; i++)
	{
		CreateSensorRegMsg(&sensorRegMsg, (uint8_t*)&sensorInfoCollection[i], MY_NODE_ID);
		if(SendMessage(&sensorRegMsg, udpSocket, status, HUB))
		{
			// Wait up to 250 ms for the registration to be
			// acknowledged by the Data Hub
			if(WiFiListenXMillisForMsgOfType(250/*msec*/, REG_ACK, &regAckMsg, udpSocket))
				xil_printf("%s has been successfully registered!\r\n", sensorInfoCollection[i].sensorName);
			else
			{
				xil_printf("ERROR: %s could not be registered\r\n", sensorInfoCollection[i].sensorName);
				successfulRegistration = false;
			}
		}
	}

	return successfulRegistration;
}

////////////////////////////////////////////////////////////////
// 	Summary:
// 		The data and the ID of its sensor provided to this
//      function will be sent to the Data Hub.
//
// 	Returns:
// 		TRUE -> if the sensor data and ID was successfully sent
// 		FALSE -> if there was an issue sending the data provided
bool SensorDataPub(UDPSocket* udpSocket, IPSTATUS* status, SensorData sensorData, uint8_t sensorID)
{
	// Create and populate the Sensor Data Pub message
	Message dataPubMsg;
	SensorDataPayload payload;
	payload.sensorID = sensorID;
	memcpy(&sensorData, &payload.sensorData, SENSOR_DATA_SIZE);
	CreateSensorDataPubMsg(&dataPubMsg, (uint8_t*)&payload, MY_NODE_ID);

	// Send the message
	if(SendMessage(&dataPubMsg, udpSocket, status, HUB))
	{
		xil_printf("Data from !\r\n");
		// TODO: Print the sensor ID
		xil_printf(" has been published to the Hub!");
		return true;
	}
	xil_printf("Error publishing the data from ");
	// TODO: Print the sensor
	xil_printf("\r\n");
	return false;
}

////////////////////////////////////////////////////////////////
// 	Summary:
// 		
//
// 	Returns:
// 		TRUE -> 
// 		FALSE -> 
bool CheckForSystemHealthMsg(Message* msg, UDPSocket* udpSocket)
{
	// TODO: implement CheckForSystemHealthMsg()
}

////////////////////////////////////////////////////////////////
// 	Summary:
// 		
//
// 	Returns:
// 		TRUE -> 
// 		FALSE -> 
bool SubToSensor(Message* msg, UDPSocket* udpSocket)
{
	// TODO: implement SubToSensor()
}

////////////////////////////////////////////////////////////////
// 	Summary:
// 		
//
// 	Returns:
// 		TRUE -> 
// 		FALSE -> 
bool ReportEmergency(Message* msg, UDPSocket* udpSocket)
{
	// TODO: implement ReportEmergency()
}

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
void WiFiListenForXMillisAndRespond(UDPSocket* udpSocket, uint32_t waitTime_ms)
{
	// Variables init
	uint32_t bytesRead;
	Message message;

	// Start the timer
	uint32_t timeStart_ms = SYSGetMilliSecond();
    while(ElapsedMilliSeconds(timeStart_ms, SYSGetMilliSecond()) < waitTime_ms)
    {
		// Read incoming messages and check if they are of the desired type
	    if((bytesRead = RecvMessage(&message, udpSocket)) > 0)
		{
			PrintMsgType((MsgTypes)message.header.msgType);
			xil_printf(" message received!");

			switch(message.header.msgType)
			{
				case CONNECTION_REQ:
					HandleConnReq(msg, udpSocket);
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
void HandleConnReq(Message* msg, UDPSocket* udpSocket)
{
	xil_printf("Received connection request from: ");
	PrintSensorNodeId(msg->header.sourceId);
	// Make sure that the node requesting a connection is not
	// already connected, if it is ignore the request
	if(!connectedNodes[msg->header.sourceId])
	{
		// Send a system health msg to the new node
		Message sysHealthMsg;
		CreateSysHealthMsg(&sysHealthMsg, /*System Health Payload*/, MY_NODE_ID);
		SendHealth(&sysHealthMsg, udpSocket);
		xil_printf("Sending SYS_HEALTH to: ");
		PrintSensorNodeId(msg->header.sourceId);

		// Allow 1 second for the new node to ACK the system health msg
		if(WaitXMillisForMsgOfSpecifiedType(1000, SYS_HEALTH_ACK, NULL, udpSocket))
		{
			// Record that the node has successfully connected to the hub
			numConnectedNodes++;
			connectedNodes[msg->header.sourceId] = true;
			xil_printf("A connection to ");
			PrintSensorNodeId(msg->header.sourceId);
			xil_printf(" has been established!");
		}
		else
		{
			// 1 second elapsed and no ACk was received
			xil_printf("Unable to establish a connection to ");
			PrintSensorNodeId(msg->header.sourceId);
			xil_printf(" they did not ACK the SYS_HEALTH");
		}
	}
	else
	{
		xil_printf("This node already has an active connection to the hub!");
		xil_printf("This request will be ignored!");
	}
}

////////////////////////////////////////////////////////////////
// 	
bool SendHealth(Message* msg, UDPSocket* udpSocket)
{
	// TODO: Handle system health msg distribution
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

// Read all incoming messages until a message of the specified type comes in,
// or the specified amount of time has elapsed
bool WiFiListenXMillisForMsgOfType(uint32_t waitTime_ms, MsgTypes typeWanted, Message* msg, UDPSocket* udpSocket)
{
	// Variables init
	uint32_t bytesRead;
	Message message;
	bool wantedMsgRecvd = false;

	// Print the message type we are waiting for
	xil_printf("waiting for a ");
	PrintMsgType(typeWanted);
	xil_printf("...\r\n");

	// Start the timer
	uint32_t timeStart_ms = SYSGetMilliSecond();
    while(!wantedMsgRecvd)
    {
		// Read incoming messages and check if they are of the desired type
	    if((bytesRead = RecvMessage(&message, udpSocket)) > 0)
		{
			if(message.header.msgType == typeWanted)
			{
				PrintMsgType((MsgTypes)message.header.msgType);
				xil_printf(" message received!");

				wantedMsgRecvd = true;
			}
		}
		// Check if the 1 second has elapsed, and if so return false
		else if (ElapsedMilliSeconds(timeStart_ms, SYSGetMilliSecond()) > waitTime_ms)
		{
			xil_printf("%d miliseconds have elapsed, assuming that the message is not coming\r\n", waitTime_ms);
			return false;
	 	}
	 }

	 return true;
}

uint32_t RecvMessage(Message* msg, UDPSocket* udpSocket)
{
	if ((udpSocket->available()) > 0)
	{
		return udpSocket->readDatagram((byte*)msg, MSG_SIZE);
	}
	return 0;
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
