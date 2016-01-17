#include"System.h"

Uint16 receivedResponse[3] = {true,true,true};
Uint16 protocolTestPacketIDs[3];

#pragma CODE_SECTION(TestService,"FlashCode");
void TestService(Void)
{
	// Wait for the system to finish initializing itself
	SEM_pend(&TestServiceSem,SYS_FOREVER);

	// Run the selected test

#if TEST == TEST_SPI
	SPITest();
#elif TEST == TEST_PROTOCOL
	//ProtocolTest1();
	ProtocolTest2();
#elif TEST == TEST_MPI
	MPITest();
#endif

}

#pragma CODE_SECTION(SPITest,"FlashCode");
void SPITest()
{
#if defined(IS_ROUTER)
	Uint16 flit[NUM_CHARACTERS_IN_FLIT] = {
		0xCA5F, 0xCA5F, 0xCA5F, 0xCA5F, 0xCA5F,
		0xCA5F, 0xCA5F, 0xCA5F, 0xCA5F};
	Uint32 numTransmitsInGroup, numGroups = 0, numTransmits = 0;

	TSK_sleep(2000);
	while(numTransmits < 500000)
	{
		numTransmitsInGroup = 0;
		while(numTransmitsInGroup < 5000)
		{
			if(gpioDataRegisters.GPBDAT.bit.SWITCH1)
			{
				while(globals.processing.outboundFlitQueFull[PORTA]);
				EnqueOutboundFlit(flit,PORTA);
			}
			if(gpioDataRegisters.GPBDAT.bit.SWITCH2)
			{
				while(globals.processing.outboundFlitQueFull[PORTB]);
				EnqueOutboundFlit(flit,PORTB);
			}
			if(gpioDataRegisters.GPADAT.bit.SWITCH3)
			{
				while(globals.processing.outboundFlitQueFull[PORTC]);
				EnqueOutboundFlit(flit,PORTC);
			}
			if(gpioDataRegisters.GPADAT.bit.SWITCH4)
			{
				while(globals.processing.outboundFlitQueFull[PORTD]);
				EnqueOutboundFlit(flit,PORTD);
			}
			numTransmits++;
			numTransmitsInGroup++;
		}
		numGroups++;
		SetSevenSegmentDisplay(numGroups);
	}

	SEM_pend(&TestServiceSem,SYS_FOREVER);
#endif
}

#pragma CODE_SECTION(ProtocolTest1,"FlashCode");
void ProtocolTest1()
{
#if defined(IS_ROUTER)
#if PROFILE_TEST_SERVICE == true
	double executionTime;
	Uint32 startTime;
#endif
	struct Packet newPacket;
	Uint16 x[100], i, j;

	for(i = 0; i < 100; i++)
		x[i] = i;

	TSK_sleep(3000);

	if(gpioDataRegisters.GPBDAT.bit.SWITCH1)
	{
		for(i = 0; i < 100; i++)
		{
			for(j = 0; j < 50; j++)
			{
				InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
				
				// Create the data transfer packet
				newPacket.a.communicationType = COMM_TYPE_UNICAST;
				newPacket.transmissionInfo.destination = ROOT_ADDRESS;
				newPacket.b.command = COMMAND_DATA_TRANSFER;
				newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;

				// Set the data
				newPacket.dataBuffer = x;
				newPacket.dataBufferInfo.dataBufferLength = 100;

#if PROFILE_TEST_SERVICE == true
				startTime = timer0Registers.TIM.all;
#endif
				SendDataPacket(&newPacket);
				while(globals.statistics.packet.numDataTransfersSucceeded + 
					globals.statistics.packet.numDataTransfersExpired + 
					globals.statistics.packet.numDataTransfersFailed < 
					globals.statistics.packet.numDataTransfersSetup);

#if PROFILE_TEST_SERVICE == true
				executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
				globals.statistics.profiling.avgTestServiceTime = 
					(globals.statistics.profiling.avgTestServiceTime * 
					globals.statistics.profiling.numTestServiceSamples) + executionTime;
				globals.statistics.profiling.numTestServiceSamples++;
				globals.statistics.profiling.avgTestServiceTime /= 
					globals.statistics.profiling.numTestServiceSamples;
				if(executionTime < globals.statistics.profiling.minTestServiceTime)
					globals.statistics.profiling.minTestServiceTime = executionTime;
				else if(executionTime > globals.statistics.profiling.maxTestServiceTime)
					globals.statistics.profiling.maxTestServiceTime = executionTime;
#endif
			}
			SetSevenSegmentDisplay(i);
		}
	}
#endif
}

