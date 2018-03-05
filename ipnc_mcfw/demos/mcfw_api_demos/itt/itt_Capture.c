/** ==================================================================
 *  @file   itt_Capture.c                                                  
 *                                                                    
 *  @path    ipnc_mcfw/demos/mcfw_api_demos/itt/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*
 *  Notify from A8 to VIDEO M3 functions
 */

#include <string.h>
#include <mcfw/interfaces/ti_media_std.h>
#include <itt_Capture.h>
#include <ti/syslink/Std.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/SysLink.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static TaskCtx ittThrHandle;

#define ITT_ASSERT( COND )                                 // COND

#define ITT_PRINTF( MSG )     OSA_printf(MSG)

#define ITT_SERVER_MSGQ_NAME      "ITT_SERVERQ"
#define DCC_SERVER_MSGQ_NAME      "DCC_SERVERQ"
/**
 *  Handle to the ITT Server Message Queue
 */
MessageQ_Handle hIttMsgQ = NULL;

typedef struct ittServer_Msg {
    MessageQ_MsgHeader header;
    Int32 response;
    Int32 arg1;
    Int32 arg2;

} ittServer_Msg;

#define CMD_CKH_CONNECTION   (1)
#define CMD_SEND_DCC         (2)
#define CMD_MEM_READ_WORD    (3)
#define CMD_SAVE_RAW         (4)
#define CMD_REGS_READ        (5)
#define CMD_MEM_WRITE_WORD   (6)
#define CMD_RECV_DCC         (7)
#define CMD_CAMERA_CNTL      (8)
#define CMD_SAVE_YUV         (9)
#define CMD_MEM_READ_BUFF    (10)
#define CMD_SENSOR_REGS_READ (11)
#define CMD_SENSOR_REG_WRITE (12)

#define ITT_SHAREDREGION_ID (0)

/**
 *******************************************************************************
 *  @func       RfileServer_cleanup
 *  @brief      This function uninitializes the Remote File Server
 *
 *  @retval Rfile_SUCCESS   Remote File Server uninitialization successful
 *  @retval Rfile_FAIL      Error in uninitializing Remote File Server
 *
 *  @remarks    After calling RfileServer_cleanup, no further Remote File
 *  operations should be attempted.
 *
 *******************************************************************************
 */
