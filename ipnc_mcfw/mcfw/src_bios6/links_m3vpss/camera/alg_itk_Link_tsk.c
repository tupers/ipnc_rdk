/** ==================================================================
 *  @file   alg_itk_Link_tsk.c
 *
 *  @path    ipnc_mcfw/mcfw/src_bios6/links_m3vpss/camera/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    This file implements the state machine logic for this link.
    A message command will cause the state machine to take some action and then
    move to a different state.

    The state machine table is as shown below

    Cmds| CREATE | DETECT_VIDEO | START | NEW_DATA  | STOP   | DELETE |
 States |========|==============|=======|===========|========|========|
   IDLE | READY  | -            | -     | -         | -      | -      |
   READY| -      | READY        | RUN   | -         | READY  | IDLE   |
   RUN  | -      | -            | -     | RUN       | READY  | IDLE   |

 */

// #include "alg_ITK_Link_priv.h"
#include "cameraLink_priv.h"
#include "alg_itk_Link_priv.h"

#include <ti/psp/iss/alg/dcc/inc/idcc.h>
#include <ti/psp/devices/mt9j003/issdrv_mt9j003.h>

SensorControlParams_t gSensorControlParams;

extern Int32 gVstabEnable;

/* link stack */
#pragma DATA_ALIGN(gAlg_ITK_Link_tskStack, 32)
#pragma DATA_SECTION(gAlg_ITK_Link_tskStack, ".bss:taskStackSection")

UInt8 gAlg_ITK_Link_tskStack[CAMERA_LINK_TSK_STACK_SIZE];

extern iss_drv_config_t *iss_drv_config;

/* ===================================================================
 *  @func     TI_Increase_Exposure
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
extern TI_Increase_Exposure();
/* ===================================================================
 *  @func     TI_Decrease_Exposure
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
extern TI_Decrease_Exposure();

Itt_RegisterParams gItt_RegisterParams;

volatile int ti_2a_run = 1;

volatile int dcc_parser_run = 1;

volatile int yuv_data_ready;

volatile int request_yuv_data;

volatile int YUVDumpSize;

uint32 *isif_dump;

uint32 isif_dump_size;
static uint32 ITT_ExitDone = TRUE;
static uint32 Force_unblock = FALSE;
extern ti2a_output ti2a_output_params;
extern ti2aControlParams_t gTi2aControlParams;

/* ===================================================================
 *  @func     writeSensorReg
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
int writeSensorReg(uint32 addr, uint32 data);

/* ===================================================================
 *  @func     readSensorReg
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
int readSensorReg(uint32 addr, uint32 * data);

/* ===================================================================
 *  @func     readSensorReg
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
int readSensorReg(uint32 addr, uint32 * data)
{
    int status = 0;

    return status;
}

/* ===================================================================
 *  @func     writeSensorReg
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
int writeSensorReg(uint32 addr, uint32 data)
{
    int status = 0;

    /* int status,devAddr,count = 0; I2c_Ctrl mt9j003_I2c_ctrl;
     * mt9j003_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId(); devAddr =
     * MT9J_003_ADDR; mt9j003_I2c_ctrl.regAddr[count] = addr;
     * mt9j003_I2c_ctrl.regValue[count] = data; mt9j003_I2c_ctrl.numRegs = 1;
     * status =
     * Iss_deviceWrite16(mt9j003_I2c_ctrl.i2cInstId,devAddr,&mt9j003_I2c_ctrl.regAddr[count],&mt9j003_I2c_ctrl.regValue[count],mt9j003_I2c_ctrl.numRegs);
     */
    return status;
}