#pragma CODE_SECTION(ProtocolTest2,"FlashCode");
void ProtocolTest2()
{
#if defined(IS_ROOT)
	struct Packet newPacket;
	Uint16 x[100], i, j;
#if PROFILE_TEST_SERVICE == true
	double executionTime;
	Uint32 startTime;
#endif

	for(i = 0; i < 100; i++)
		x[i] = i;

	TSK_sleep(3000);

	for(i = 0; i < 100; i++)
	{
		for(j = 0; j < 50; j++)
		{
			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			
			// Create the data transfer packet
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.transmissionInfo.destination = 2;
			newPacket.b.command = COMMAND_DATA_TRANSFER;
			newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;

			// Set the data
			newPacket.dataBuffer = x;
			newPacket.dataBufferInfo.dataBufferLength = 10;

			SendDataPacket(&newPacket);

			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			
			// Create the data transfer packet
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.transmissionInfo.destination = 3;
			newPacket.b.command = COMMAND_DATA_TRANSFER;
			newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;

			// Set the data
			newPacket.dataBuffer = x;
			newPacket.dataBufferInfo.dataBufferLength = 10;

			SendDataPacket(&newPacket);

			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			
			// Create the data transfer packet
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.transmissionInfo.destination = 4;
			newPacket.b.command = COMMAND_DATA_TRANSFER;
			newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;

			// Set the data
			newPacket.dataBuffer = x;
			newPacket.dataBufferInfo.dataBufferLength = 10;

#if PROFILE_TEST_SERVICE == true
			startTime = timer0Registers.TIM.all;
#endif
#if PROFILE_LATENCY == true
			globals.statistics.profiling.latencyStartTime = timer0Registers.TIM.all;
#endif
			SendDataPacket(&newPacket);

			while(globals.statistics.packet.numDataTransfersSucceeded + 
				globals.statistics.packet.numDataTransfersExpired + 
				globals.statistics.packet.numDataTransfersFailed < 
				globals.statistics.packet.numDataTransfersSetup);

#if PROFILE_TEST_SERVICE == true
			executionTime = TimeDifference(startTime, timer0Registers.TIM.all);
			globals.statistics.profiling.avgTestServiceTime = 
				(globals.statistics.profiling.avgTestServiceTime * 
				globals.statistics.profiling.numTestServiceSamples) + executionTime;
			globals.statistics.profiling.numTestServiceSamples++;
			globals.statistics.profiling.avgTestServiceTime /= 
				globals.statistics.profiling.numTestServiceSamples;
			if(executionTime < globals.statistics.profiling.minTestServiceTime)
				globals.statistics.profiling.minTestServiceTime = executionTime;
			else if(executionTime > globals.statistics.profiling.maxTestServiceTime)
				globals.statistics.profiling.maxTestServiceTime = executionTime;
#endif
		}
		SetSevenSegmentDisplay(i);
	}
#endif
}


#ifdef __cplusplus
#pragma DATA_SECTION("MpiData")
#else
#pragma DATA_SECTION(xglobal,"MpiData");
#endif
int xglobal[700];
#ifdef __cplusplus
#pragma DATA_SECTION("MpiData")
#else
#pragma DATA_SECTION(yglobal,"MpiData");
#endif
int yglobal[700];
#ifdef __cplusplus
#pragma DATA_SECTION("MpiData")
#else
#pragma DATA_SECTION(resultglobal,"MpiData");
#endif
int resultglobal[1399];