/* ===================================================================
 *  @func     ittServerMsgQ_cleanup                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 ittServerMsgQ_cleanup()
{
    MessageQ_delete(&hIttMsgQ);
    return (1);
}

#define ITT_SERVER_DEFAULT_PORT    (5000)
static int serverPort;

static int serverSocketId;

/* ===================================================================
 *  @func     ittServer_CreateSocket                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int ittServer_CreateSocket(unsigned int port)
{
    struct sockaddr_in server;

    int status;

    serverPort = port;

    serverSocketId = socket(AF_INET, SOCK_STREAM, 0);
    ITT_ASSERT(serverSocketId < 0) server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);
    memset((void *) &(server.sin_zero), 0, 8);

    status = bind(serverSocketId, (struct sockaddr *) &server, sizeof(server));
    ITT_ASSERT(status == -1);

    status = listen(serverSocketId, 5);
    ITT_ASSERT(status < 0);

    return (0);
}

/* ===================================================================
 *  @func     ittServer_SendData                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int ittServer_SendData(int SocketId, unsigned char *dataBuf,
                       unsigned int dataSize)
{
    int actDataSize = 0;

    while (dataSize > 0)
    {
        actDataSize = send(SocketId, dataBuf, dataSize, 0);
        OSA_printf(" ITT_Server: Send Data %d (%d)\n", actDataSize, dataSize);
        if (actDataSize <= 0)
            break;
        dataBuf += actDataSize;
        dataSize -= actDataSize;
    }

    if (dataSize > 0)
    {
        ITT_ERROR(" ITT_Server: Failed to send the data\n");
        return (-1);
    }

    return (0);
}

/* ===================================================================
 *  @func     ittServer_RecvData                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int ittServer_RecvData(int SocketId, unsigned char *dataBuf,
                       unsigned int dataSize)
{
    int actDataSize = 0;

    while (dataSize > 0)
    {
        actDataSize = recv(SocketId, dataBuf, dataSize, 0);
        OSA_printf(" ITT_Server: Recived Data %d (%d)\n", actDataSize, dataSize);

        if (actDataSize <= 0)
            break;
        dataBuf += actDataSize;
        dataSize -= actDataSize;
    }

    if (dataSize > 0)
    {
        ITT_ERROR(" ITT_Server: Failed to recicve the data\n");
        return (-1);
    }

    return (0);
}

/* ===================================================================
 *  @func     ittServer_run                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MessageQ_QueueId remoteQ; 
Int32 ittServer_run()
{
    Int32 status = 0;

    Bits16 cmd = 1;

    ittServer_Msg *ittMsg = NULL;

    remoteQ = 0;

    MessageQ_Msg msg = NULL;

    Ptr srHeap = NULL;

    MessageQ_Params msgQParams;

    //int cmdCounter = 1;

    unsigned int sin_size;

    struct sockaddr_in client;

    int connectedSocketId;

    int dataSize;

    // unsigned char dataBuf[100*1024];
    Int32 recvData[5];

    Int32 payLoadSize;

    Bits32 srPtr = 0;

    Ptr buf = NULL;

    OSA_printf("\n ITT ittServer_run\n");
    /*--------------------------------------------------------------------------
    * Get the heap associated with the Shared Region for Message Queue. This
    * heap is used to create Message Queues, allocate Messages subsequently.
    *------------------------------------------------------------------------*/
    srHeap = SharedRegion_getHeap(ITT_SHAREDREGION_ID);
    ITT_ASSERT(srHeap != NULL);
    /*--------------------------------------------------------------------------
    * Create a message queue for Remote File Server. The client modules send
    * their requests to this message queue
    *------------------------------------------------------------------------*/
    MessageQ_Params_init(&msgQParams);
    hIttMsgQ = MessageQ_create(ITT_SERVER_MSGQ_NAME, &msgQParams);
    ITT_ASSERT(hIttMsgQ != NULL);

    if(hIttMsgQ == NULL)
	{
		goto EXIT;
	}	
	
    msg = MessageQ_alloc(ITT_SHAREDREGION_ID, sizeof(ittServer_Msg));
    if (msg == NULL)
    {
        OSA_printf("ERROR: Can't allocate message for [%s]\n",
               ITT_SERVER_MSGQ_NAME);
        MessageQ_delete(&hIttMsgQ);
        return (0);
    }
    ittMsg = (ittServer_Msg *) msg;
    OSA_printf("ITT Server Message initialization successful\n");
    do
    {
        status = MessageQ_open(DCC_SERVER_MSGQ_NAME, &remoteQ);
    } while (status == MessageQ_E_NOTFOUND);
    OSA_printf("DCC server Message ques is open succefully\n");

    // while (1)
    {

        ittMsg->arg1 = 1024 * 1024;
        /*--------------------------------------------------------------------------
        * Specify the command to be used
        *------------------------------------------------------------------------*/
        cmd = CMD_CKH_CONNECTION;
        OSA_printf("Setting cmd <%d> in message <%x>\n", cmd, (UInt32)msg);
        MessageQ_setMsgId(msg, cmd);
		
		/*--------------------------------------------------------------------------
        * Specify the MessageQ owned by this Task as the reply queue. The Remote
        * File Server will reply to the queue embedded in this message
        *------------------------------------------------------------------------*/
        MessageQ_setReplyQueue(hIttMsgQ, msg);
        /*--------------------------------------------------------------------------
        * Send the command to the Remote File Server
        *------------------------------------------------------------------------*/
        OSA_printf("Posting message <%x> in QId <%x> \n", (UInt32)msg, (UInt32)remoteQ);
        status = MessageQ_put(remoteQ, msg);
        ITT_ASSERT(status == MessageQ_S_SUCCESS);

        /*--------------------------------------------------------------------------
        * Wait for a response from the Remote File Server
        *------------------------------------------------------------------------*/
        status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
        ITT_ASSERT(status == MessageQ_S_SUCCESS);
		
		if(msg == NULL)
		{
		    goto EXIT;
		}
		
        OSA_printf("Response from M3 is : %d\n", ittMsg->response);

        srPtr = (Bits32) ittMsg->arg1;

        /*--------------------------------------------------------------------------
         * Check if shared region pointer is valid
         *------------------------------------------------------------------------*/
        if (srPtr == 0)
        {
            OSA_printf("Invalid sr Pointer \n");
            goto EXIT;
        }
        /*--------------------------------------------------------------------------
         * Check if SR pointer can be translated to a valid local pointer
         *------------------------------------------------------------------------*/
        else if ((buf = SharedRegion_getPtr(srPtr)) == NULL)
        {
            OSA_printf("Invalid sr translation \n");
            goto EXIT;
        }
    }