/* IDLE and READY state implementation */
/* ===================================================================
 *  @func     Alg_ITK_Link_tskMain
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
Void Alg_ITK_Link_tskMain(struct Utils_TskHndl * pTsk, Utils_MsgHndl * pMsg)
{

    Int32 status = 0;

    Bits16 cmd = 0;

    ittServer_Msg *ittMsg = NULL;

    MessageQ_QueueId sender = 0;

    MessageQ_Msg msg = NULL;

    Ptr srHeap = NULL;

    MessageQ_Params msgQParams;

    SharedRegion_SRPtr srPtr = NULL;

    Int32 dccMemSize = 0;

    Ptr buf = NULL;

    CameraLink_Obj *pObj;

    CameraLink_InstObj *pInst;
	
    pObj = (CameraLink_Obj *) pTsk->appData;
    UInt16 instId = 0;

    pInst = &pObj->instObj[instId];

    Vps_rprintf("DCC server task running \n");
    /*--------------------------------------------------------------------------
    * Get the heap associated with the Shared Region for Message Queue. This
    * heap is used to create Message Queues, allocate Messages subsequently.
    *------------------------------------------------------------------------*/
    srHeap = SharedRegion_getHeap(ITT_SHAREDREGION_ID);
    UTILS_assert(srHeap != NULL);

    /*--------------------------------------------------------------------------
    * Register this shared region heap with the Message Queue module
    *------------------------------------------------------------------------*/

    /*--------------------------------------------------------------------------
    * Create a message queue for Remote File Server. The client modules send
    * their requests to this message queue
    *------------------------------------------------------------------------*/
    MessageQ_Params_init(&msgQParams);
    hDccMsgQ = MessageQ_create(DCC_SERVER_MSGQ_NAME, &msgQParams);
    UTILS_assert(hDccMsgQ != NULL);

    while (ITT_ExitDone)
    {
        /*----------------------------------------------------------------------
         * Wait for a message on the server queue. All the client modules, no
         * matter which Slave processor they are running on, send a message
         * to the well known Remote File Server Message Queue
         *--------------------------------------------------------------------*/
        status =
            MessageQ_get(hDccMsgQ, (MessageQ_Msg *) & msg, MessageQ_FOREVER);
        if(Force_unblock == FALSE)
        UTILS_assert(status == MessageQ_S_SUCCESS);
        else{
        Memory_free(srHeap,buf,dccMemSize);
        }
        /*----------------------------------------------------------------------
         * Extract the Remote File command from the message.
         * Extract the identity of the client module.
         *--------------------------------------------------------------------*/
        if(msg != NULL){
        cmd = MessageQ_getMsgId(msg);
        sender = MessageQ_getReplyQueue(msg);
        }
        else
            break;
        /*----------------------------------------------------------------------
         * Despatch the command to the appropriate command handler. The command
         * handler also updates the message with the response.
         *--------------------------------------------------------------------*/
        Vps_rprintf("Command %d recived from HOST A8 \n", cmd);
        ittMsg = (ittServer_Msg *) msg;

        if (cmd == CMD_CKH_CONNECTION)                     // Request to
            // allocate memory
            // in Heap
        {
           /*--------------------------------------------------------------------------
            * Get the heap associated with the Shared Region. This heap should be used
            * for allocating memory blocks shared between Master and Slave
            *------------------------------------------------------------------------*/
            srHeap = SharedRegion_getHeap(ITT_SHAREDREGION_ID);
            UTILS_assert(srHeap != NULL);
            dccMemSize = ittMsg->arg1;
            buf = Memory_calloc(srHeap, ittMsg->arg1, 128, NULL);

            /*--------------------------------------------------------------------------
            * Get the Shared Region pointer corresponding to the 'buf' pointer. The
            * 'buf' is a local address accessible on Slave processor. The SharedRegion
            * module translates this local address into a special 'Shared Region'
            * address accessible across all processors.
            *------------------------------------------------------------------------*/
            srPtr = SharedRegion_getSRPtr(buf, ITT_SHAREDREGION_ID);

            ittMsg->arg1 = (Int32) (srPtr);
            ittMsg->response = 1;
            /*----------------------------------------------------------------------
            * Send the response back to the client module
            *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_SEND_DCC)                      // DCC data
        {
            if (buf == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                Iss_CaptDccPrm dccPrm;
                Int32 instId;
                Int32 status;
                Cache_inv(buf, dccMemSize, Cache_Type_ALL, NULL);
                Vps_rprintf("New DCC data of size <%d> \n", ittMsg->arg1);


                extern iss_drv_config_t *iss_drv_config;
                memcpy(dcc_buf, buf, ittMsg->arg1);
                new_dcc_data_available = 1;
                dcc_buf_size = ittMsg->arg1;
                ittMsg->response = 1;
                dccPrm.dcc_Default_Param = (UInt8 *)dcc_buf;
                dccPrm.dcc_init_done     = TRUE;
                dccPrm.dccSize           = ittMsg->arg1;

                for (instId = 0; instId < pObj->createArgs.numVipInst; instId++)
                {
                    pInst = &pObj->instObj[instId];
                    status = FVID2_control(pInst->cameraVipHandle,
                                           IOCTL_ISS_CAPT_SET_DCCPRM,
                                           &dccPrm, NULL);
                    UTILS_assert(status == FVID2_SOK);
                }

              	if(pObj->vnfLinkId != SYSTEM_LINK_ID_INVALID)
                  	System_linkControl(pObj->vnfLinkId,
                                 VNF_LINK_CMD_UPDATE_DCC,
                                 &dccPrm,
                                 sizeof(Iss_CaptDccPrm),TRUE);
              	if(pObj->glbceLinkId != SYSTEM_LINK_ID_INVALID)
                  	System_linkControl(pObj->glbceLinkId,
                                 GLBCE_LINK_CMD_UPDATE_DCC,
                                 &dccPrm,
                                 sizeof(Iss_CaptDccPrm),TRUE);
	          	if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  	System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_DCC,
								 &dccPrm,
								 sizeof(Iss_CaptDccPrm),TRUE);				
	          	if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  	System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_2A,
								 &ti2a_output_params,
								 sizeof(ti2a_output),TRUE);				
	          	if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  	System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_ISS_CONFIG,
								 iss_drv_config,
								 sizeof(iss_drv_config_t),TRUE);	
            }
           /*----------------------------------------------------------------------
            * Send the response back to the client module
            *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_MEM_READ_WORD)                 // Read Memory
        {
            if (ittMsg->arg1 == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                ittMsg->arg1 = *(int *) (ittMsg->arg1);
                ittMsg->response = 1;

            }

           /*----------------------------------------------------------------------
            * Send the response back to the client module
            *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        /* Get the raw data in the shared memory */
        else if (cmd == CMD_SAVE_RAW)
        {
            if (buf == NULL)
            {
                Vps_rprintf("Buffer is null can not trasmit the RAW data \n");
                ittMsg->response = 0;
            }
            else
            {

                int chunk = 0;

                char *raw_buf = (char *) isif_dump;
                CameraLink_Obj *pObj;
                CameraLink_VipInstParams *pInstPrm;
                CameraLink_OutParams *pOutPrm;
                pObj = &gCameraLink_obj;
                pInstPrm = &pObj->createArgs.vipInst[0];
                pOutPrm = &pInstPrm->outParams[0];

                int remaining_size;
                int capHorLineOffset,capVertLineOffset;

                int max_transfer_size = 1000 * 1024;

                capHorLineOffset  = pOutPrm->scOutWidth;
                capVertLineOffset = pOutPrm->scOutHeight;

                if(gVstabEnable == 1)
                {
                    capHorLineOffset  = (pOutPrm->scOutWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
                    capVertLineOffset = (pOutPrm->scOutHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
                }

                remaining_size = ((capHorLineOffset * capVertLineOffset * 4) / 2);

                Vps_rprintf("Requesing to dump the raw data \n");
                raw_data_ready = 0;
                request_raw_data = 1;
                if (pObj->createArgs.captureMode == CAMERA_LINK_CAPMODE_ISIF)
                {
                    Iss_IttParams ittParams;

                    /* wait till isr of isif dumps the raw data */
                    ittParams.ittCommand = cmd;
                    ittParams.isif_dump = isif_dump;
                    ittParams.Raw_Data_Ready = 0;
                    status =
                        FVID2_control(pInst->cameraVipHandle,
                                      IOCTL_ISS_ALG_ITT_CONTROL, &ittParams, NULL);
                    if (status != FVID2_SOK)
                    {
                        Vps_rprintf("AWB write to ISP failed!\n");
                        return ;
                    }
                    while (ittParams.Raw_Data_Ready == 0)
                    {
                        Task_sleep(100);
                    }
                }
                else
                {
                    while (raw_data_ready == 0)
                    {
                        Task_sleep(100);
                    }
                }
                Vps_rprintf(" raw data is ready to trasmit \n");

                Vps_rprintf("Trasmitting the raw data \n");
                // DEBUG
                do                                         // for (chunk = 0;
                                                           //
                    // chunk < 3;
                    // chunk++)
                {
                    int bytes_to_trasfer =
                        (remaining_size >
                         max_transfer_size) ? max_transfer_size :
                        remaining_size;
                    Vps_rprintf
                        ("Transmitting chunk %d bytes remaining = %d from raw buf %x\n",
                         chunk, remaining_size, raw_buf);
                    char *shared_buf = (char *) buf;

                    memcpy(shared_buf, raw_buf, bytes_to_trasfer);
                    raw_buf += bytes_to_trasfer;
                    {
                        int i;

                        for (i = 0; i < 10; i++)
                        {
                            Vps_rprintf("%x", ((int *) shared_buf)[i]);
                        }
                    }
                    /* for (i = 0; i < bytes_to_trasfer; i++) { *shared_buf++
                     * = raw_buf++; } */
                    ittMsg->arg1 = bytes_to_trasfer;
                    ittMsg->response = 1;
                    Vps_rprintf("Sending the reply message \n");
                     /*----------------------------------------------------------------------
                     * Send the response back to the client module
                         *--------------------------------------------------------------------*/
                    if(msg != NULL)
                    {
                        status = MessageQ_put(sender, (MessageQ_Msg) msg);
                        UTILS_assert(status == MessageQ_S_SUCCESS);
                    }
                    // Get
                    status =
                        MessageQ_get(hDccMsgQ, (MessageQ_Msg *) & msg,
                                     MessageQ_FOREVER);
                    Vps_rprintf("Got the acknowledgement  \n");
                    UTILS_assert(status == MessageQ_S_SUCCESS);
                    remaining_size -= bytes_to_trasfer;
                    chunk++;
                } while (remaining_size > 0);

                ittMsg->arg1 = -1;
                ittMsg->response = 1;
                if(msg != NULL)
                {
                    status = MessageQ_put(sender, (MessageQ_Msg) msg);
                    UTILS_assert(status == MessageQ_S_SUCCESS);
                }
            }
        }
        else if (cmd == CMD_SAVE_YUV)
        {
            if (buf == NULL)
            {
                Vps_rprintf("Buffer is null can not trasmit the YUV data \n");
                ittMsg->response = 0;
            }
            else
            {

                int chunk = 0;
                int capHorLineOffset,capVertLineOffset;
                CameraLink_Obj *pObj;
                CameraLink_VipInstParams *pInstPrm;
                CameraLink_OutParams *pOutPrm;
                pObj = &gCameraLink_obj;
                pInstPrm = &pObj->createArgs.vipInst[0];
                pOutPrm = &pInstPrm->outParams[0];

                capHorLineOffset  = pOutPrm->scOutWidth;
                capVertLineOffset = pOutPrm->scOutHeight;

                if(gVstabEnable == 1)
                {
                    capHorLineOffset  = (pOutPrm->scOutWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
                    capVertLineOffset = (pOutPrm->scOutHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
                }

                if (pOutPrm->dataFormat == FVID2_DF_YUV420SP_UV || pOutPrm->dataFormat == FVID2_DF_YUV420SP_VU){
                    YUVDumpSize = (((capHorLineOffset) * capVertLineOffset) * 3)/2;
                }
                else if (pOutPrm->dataFormat == FVID2_DF_YUV422I_UYVY ||
                         pOutPrm->dataFormat == FVID2_DF_YUV422I_YUYV ||
                         pOutPrm->dataFormat == FVID2_DF_YUV422I_YVYU ||
                         pOutPrm->dataFormat == FVID2_DF_YUV422I_VYUY ||
                         pOutPrm->dataFormat == FVID2_DF_BAYER_RAW){
                    YUVDumpSize = ((capHorLineOffset) * capVertLineOffset) * 2;
                }
                char *yuv_buf = (char *) captureBuff_Y[0];

                int remaining_size = (int) YUVDumpSize;

                int max_transfer_size = 1000 * 1024;

                Vps_rprintf("Requesing to dump the YUV data \n");
                yuv_data_ready = 0;
                request_yuv_data = 1;
                Vps_rprintf("Trasmitting the YUV data \n");
                if (pObj->createArgs.captureMode == CAMERA_LINK_CAPMODE_ISIF)
                {
                    // DEBUG
                    Iss_IttParams ittParams;

                    ittParams.Raw_Data_Ready = 0;
                    /* wait till isr of isif dumps the raw data */
                    ittParams.ittCommand = cmd;
                    ittParams.isif_dump = (UInt32 *)captureBuff_Y[0];
                    status =
                        FVID2_control(pInst->cameraVipHandle,
                                      IOCTL_ISS_ALG_ITT_CONTROL, &ittParams, NULL);
                    if (status != FVID2_SOK)
                    {
                        Vps_rprintf("YUV capture dump failed!\n");
                        return ;
                    }

                    while (ittParams.Raw_Data_Ready == 0)
                    {
                        Task_sleep(100);
                    }
                }
                else
                {
                    while (yuv_data_ready == 0)
                    {
                        Task_sleep(100);
                    }
                }
#if 1 //one frame YUV dump

		do					    					// for (chunk = 0;
													// chunk < 3;
													// chunk++)
		{
			int bytes_to_trasfer =
				(remaining_size >
				 max_transfer_size) ? max_transfer_size :
				remaining_size;
			Vps_rprintf("Transmitting chunk %d bytes remaining = %d \n",
						chunk, remaining_size);
			char *shared_buf = (char *) buf;

			memcpy(shared_buf, yuv_buf, bytes_to_trasfer);
			yuv_buf += bytes_to_trasfer;
			ittMsg->arg1 = bytes_to_trasfer;
			ittMsg->response = 1;
			Vps_rprintf("Sending the reply message \n");
			/*----------------------------------------------------------------------
			* Send the response back to the client module
			*--------------------------------------------------------------------*/
			if(msg != NULL)
			{
		 		status = MessageQ_put(sender, (MessageQ_Msg) msg);
				UTILS_assert(status == MessageQ_S_SUCCESS);
			}
            // Get
            status =
                MessageQ_get(hDccMsgQ, (MessageQ_Msg *) & msg,
                             MessageQ_FOREVER);
            Vps_rprintf("Got the acknowledgement  \n");
            UTILS_assert(status == MessageQ_S_SUCCESS);

            remaining_size -= bytes_to_trasfer;
            chunk++;
        } while (remaining_size > 0);				

#else //multi-frame dump, for NF tuning etc.,
                chunk= 0;

                for (frame_num = 0; frame_num < 20; frame_num ++)
                  {
                    char *yuv_buf = (char *) captureBuff_Y[frame_num];
                    remaining_size = (int) YUVDumpSize;
                    
                    do                                         // for (chunk = 0;
                                                               //
                        // chunk < 3;
                        // chunk++)
                    {
                        int bytes_to_trasfer =
                            (remaining_size >
                             max_transfer_size) ? max_transfer_size :
                            remaining_size;
                        Vps_rprintf("Transmitting chunk %d bytes remaining = %d \n",
                                    chunk, remaining_size);
                        char *shared_buf = (char *) buf;

                        memcpy(shared_buf, yuv_buf, bytes_to_trasfer);
                        yuv_buf += bytes_to_trasfer;
                        ittMsg->arg1 = bytes_to_trasfer;
                        ittMsg->response = 1;
                        Vps_rprintf("Sending the reply message \n");
            /*----------------------------------------------------------------------
            * Send the response back to the client module
            *--------------------------------------------------------------------*/
                        if(msg != NULL)
                        {
                            status = MessageQ_put(sender, (MessageQ_Msg) msg);
                            UTILS_assert(status == MessageQ_S_SUCCESS);
                        }
                        // Get
                        status =
                            MessageQ_get(hDccMsgQ, (MessageQ_Msg *) & msg,
                                         MessageQ_FOREVER);
                        Vps_rprintf("Got the acknowledgement  \n");
                        UTILS_assert(status == MessageQ_S_SUCCESS);
                        remaining_size -= bytes_to_trasfer;
                        chunk++;
                    } while (remaining_size > 0);
                  }
#endif
#if 0
                yuv_buf = (UInt8 *) captureBuff_C[0];
                remaining_size = (int) YUVDumpSize / 2;
                do
                {
                    int bytes_to_trasfer =
                        (remaining_size >
                         max_transfer_size) ? max_transfer_size :
                        remaining_size;
                    Vps_rprintf("Transmitting chunk %d bytes remaining = %d \n",
                                chunk, remaining_size);
                    char *shared_buf = (char *) buf;

                    memcpy(shared_buf, yuv_buf, bytes_to_trasfer);
                    yuv_buf += bytes_to_trasfer;
                    ittMsg->arg1 = bytes_to_trasfer;
                    ittMsg->response = 1;
                    Vps_rprintf("Sending the reply message \n");
        /*----------------------------------------------------------------------
        * Send the response back to the client module
        *--------------------------------------------------------------------*/
                    if(msg != NULL)
                    {
                        status = MessageQ_put(sender, (MessageQ_Msg) msg);
                        UTILS_assert(status == MessageQ_S_SUCCESS);
                    }
                    // Get
                    status =
                        MessageQ_get(hDccMsgQ, (MessageQ_Msg *) & msg,
                                     MessageQ_FOREVER);
                    Vps_rprintf("Got the acknowledgement  \n");
                    UTILS_assert(status == MessageQ_S_SUCCESS);
                    remaining_size -= bytes_to_trasfer;
                    chunk++;
                } while (remaining_size > 0);
#endif

                ittMsg->arg1 = -1;
                ittMsg->response = 1;
                if(msg != NULL)
                {
                    status = MessageQ_put(sender, (MessageQ_Msg) msg);
                    UTILS_assert(status == MessageQ_S_SUCCESS);
                }
            }
        }
        else if (cmd == CMD_REGS_READ)                     // ISP reg data
        {
            if (buf == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                if (ittMsg->arg1 == 1)
                {
                    ittMsg->arg1 = sizeof(CSL_IpipeRegs);
                    ittMsg->arg2 = (unsigned int) ipipe_reg;
                    memcpy(buf, (const void *)ipipe_reg, ittMsg->arg1);
                }
                else if ((ittMsg->arg1 >= 0x55050800) &&
                         ((ittMsg->arg1 + ittMsg->arg2) <= 0x55051480) &&
                         (ittMsg->arg2 > 0))
                {
                    int temp = ittMsg->arg2;

                    ittMsg->arg2 = ittMsg->arg1;
                    ittMsg->arg1 = temp;
                    Vps_rprintf
                        ("reading %d bytes from %x address and the IPIPE bases address is %x",
                         ittMsg->arg1, ittMsg->arg2, (unsigned int) ipipe_reg);
                    memcpy(buf, (const void *)ittMsg->arg2, ittMsg->arg1);
                }
                ittMsg->response = 1;
            }

        /*----------------------------------------------------------------------
        * Send the response back to the client module
        *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_MEM_READ_BUFF)                 // Memory buff
            // data
        {
            if (buf == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                if ((((ittMsg->arg1 >= 0x80000000) &&
                      ((ittMsg->arg1 + ittMsg->arg2) < 0xA0000000)) ||
                     ((ittMsg->arg1 >= 0xC0000000) &&
                      ((ittMsg->arg1 + ittMsg->arg2) < 0xD0000000)) ||
                     ((ittMsg->arg1 >= 0x55040000) &&
                      ((ittMsg->arg1 + ittMsg->arg2) < 0x55051480))) &&
                    (ittMsg->arg2 > 0))
                {
                    int temp = ittMsg->arg2;

                    ittMsg->arg2 = ittMsg->arg1;
                    ittMsg->arg1 = temp;
                    Vps_rprintf("reading %d bytes from %x address",
                                ittMsg->arg1, ittMsg->arg2);
                    memcpy(buf, (const void *)ittMsg->arg2, ittMsg->arg1);
                }
                else
                {
                    ittMsg->arg2 = ittMsg->arg1;
                    ittMsg->arg1 = 20;
                }
                ittMsg->response = 1;
            }

        /*----------------------------------------------------------------------
        * Send the response back to the client module
        *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_SENSOR_REGS_READ)              // Sensor regiter
            // read
        {
            ittMsg->response = 0;
            if (buf != NULL)
            {
                if (((ittMsg->arg1 >= 0x0000) &&
                     ((ittMsg->arg1 + ittMsg->arg2) < 0x4000)) &&
                    (ittMsg->arg2 > 0))
                {
                    int idx;

                    int temp = ittMsg->arg2;

                    ittMsg->arg2 = ittMsg->arg1;
                    temp &= (~1);
                    // temp = 2;
                    ittMsg->arg1 = temp;
                    Vps_rprintf
                        ("reading %d bytes from %x offset of sensor regiters",
                         ittMsg->arg1, ittMsg->arg2);
                    for (idx = 0; idx < temp; idx += 2)
                    {
                        // Vps_rprintf("reading register num : %d \n", idx >>
                        //
                        // 1);
                        gItt_RegisterParams.Control = 0;
                        gItt_RegisterParams.regAddr = (ittMsg->arg2 + idx);
                        gItt_RegisterParams.regValue = ((UInt32)buf + idx);
                        status = FVID2_control(pInst->SensorHandle,
                                          IOCTL_ISS_SENSOR_UPDATE_ITT,
                                          &gItt_RegisterParams, NULL);
                        if (status != FVID2_SOK)
                        {
                            ittMsg->response = 0;
                            Vps_rprintf("\nAE write to sensor failed!\n");
                            return ;
                        } else
                        {
                            ittMsg->response = 1;
                        }

                        *((UInt16*)((UInt32)buf+idx)) = gItt_RegisterParams.regValue;
                    }
                }
                else
                {
                    ittMsg->arg2 = ittMsg->arg1;
                    ittMsg->arg1 = 20;
                }

            }

        /*----------------------------------------------------------------------
        * Send the response back to the client module
        *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_SENSOR_REG_WRITE)              // Write Sensor
            // Reg
        {
            if (ittMsg->arg1 == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                gItt_RegisterParams.Control = 1;
                gItt_RegisterParams.regAddr = ittMsg->arg1;
                gItt_RegisterParams.regValue = ittMsg->arg2;
                status =
                    FVID2_control(pInst->SensorHandle,
                                  IOCTL_ISS_SENSOR_UPDATE_ITT,
                                  &gItt_RegisterParams, NULL);
                if (status != FVID2_SOK)
                {
                    Vps_rprintf("\nAE write to sensor failed!\n");
                    ittMsg->response = 0;
                    return ;
                } else
                {
                    ittMsg->response = 1;
                }
            }

      /*----------------------------------------------------------------------
      * Send the response back to the client module
      *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_MEM_WRITE_WORD)                // Write Memory
        {
            if (ittMsg->arg1 == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                *(int *) (ittMsg->arg1) = ittMsg->arg2;
                ittMsg->response = 1;
            }

      /*----------------------------------------------------------------------
      * Send the response back to the client module
      *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_RECV_DCC)                      // Trasfer DCC
            // from Target to
            // PC tuning tool
        {
            if (buf == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                ittMsg->arg1 = dcc_buf_size;
                memcpy(buf, dcc_buf, ittMsg->arg1);
                Vps_rprintf("Read DCC data of size <%d> \n", ittMsg->arg1);
                ittMsg->response = 1;
            }

    /*----------------------------------------------------------------------
    * Send the response back to the client module
    *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }

        /* Camera Control */
        else if (cmd == CMD_CAMERA_CNTL)
        {
            if (buf == NULL)
            {
                ittMsg->response = 0;
            }
            else
            {
                Cache_inv(buf, dccMemSize, Cache_Type_ALL, NULL);
                Vps_rprintf("Camera Control Params of size <%d> \n",
                            ittMsg->arg1);
                memcpy((void *) &gSensorControlParams, buf, ittMsg->arg1);
                if (gSensorControlParams.update == AEWB_ENABLE)
                {
                    ti_2a_run = 1;
                    gTi2aControlParams.aewbType = 3;
                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == AEWB_DISABLE)
                {
                    ti_2a_run = 0;
                    ti2a_output_params.mask = 0;
                    gTi2aControlParams.aewbType = 0;
                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == AWB_TOGGLE)
                {
                    // extern int awb_alg;
                    // if(awb_alg == 1)
                    // {
                    // awb_alg = 0;
                    // Vps_rprintf("TI DSP RND AWB algo is selected \n");
                    // }
                    // else
                    // {
                    // awb_alg = 1;
                    // Vps_rprintf("TI VSP Team AWB algo is selected \n");
                    // } Needed to modify

                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == INCREASE_EXPOSURE)
                {
                    if (ti_2a_run == 0)
                    {
                        TI_Increase_Exposure();
                    }
                    else
                    {
                        Vps_rprintf("sensor minExposure Time    :  < %5d > \n", gTi2aControlParams.minExposure);
                        Vps_rprintf("sensor stepSize            :  < %5d > \n", gTi2aControlParams.stepSize);
                        Vps_rprintf("sensor aGainMin            :  < %5d > \n", gTi2aControlParams.aGainMin);
                        Vps_rprintf
                            ("AEWB is currently running and the current status is\n");
                        Vps_rprintf("sensor Exposure Time   :  < %5d > \n",
                                    ti2a_output_params.sensorExposure);
                        Vps_rprintf("Sensor Analog Gain     :  < %5d > \n",
                                    ti2a_output_params.sensorGain);
                        Vps_rprintf("IPIPE Digital Gain     :  < %5d > \n",
                                    (ti2a_output_params.ipipe_awb_gain.dGain *
                                     2));
                        Vps_rprintf("Color Temparature      :  < %5d > \n",
                                    (ti2a_output_params.colorTemparaure));
                        Vps_rprintf
                            ("WB gains are           :  < %5d > ,< %5d > ,< %5d > \n",
                             ti2a_output_params.ipipe_awb_gain.rGain,
                             ti2a_output_params.ipipe_awb_gain.grGain,
                             ti2a_output_params.ipipe_awb_gain.bGain);
                    }
                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == DECREASE_EXPOSURE)
                {
                    if (ti_2a_run == 0)
                    {
                        TI_Decrease_Exposure();
                    }
                    else
                    {
                        Vps_rprintf("\nAEWB is currently running and the current status is\n");
                        Vps_rprintf("sensor minExposure Time    :  < %5d > \n", gTi2aControlParams.minExposure);
                        Vps_rprintf("sensor stepSize            :  < %5d > \n", gTi2aControlParams.stepSize);
                        Vps_rprintf("sensor aGainMin            :  < %5d > \n", gTi2aControlParams.aGainMin);
                        Vps_rprintf("sensor Exposure Time       :  < %5d > \n", ti2a_output_params.sensorExposure);
                        Vps_rprintf("Sensor Analog Gain         :  < %5d > \n", ti2a_output_params.sensorGain);
                        Vps_rprintf("IPIPE Digital Gain         :  < %5d > \n",(ti2a_output_params.ipipe_awb_gain.dGain * 2));
                        Vps_rprintf("Color Temparature          :  < %5d > \n",(ti2a_output_params.colorTemparaure));
                        Vps_rprintf
                            ("WB gains are           :  < %5d > ,< %5d > ,< %5d > \n",
                             ti2a_output_params.ipipe_awb_gain.rGain,
                             ti2a_output_params.ipipe_awb_gain.grGain,
                             ti2a_output_params.ipipe_awb_gain.bGain);
                    }

                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == DCC_ENABLE)
                {
                    if (dcc_parser_run == 1)
                    {
                        Vps_rprintf("dcc parsing is already running \n");
                    }
                    else
                    {
                        Iss_CaptDccPrm dccPrm;
                        Int32 instId;
                        Int32 status;
                        dcc_parser_run = 1;
                        dccPrm.dcc_init_done     = TRUE;
                        dccPrm.dcc_Default_Param = (UInt8 *)dcc_buf;
                        dccPrm.dccSize           = dcc_buf_size;

                        for (instId = 0; instId < pObj->createArgs.numVipInst; instId++)
                        {
                            pInst = &pObj->instObj[instId];
                            status = FVID2_control(pInst->cameraVipHandle,
                                                   IOCTL_ISS_CAPT_SET_DCCPRM,
                                                   &dccPrm, NULL);
                            UTILS_assert(status == FVID2_SOK);
                        }
                        Vps_rprintf("dcc parser is enabled sucessfully \n");

                        if(pObj->vnfLinkId != SYSTEM_LINK_ID_INVALID)
                        System_linkControl(pObj->vnfLinkId,
                             VNF_LINK_CMD_UPDATE_DCC,
                             &dccPrm,
                             sizeof(Iss_CaptDccPrm),FALSE);

                        if(pObj->glbceLinkId != SYSTEM_LINK_ID_INVALID)
                        System_linkControl(pObj->glbceLinkId,
                             GLBCE_LINK_CMD_UPDATE_DCC,
                             &dccPrm,
                             sizeof(Iss_CaptDccPrm),FALSE);
						  if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
							  System_linkControl(pObj->mctnfLinkId,
											 MCTNF_LINK_CMD_UPDATE_DCC,
											 &dccPrm,
											 sizeof(Iss_CaptDccPrm),FALSE);				
	          if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_2A,
								 &ti2a_output_params,
								 sizeof(ti2a_output),FALSE);				
	          if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_ISS_CONFIG,
								 iss_drv_config,
								 sizeof(iss_drv_config_t),FALSE);	

                    }
                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == DCC_DISABLE)
                {
                    if (dcc_parser_run == 0)
                    {
                        Vps_rprintf
                            ("dcc parsing is already in disabled state \n");
                    }
                    else
                    {

                        Iss_CaptDccPrm dccPrm;
                        Int32 instId;
                        Int32 status;
                        dccPrm.dcc_init_done     = FALSE;
                        dcc_parser_run = 0;
                        dccPrm.dcc_Default_Param = (UInt8 *)dcc_buf;
                        dccPrm.dccSize           = dcc_buf_size;

                        for (instId = 0; instId < pObj->createArgs.numVipInst; instId++)
                        {
                            pInst = &pObj->instObj[instId];
                            status = FVID2_control(pInst->cameraVipHandle,
                                                   IOCTL_ISS_CAPT_SET_DCCPRM,
                                                   &dccPrm, NULL);
                            UTILS_assert(status == FVID2_SOK);
                        }
                        if(pObj->vnfLinkId != SYSTEM_LINK_ID_INVALID)
                        System_linkControl(pObj->vnfLinkId,
                             VNF_LINK_CMD_UPDATE_DCC,
                             &dccPrm,
                             sizeof(Iss_CaptDccPrm),FALSE);

                        if(pObj->glbceLinkId != SYSTEM_LINK_ID_INVALID)
                        System_linkControl(pObj->glbceLinkId,
                             GLBCE_LINK_CMD_UPDATE_DCC,
                             &dccPrm,
                             sizeof(Iss_CaptDccPrm),FALSE);
	          if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_DCC,
								 &dccPrm,
								 sizeof(Iss_CaptDccPrm),FALSE);				
	          if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_2A,
								 &ti2a_output_params,
								 sizeof(ti2a_output),FALSE);	
								 
	          if(pObj->mctnfLinkId != SYSTEM_LINK_ID_INVALID)
				  System_linkControl(pObj->mctnfLinkId,
								 MCTNF_LINK_CMD_UPDATE_ISS_CONFIG,
								 iss_drv_config,
								 sizeof(iss_drv_config_t),FALSE);	

                             Vps_rprintf("dcc parser is disabled sucessfully \n");
                    }
                    gSensorControlParams.update = CLEAR_COMMAND;
                }
                else if (gSensorControlParams.update == CONTROL_PARAMS_2A)
                {

                    Vps_rprintf(" gSensorControlParams.flickerFreq %d \n",
                                gSensorControlParams.flickerFreq);
                    Vps_rprintf("  gSensorControlParams.minExposure %d \n",
                                gSensorControlParams.minExposure);
                    Vps_rprintf("  gSensorControlParams.maxExposure%d \n",
                                gSensorControlParams.maxExposure);
                    Vps_rprintf("  gSensorControlParams.stepSize %d \n",
                                gSensorControlParams.stepSize);
                    Vps_rprintf(" gSensorControlParams.aGainMin%d \n",
                                gSensorControlParams.aGainMin);
                    Vps_rprintf(" gSensorControlParams.aGainMax%d \n",
                                gSensorControlParams.aGainMax);
                    Vps_rprintf(" gSensorControlParams.dGainMin%d \n",
                                gSensorControlParams.dGainMin);
                    Vps_rprintf(" gSensorControlParams.dGainMax%d \n",
                                gSensorControlParams.dGainMax);
                    Vps_rprintf
                        (" gSensorControlParams.targetBrightnessMin %d \n",
                         gSensorControlParams.targetBrightnessMin);
                    Vps_rprintf
                        (" gSensorControlParams.targetBrightnessMax%d \n",
                         gSensorControlParams.targetBrightnessMax);
                    Vps_rprintf(" gSensorControlParams.targetBrightness%d \n",
                                gSensorControlParams.targetBrightness);

                    gTi2aControlParams.flickerFreq =  gSensorControlParams.flickerFreq;
                    gTi2aControlParams.minExposure =  gSensorControlParams.minExposure;
                    gTi2aControlParams.maxExposure =  gSensorControlParams.maxExposure;
                    gTi2aControlParams.stepSize =  gSensorControlParams.stepSize;
                    gTi2aControlParams.aGainMin =  gSensorControlParams.aGainMin;
                    gTi2aControlParams.aGainMax =  gSensorControlParams.aGainMax;
                    gTi2aControlParams.dGainMin =  gSensorControlParams.dGainMin;
                    gTi2aControlParams.dGainMax =  gSensorControlParams.dGainMax;
                    gTi2aControlParams.targetBrightnessMin =  gSensorControlParams.targetBrightnessMin;
                    gTi2aControlParams.targetBrightnessMax =  gSensorControlParams.targetBrightnessMax;
                    gTi2aControlParams.targetBrightness =  gSensorControlParams.targetBrightness;
                    gTi2aControlParams.update |= TI2A_UPDATE_CONTROL_PARAMS_2A;

                }
                ittMsg->response = 1;
            }
           /*----------------------------------------------------------------------
            * Send the response back to the client module
            *--------------------------------------------------------------------*/
            if(msg != NULL)
            {
                status = MessageQ_put(sender, (MessageQ_Msg) msg);
                UTILS_assert(status == MessageQ_S_SUCCESS);
            }
        }
        else if (cmd == CMD_ITT_STOP)
        {
            ITT_ExitDone = TRUE;
            break;
        }
        else
        {
            ittMsg->response = 0;
            Vps_rprintf("Invalid command \n");
        }

        /*----------------------------------------------------------------------
         *  Quit if the command is for shutdown
         *--------------------------------------------------------------------*/
        if (cmd == 0)
        {
            break;
        }

    }
}

/* ===================================================================
 *  @func     Alg_ITK_Link_init
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
Int32 Alg_ITK_Link_init()
{
    Int32 status;
    UInt32 capHorLineOffset,capVertLineOffset;

    CameraLink_Obj *pObj;
    CameraLink_VipInstParams *pInstPrm;
    CameraLink_OutParams *pOutPrm;

    Int32 idx;
    Int32 CaptureFrame_Size = 0;

    /* register link with system API */
    pObj = &gCameraLink_obj;

    /* Setting Default values */
    ITT_ExitDone = TRUE;
    Force_unblock = FALSE;
    /** Create link task, task remains in IDLE state
        CameraLink_tskMain is called when a message command is received
    */
    pInstPrm = &pObj->createArgs.vipInst[0];
    pOutPrm = &pInstPrm->outParams[0];

    capHorLineOffset  = pOutPrm->scOutWidth;
    capVertLineOffset = pOutPrm->scOutHeight;

    if(gVstabEnable == 1)
    {
        capHorLineOffset  = (pOutPrm->scOutWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        capVertLineOffset = (pOutPrm->scOutHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
    }

    if (pOutPrm->dataFormat == FVID2_DF_YUV420SP_UV || pOutPrm->dataFormat == FVID2_DF_YUV420SP_VU){
        CaptureFrame_Size = (((capHorLineOffset) * capVertLineOffset) * 3)/2;
    }
    else if (pOutPrm->dataFormat == FVID2_DF_YUV422I_UYVY ||
             pOutPrm->dataFormat == FVID2_DF_YUV422I_YUYV ||
             pOutPrm->dataFormat == FVID2_DF_YUV422I_YVYU ||
             pOutPrm->dataFormat == FVID2_DF_YUV422I_VYUY ||
             pOutPrm->dataFormat == FVID2_DF_BAYER_RAW){
        CaptureFrame_Size = ((capHorLineOffset) * capVertLineOffset) * 2;
    }
    for (idx = 0; idx < NUM_CAPTURE_BUFS; idx++)
    {
        captureBuff_Y[idx] = Utils_memAlloc(CaptureFrame_Size, 32);
    }
    isif_dump = Utils_memAlloc((capHorLineOffset * capVertLineOffset) * 2, 32);

    status = Utils_tskCreate(&pObj->tsk_ITK,
                             Alg_ITK_Link_tskMain,
                             ALG_ITK_LINK_TSK_PRI,
                             gAlg_ITK_Link_tskStack,
                             ALG_ITK_LINK_TSK_STACK_SIZE, pObj, "ALG_ITK ");

    UTILS_assert(status == FVID2_SOK);

    return status;
}

/* ===================================================================
 *  @func     Alg_ITK_Link_delete
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
 Int32 Alg_ITK_Link_delete()
 {
    Int32 idx;
    UInt32 capHorLineOffset,capVertLineOffset;
    CameraLink_Obj *pObj;
    CameraLink_VipInstParams *pInstPrm;
    CameraLink_OutParams *pOutPrm;
    Int32 CaptureFrame_Size = 0;

    pObj = &gCameraLink_obj;

    pInstPrm = &pObj->createArgs.vipInst[0];
    pOutPrm = &pInstPrm->outParams[0];

    capHorLineOffset  = pOutPrm->scOutWidth;
    capVertLineOffset = pOutPrm->scOutHeight;

    if(gVstabEnable == 1)
    {
        capHorLineOffset  = (pOutPrm->scOutWidth * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
        capVertLineOffset = (pOutPrm->scOutHeight * VSTAB_SCALE_NUMERATOR)/VSTAB_SCALE_DENOMINATOR;
    }

    if (pOutPrm->dataFormat == FVID2_DF_YUV420SP_UV || pOutPrm->dataFormat == FVID2_DF_YUV420SP_VU){
        CaptureFrame_Size = (((capHorLineOffset) * capVertLineOffset) * 3)/2;
    }
    else if (pOutPrm->dataFormat == FVID2_DF_YUV422I_UYVY ||
             pOutPrm->dataFormat == FVID2_DF_YUV422I_YUYV ||
             pOutPrm->dataFormat == FVID2_DF_YUV422I_YVYU ||
             pOutPrm->dataFormat == FVID2_DF_YUV422I_VYUY ||
             pOutPrm->dataFormat == FVID2_DF_BAYER_RAW){
        CaptureFrame_Size = ((capHorLineOffset) * capVertLineOffset) * 2;
    }
    for (idx = 0; idx < NUM_CAPTURE_BUFS; idx++)
    {
         Utils_memFree(captureBuff_Y[idx], CaptureFrame_Size);
    }

    Utils_memFree(isif_dump, (capHorLineOffset * capVertLineOffset) * 2);

    return FVID2_SOK;
 }


/* ===================================================================
 *  @func     Alg_ITK_Link_deInit
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
Int32 Alg_ITK_Link_deInit()
{
    CameraLink_Obj *pObj;

    pObj = &gCameraLink_obj;
    Force_unblock = TRUE;
    ITT_ExitDone = FALSE;

    MessageQ_unblock(hDccMsgQ);
    MessageQ_delete(&hDccMsgQ);

    /* Delete link task */
    Utils_tskDelete(&pObj->tsk_ITK);

    return FVID2_SOK;
}
