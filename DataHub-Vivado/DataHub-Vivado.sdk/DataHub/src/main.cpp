#include "WiFi.h"
#include "PmodWIFI.h"

// Definition of all states
typedef enum {INIT,	CONNECT, WIFI, CLOSE, DONE} State;

// Prototypes
void ChangeStatePrintTransition(State* currState, State newState);
void PrintState(State state);

int main(void) 
{
	Xil_ICacheEnable();
	Xil_DCacheEnable();

	// Variable declaration and initialization
	State state = INIT;
	IPSTATUS status;
	bool performPeriodicTasks = false;

	while(1)
	{
		switch(state)
		{
			// Perform necessary WiFi initialization
			case INIT:
				xil_printf("\r\n\r\n-------------- NEW RUN --------------\r\n");
				xil_printf("Performing initialization...\r\n");
				InitWiFi();
				xil_printf("Initialization complete!\r\n");
				ChangeStatePrintTransition(&state, CONNECT);
				break;

			// Attempt to connect to the network per the specifications in Config.h
			case CONNECT:
				xil_printf("Attempting to connect to network...\r\n");
				while(!Connect(&status) && !IsIPStatusAnError(status)){}

				// If connection successful change states
				if(!IsIPStatusAnError(status))
				{
					performPeriodicTasks = true;
					ChangeStatePrintTransition(&state, WIFI);
				}
				else // error with connection attempt
				{
					xil_printf("ERROR: Unable to connect, status code: 0x%X\r\n", status);
					ChangeStatePrintTransition(&state, CLOSE);
				}
				break;

			// Handle any periodic tasks and poll all incoming interfaces for a task
			case WIFI:
				xil_printf("For now there is nothing to do in the IDLE state...\r\n");
				ChangeStatePrintTransition(&state, CLOSE);
				break;

			// Close the socket and transition to the DONE state
			case CLOSE:
				ChangeStatePrintTransition(&state, DONE);
				break;

			// The program will remain in this DONE state until the program
			// is restarted or the device is re-programmed
			case DONE:
			default:
				break;
		}

		// Every pass through loop(), keep the stack alive once the
		// WiFi interface is initialized
		if(performPeriodicTasks)
			DEIPcK::periodicTasks();
	}

	return 0;
}

void ChangeStatePrintTransition(State* currState, State newState)
{
	xil_printf("Transitioning from ");
	PrintState(*currState);
	xil_printf(" to ");
	PrintState(newState);
	xil_printf("\r\n");
	xil_printf("\r\n");
	*currState = newState;
}

void PrintState(State state)
{
	switch(state)
	{
		case INIT:
			xil_printf("INIT");
			break;
		case CONNECT:
			xil_printf("CONNECT");
			break;
		case WIFI:
			xil_printf("WIFI");
			break;
		case CLOSE:
			xil_printf("CLOSE");
			break;
		case DONE:
			xil_printf("DONE");
			break;
		default:
			break;
	}
}