#pragma CODE_SECTION(MPITest,"FlashCode");
void MPITest()
{
	int rank, numNodes;
	Uint16 sendBuf[1], *receiveBuf;
	Uint16 i, j;
	MPI_Status status;
	Uint32 startTime;
	double elapsedTime;

	SEM_pend(&TestServiceSem,SYS_FOREVER);

	// Generate the x data
	for(i = 0; i < 14; i++)
	{
		for(j = 0; j < 25; j++)
			xglobal[i * 50 + j] = (j / 4);
		for(j = 0; j < 25; j++)
			xglobal[i * 50 + j + 25] = 6 - (j / 4);
	}

	// Generate the y data
	for(i = 0; i < 28; i++)
	{
		for(j = 0; j < 25; j++)
			yglobal[i * 25 + j] = j / 2;
	}

	// Initialize MPI
	MPI_Init(NULL,NULL);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	globals.processing.sevenSegmentUpperDigit = SEVENSEG_1DASH;

	// Distribute the number of nodes to use
	if(rank == 0)
	{
		numNodes = (gpioDataRegisters.GPBDAT.bit.SWITCH1 << 3) + 
			(gpioDataRegisters.GPBDAT.bit.SWITCH2 << 2) + 
			(gpioDataRegisters.GPADAT.bit.SWITCH3 << 1) + 
			(gpioDataRegisters.GPADAT.bit.SWITCH4);
		sendBuf[0] = numNodes;
		for(i = 1; i <= 6; i++)
			MPI_Send(sendBuf,1,MPI_SHORT,i,0,MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv((void**)(&receiveBuf),1,MPI_SHORT,0,0,MPI_COMM_WORLD,&status);
		numNodes = receiveBuf[0];
		MemFree(receiveBuf);
	}
	MPI_Barrier(MPI_COMM_WORLD);

	// If this node is participating in the convolution
	if(rank < numNodes)
	{
		globals.processing.sevenSegmentUpperDigit = SEVENSEG_2DASH;

		startTime = timer0Registers.TIM.all;
		Convolution(xglobal,yglobal,100,100,resultglobal,numNodes);
		elapsedTime = TimeDifference(startTime, timer0Registers.TIM.all);

		// Finalize MPI
		if(rank == 0)
		{
			globals.processing.sevenSegmentLowerDigit = SEVENSEG_FINAL;
			globals.processing.sevenSegmentUpperDigit = SEVENSEG_FINAL;
		}
		else
			globals.processing.sevenSegmentUpperDigit = SEVENSEG_3DASH;
	}
	else
	{
		globals.processing.sevenSegmentUpperDigit = SEVENSEG_FINAL;
	}
	SEM_pend(&TestServiceSem,SYS_FOREVER);
}

#pragma CODE_SECTION(Convolution,"FlashCode");
void Convolution(int* x, int* y, int xLength, int yLength, int* result, int numNodesToUse)
{
	int n, m, resultLength = xLength + yLength - 1, rank, transfersIncomplete, flag;
	int *nodeResults[MAX_NUM_NODES];
	int receivedResults[MAX_NUM_NODES] = 
		{false,false,false,false,false,false,false,false,false};
	MPI_Status status;
	MPI_Request recvRequests[MAX_NUM_NODES];

	// Initialize data
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	for(n = 0; n < resultLength; n++)
		result[n] = 0;

	// x[n] conv y[n] = sum(x[m] * y[n-m] over m)
	if(rank == 0)
	{
		// Convolve the signals
		for(n = 0; n < resultLength / numNodesToUse; n++)
		{
			result[n] = 0;
			for(m = n - yLength; m < n + yLength; m++)
			{
				if((m >= 0 && m < xLength) && (n - m >= 0 && n - m < yLength))
					result[n] += x[m] * y[n - m];
			}
		}
	}
	else
	{
		// Convolve the signals
		for(n = 0; n < (rank * resultLength) / numNodesToUse; n++)
			result[n] = 0;
		for(n = (rank * resultLength) / numNodesToUse; n < ((rank + 1) * resultLength) / 
			numNodesToUse; n++)
		{
			result[n] = 0;
			for(m = n - yLength; m < n + yLength; m++)
			{
				if((m >= 0 && m < xLength) && (n - m >= 0 && n - m < yLength))
					result[n] += x[m] * y[n - m];
			}
		}
		for(n = ((rank + 1) * resultLength) / numNodesToUse; n < resultLength; n++)
			result[n] = 0;
	}

	if(numNodesToUse > 1)
	{
		// Collect the data
		if(rank == 0)
		{	
			// Initiate the receive requests
			for(n = 1; n < numNodesToUse; n++)
				MPI_Irecv((void**)(&(nodeResults[n])),resultLength,MPI_SHORT,n,0,
					MPI_COMM_WORLD,&(recvRequests[n]));

			// Test the status of the receives
			transfersIncomplete = true;
			while(transfersIncomplete)
			{
				transfersIncomplete = false;
				for(n = 1; n < numNodesToUse; n++)
				{
					if(!receivedResults[n])
					{
						// Test the status of the receive from node of rank n
						MPI_Test(&(recvRequests[n]),&flag,&status);

						// If the transfer is complete, save the results
						if(flag)
						{
							// Save the results
							receivedResults[n] = true;
							for(m = (n * resultLength) / numNodesToUse; 
								m < ((n + 1) * resultLength) / numNodesToUse; m++)
								result[m] = nodeResults[n][m - (n * resultLength) / 
								numNodesToUse];
							MemFree((Uint16*)(nodeResults[n]));
						}
						else
							transfersIncomplete = true;
					}
				}
			}
		}
		else
		{
			TSK_sleep(100);
			MPI_Send(&(result[(rank * resultLength) / numNodesToUse]),
				((rank + 1) * resultLength) / numNodesToUse - (rank * resultLength) / 
				numNodesToUse,MPI_SHORT,0,0,MPI_COMM_WORLD);
		}
	}
}