#if 1
    /* send the default dcc data if exists to M3 */
    {
        FILE *fp;

        int lSize, result;

        fp = fopen("/opt/ipnc/dcc/dcc_default.bin", "rb");
        if (fp == NULL)
        {
            OSA_printf
                ("Default dcc profile does not exists. Using system defaults ... \n");
        }
        else                                               /* Default profile
                                                            * exists */
        {

            // obtain file size:
            fseek(fp, 0, SEEK_END);
            lSize = ftell(fp);
            rewind(fp);
            result = fread(buf, 1, lSize, fp);
            if (result != lSize)
            {
                OSA_printf("Unable to read complete dcc file ... \n");
            }
            else
            {
                /* Send a message to M3 asking to read the dcc data */
                cmd = CMD_SEND_DCC;                                 // 2 is for dcc
                                                           // TODO nee to
                                                           // make it enum
                ittMsg->arg1 = lSize;
                MessageQ_setMsgId(msg, cmd);

                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
				
				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);
                OSA_printf("Default parameters were sent sucessfully \n");
            }
            fclose(fp);

        }
    }
#endif
    ittServer_CreateSocket(ITT_SERVER_DEFAULT_PORT);
    OSA_printf("Server Socket created with ID <%d> \n", serverSocketId);

    while (1)
    {
        sin_size = sizeof(struct sockaddr_in);
        connectedSocketId =
            accept(serverSocketId, (struct sockaddr *) &client, &sin_size);

        if (connectedSocketId < 0)
        {
            ITT_ERROR("Illegal client socket ID\n");
            goto EXIT;
        }
        OSA_printf(" ITT Server: Connected to client\n");
        while (1)
        {
            dataSize = 20;
            status = ittServer_RecvData(connectedSocketId, (unsigned char *)recvData, dataSize);
            if (status == -1)
            {
                break;
            }
            // recvData = (Int32 * )buf;
            /* Checking connectivity */
            if (recvData[0] == CMD_CKH_CONNECTION)
            {
                OSA_printf("Client checking the connectivity \n");
                break;
            }
            /* dcc data */
            else if (recvData[0] == CMD_SEND_DCC)
            {


                int updateDefault;

                cmd = recvData[0];
                payLoadSize = recvData[1];

                updateDefault = recvData[2];

                /* Get the dcc data into buffer */
                status =
                    ittServer_RecvData(connectedSocketId, buf, payLoadSize);
                if (status != -1)
                {
                    OSA_printf("Server recived <%d> bytes from client \n",
                           payLoadSize);
                }

                if (updateDefault == 1)
                {
                    FILE *fp = fopen("./dcc/dcc_default.bin", "wb");
                    if (fp == NULL)
                    {
                        OSA_printf
                            ("error in opening the default file dcc_default.bin. Unable to update \n");
                    }
                    else
                    {

                        fwrite(buf, 1, payLoadSize, fp);
                        OSA_printf("default update is sucessful \n");
                        fclose(fp);
                    }
                }
                ittMsg->arg1 = payLoadSize;
                MessageQ_setMsgId(msg, cmd);

                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
				
				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);

                recvData[0] = ittMsg->response;
                recvData[1] = 0;
                recvData[2] = 0;
                recvData[3] = 0;
                recvData[4] = 0;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                break;

            }
            /* Reg read */
            else if (recvData[0] == CMD_MEM_READ_WORD)
            {

                cmd = recvData[0];
                ittMsg->arg1 = recvData[1];
                MessageQ_setMsgId(msg, cmd);

                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
				
				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);

                recvData[0] = ittMsg->response;
                recvData[1] = ittMsg->arg1;
                recvData[2] = 0;
                recvData[3] = 0;
                recvData[4] = 0;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                break;

            }
            /* Request for RAW data */
            else if (recvData[0] == CMD_SAVE_RAW)
            {
                //int i;

                //int *buf_ptr = (int *) buf;

                cmd = recvData[0];
                MessageQ_setMsgId(msg, cmd);
                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
                do
                {
                    status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                    OSA_printf("Got a new message \n");
                    ITT_ASSERT(status == MessageQ_S_SUCCESS);
					
					if(msg == NULL)
					{
						goto EXIT;
					}					
					
                    recvData[0] = ittMsg->response;
                    recvData[1] = ittMsg->arg1;
                    recvData[2] = 0;
                    recvData[3] = 0;
                    recvData[4] = 0;

                    if (ittMsg->arg1 != -1)
                    {
                        OSA_printf("Response from M3 is : %d\n", ittMsg->response);
                        OSA_printf("Data to transfer = %d\n", ittMsg->arg1);
                        status =
                            ittServer_SendData(connectedSocketId, (unsigned char *)recvData,
                                               dataSize);

                        status =
                            ittServer_SendData(connectedSocketId, buf,
                                               ittMsg->arg1);
                        /* { int i; for (i = 0; i < 10; i++) { OSA_printf("%x",
                         * ((int*)buf)[i]); } } */
                        // Put
                        status = MessageQ_put(remoteQ, msg);
                        ITT_ASSERT(status == MessageQ_S_SUCCESS);
                    }
                    else
                    {
                        break;
                    }
                } while (1);
                recvData[1] = -1;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                break;
            }
            else if (recvData[0] == CMD_REGS_READ)
            {
                //int i;

                //int *buf_ptr = (int *) buf;

                cmd = recvData[0];
                ittMsg->arg1 = recvData[1];
                ittMsg->arg2 = recvData[2];
                MessageQ_setMsgId(msg, cmd);
                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);
                if (ittMsg->arg1 > 0)
                {
                    recvData[0] = ittMsg->response;
                    recvData[1] = ittMsg->arg1;
                    recvData[2] = ittMsg->arg2;
                    recvData[3] = 0;
                    recvData[4] = 0;
                    OSA_printf("Data to transfer = %d\n", ittMsg->arg1);
                    status =
                        ittServer_SendData(connectedSocketId, (unsigned char *)recvData,
                                           dataSize);
                    status =
                        ittServer_SendData(connectedSocketId, buf,
                                           ittMsg->arg1);
                }
                break;
            }
            else if ((recvData[0] == CMD_MEM_READ_BUFF) ||
                     (recvData[0] == CMD_SENSOR_REGS_READ))
            {
                //int i;

                //int *buf_ptr = (int *) buf;

                cmd = recvData[0];
                ittMsg->arg1 = recvData[1];
                ittMsg->arg2 = recvData[2];
                MessageQ_setMsgId(msg, cmd);
                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);
                if (ittMsg->arg1 > 0)
                {
                    recvData[0] = ittMsg->response;
                    recvData[1] = ittMsg->arg1;
                    recvData[2] = ittMsg->arg2;
                    recvData[3] = 0;
                    recvData[4] = 0;
                    OSA_printf("Data to transfer = %d\n", ittMsg->arg1);
                    status =
                        ittServer_SendData(connectedSocketId, (unsigned char *)recvData,
                                           dataSize);
                    status =
                        ittServer_SendData(connectedSocketId, buf,
                                           ittMsg->arg1);
                }
                break;
            }                                              /* Reg Write */
            else if ((recvData[0] == CMD_MEM_WRITE_WORD) ||
                     (recvData[0] == CMD_SENSOR_REG_WRITE))
            {

                cmd = recvData[0];
                ittMsg->arg1 = recvData[1];
                ittMsg->arg2 = recvData[2];
                MessageQ_setMsgId(msg, cmd);

                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
				
				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);

                recvData[0] = ittMsg->response;
                recvData[1] = 0;
                recvData[2] = 0;
                recvData[3] = 0;
                recvData[4] = 0;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                break;

            }
            else if (recvData[0] == CMD_RECV_DCC)
            {

                cmd = recvData[0];
                MessageQ_setMsgId(msg, cmd);
                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
				
				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);

                recvData[0] = ittMsg->response;
                recvData[1] = ittMsg->arg1;
                recvData[2] = 0;
                recvData[3] = 0;
                recvData[4] = 0;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                status =
                    ittServer_SendData(connectedSocketId, buf, ittMsg->arg1);
                break;

            }
            else if (recvData[0] == CMD_CAMERA_CNTL)
            {

                cmd = recvData[0];
                payLoadSize = recvData[1];

                /* Control Parameters into buffer */
                status =
                    ittServer_RecvData(connectedSocketId, buf, payLoadSize);
                if (status != -1)
                {
                    OSA_printf("Server recived <%d> bytes from client \n",
                           payLoadSize);
                }

                ittMsg->arg1 = payLoadSize;
                MessageQ_setMsgId(msg, cmd);

                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);

                status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
				
				if(msg == NULL)
				{
					goto EXIT;
				}				
				
                OSA_printf("Response from M3 is : %d\n", ittMsg->response);

                recvData[0] = ittMsg->response;
                recvData[1] = 0;
                recvData[2] = 0;
                recvData[3] = 0;
                recvData[4] = 0;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                break;
            }
            else if (recvData[0] == CMD_SAVE_YUV)
            {
                //int i;

                //int *buf_ptr = (int *) buf;

                cmd = recvData[0];
                MessageQ_setMsgId(msg, cmd);
                MessageQ_setReplyQueue(hIttMsgQ, msg);

                status = MessageQ_put(remoteQ, msg);
                ITT_ASSERT(status == MessageQ_S_SUCCESS);
                do
                {
                    status = MessageQ_get(hIttMsgQ, &msg, MessageQ_FOREVER);
                    OSA_printf("Got a new message \n");
                    ITT_ASSERT(status == MessageQ_S_SUCCESS);
					
					if(msg == NULL)
					{
						goto EXIT;
					}					
					
                    recvData[0] = ittMsg->response;
                    recvData[1] = ittMsg->arg1;
                    recvData[2] = 0;
                    recvData[3] = 0;
                    recvData[4] = 0;

                    if (ittMsg->arg1 != -1)
                    {
                        OSA_printf("Response from M3 is : %d\n", ittMsg->response);
                        OSA_printf("Data to transfer = %d\n", ittMsg->arg1);
                        status =
                            ittServer_SendData(connectedSocketId, (unsigned char *)recvData,
                                               dataSize);
                        status =
                            ittServer_SendData(connectedSocketId, buf,
                                               ittMsg->arg1);
                        status = MessageQ_put(remoteQ, msg);
                        ITT_ASSERT(status == MessageQ_S_SUCCESS);
                    }
                    else
                    {
                        break;
                    }
                } while (1);
                recvData[1] = -1;
                status =
                    ittServer_SendData(connectedSocketId, (unsigned char *)recvData, dataSize);
                break;
            }
            else
            {
                OSA_printf("Un-supported command \n");
                break;
            }
        }
        OSA_printf(" ITT Server: Disconnected from client\n");
    }

    return (1);
  EXIT:
    ITT_ERROR("ITT server exiting with erro\n");
    return (0);

}

/* ===================================================================
 *  @func     IttServer_Task                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void *IttServer_Task(void *taskHndl)
{
    OSA_printf("ITT server task running !!!\n");

    ittServer_run();

    OSA_printf("ITT server task Exiting \n");

    return NULL;
}

/* ===================================================================
 *  @func     Itt_TaskCreate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Itt_TaskCreate()
{
    Int32 status = ITT_STATUS_OK;

    status = Itt_thrCreate(&ittThrHandle,
                           (ThrEntryFunc) IttServer_Task,
                           ITT_TSK_PRI, ITT_TSK_STACK_SIZE);
    OSA_printf("\n ITT capture task created\n");

    UTILS_assert(status == ITT_STATUS_OK);
    return status;
}

/* Date Time task delete */

/* ===================================================================
 *  @func     Itt_TaskDelete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int Itt_TaskDelete()
{

    Itt_thrDelete(&ittThrHandle);
    return 0;
}

/* ===================================================================
 *  @func     Itt_thrCreate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Itt_thrCreate(TaskCtx * ctx, ThrEntryFunc entryFunc, Uint32 pri,
                    Uint32 stackSize)
{
    Int32 status = ITT_STATUS_OK;

    pthread_attr_t thread_attr;

    struct sched_param schedprm;

    if (ctx == NULL || entryFunc == NULL)
    {
        ITT_ERROR("Input param error\n");
        return ITT_STATUS_EFAIL;
    }

    // initialize thread attributes structure
    status = pthread_attr_init(&thread_attr);

    if (status != ITT_STATUS_OK)
    {
        ITT_ERROR("Itt_thrCreate() - Could not initialize thread attributes\n");
        return status;
    }

    if (stackSize != 0)
        pthread_attr_setstacksize(&thread_attr, stackSize);

    status |=
        pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED);
    status |= pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);

    if (pri > sched_get_priority_max(SCHED_FIFO))
        pri = sched_get_priority_max(SCHED_FIFO);
    else if (pri < sched_get_priority_min(SCHED_FIFO))
        pri = sched_get_priority_min(SCHED_FIFO);

    schedprm.sched_priority = pri;
    status |= pthread_attr_setschedparam(&thread_attr, &schedprm);

    if (status != ITT_STATUS_OK)
    {
        ITT_ERROR("Itt_thrCreate() - Could not initialize thread attributes\n");
        goto error_exit;
    }

    ctx->exitFlag = 0;
    status = pthread_create(&ctx->handle, &thread_attr, entryFunc, ctx);

    if (status != ITT_STATUS_OK)
    {
        ITT_ERROR("Itt_thrCreate() - Could not create thread [%d]\n", status);
    }

  error_exit:
    pthread_attr_destroy(&thread_attr);

    return status;
}

/* ===================================================================
 *  @func     Itt_thrDelete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Itt_thrDelete(TaskCtx * ctx)
{
    Int32 status = ITT_STATUS_OK;

    void *retVal;

    if (NULL == ctx)
    {
        ITT_ERROR("Itt_thrDelete() - handle is NULL\n");
        return ITT_STATUS_EFAIL;
    }

	status = close(serverSocketId);
	UTILS_assert(status == MessageQ_S_SUCCESS);
	status = MessageQ_close(&remoteQ);
	UTILS_assert(status == MessageQ_S_SUCCESS);

	MessageQ_unblock(hIttMsgQ);
	status = MessageQ_delete(&hIttMsgQ);
	UTILS_assert(status == MessageQ_S_SUCCESS);

    status |= pthread_cancel(ctx->handle);
    ctx->exitFlag = 1;
    if (pthread_join(ctx->handle, &retVal))
    {
        ITT_ERROR("Itt_thrDelete() - handle is NULL\n");
    }

    return status;
}
