/** ===========================================================================
* @file gop_lock.c
*
* @path $(IPNCPATH)\multimedia\encode_stream\stream
*
* @desc
* .
* Copyright (c) Appro Photoelectron Inc.  2008
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* =========================================================================== */

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <gop_lock.h>
#include <cache_mng.h>
#include <stream.h>
#include <semaphore_util.h>
#include <osa.h>

// #define __GOP_DEBUG

#ifdef __GOP_DEBUG
#define DBG(fmt, args...) fprintf(stderr, "GOP Debug: " fmt, ## args)
#else
#define DBG(fmt, args...)
#endif

#define ERR(fmt, args...) fprintf(stderr, "GOP Error: " fmt, ## args)

typedef struct GopInfo {
    int start_serial;
    int end_serial;
    int lock;
    struct GopInfo *next;
} GopInfo_t;

static GopInfo_t *gGopList[GOP_INDEX_NUM];

/* ===================================================================
 *  @func     GetGopInfoBySerial
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
GopInfo_t *GetGopInfoBySerial(int serial, int index);

/* ===================================================================
 *  @func     AddNewGop
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
GopRet_t AddNewGop(int start_serial, int end_serial, int index);

/* ===================================================================
 *  @func     DeleteGop
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
GopRet_t DeleteGop(GopInfo_t * pGop, int index);

/* ===================================================================
 *  @func     GetGopLastPwithStartSerial
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
int GetGopLastPwithStartSerial(int start_serial, VIDEO_BLK_INFO * pVidInfo);

/* ===================================================================
 *  @func     LockGop
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
GopRet_t LockGop(int start_serial, int end_serial, VIDEO_BLK_INFO * pVidInfo);

/* ===================================================================
 *  @func     AddAndLockGop
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
GopRet_t AddAndLockGop(int start_serial, int end_serial,
                       VIDEO_BLK_INFO * pVidInfo, int index);
/* ===================================================================
 *  @func     UnlockGop
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
GopRet_t UnlockGop(int start_serial, int end_serial, VIDEO_BLK_INFO * pVidInfo);

/* ===================================================================
 *  @func     ShowAllGop
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
void ShowAllGop(int index);

/**
 * @brief	Initialize GOP
 * @param	none
 * @return	none
 */
/* ===================================================================
 *  @func     GopInit
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
void GopInit(void)
{
    memset(gGopList, 0, sizeof(gGopList));
}

/**
 * @brief	GopInfo mem allocate
 * @param	none
 * @return	pointer to GopInfo_t
 */
/* ===================================================================
 *  @func     GopInfoMalloc
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
GopInfo_t *GopInfoMalloc(void)
{

    return malloc(sizeof(GopInfo_t));

}

/**
 * @brief	free GopInfo
 * @param	"GopInfo_t *pGopinfo" : GopInfo address
 * @return	0
 */
/* ===================================================================
 *  @func     GopInfoFree
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
int GopInfoFree(GopInfo_t * pGopinfo)
{

    free(pGopinfo);
    return 0;

}

/**
 * @brief	Lock GOP by serial
 * @param	"int serial" : serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int index" : index
 * @return	0 : success ; others : fail
 */
/* ===================================================================
 *  @func     LockGopBySerial
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
GopRet_t LockGopBySerial(int serial, VIDEO_BLK_INFO * pVidInfo, int index)
{
    VIDEO_FRAME *pFrame;

    GopInfo_t *pGopInfo;

    GopRet_t ret = GOP_COMPLETE;

    int end_serial, cur_serial;

    if (pVidInfo == NULL || serial < 0)
    {
        return GOP_INVALID_PRM;
    }

    Sem_lock(STREAM_SEM_GOP);
    if ((pGopInfo = GetGopInfoBySerial(serial, index)) != NULL)
    {
        pGopInfo->lock++;
    }
    else
    {
        cur_serial = GetCurrentSerialNo(pVidInfo);

        if ((pFrame = MemMng_GetFrameBySerial(serial, pVidInfo)) == NULL)
        {
            Sem_unlock(STREAM_SEM_GOP);
            if (cur_serial == serial)
                return GOP_FAIL;
            else if (cur_serial < serial)
                return GOP_NOT_READY;
            else
                return GOP_OVERWRITTEN;
        }
        if (pFrame->fram_type == I_FRAME)
        {
            end_serial = GetGopLastPwithStartSerial(serial, pVidInfo);
            if (0 > end_serial)
            {
                end_serial = cur_serial;
                ret = AddAndLockGop(serial, end_serial, pVidInfo, index);
                if (ret == GOP_COMPLETE)
                    ret = GOP_INCOMPLETE;
            }
            else
            {
                ret = AddAndLockGop(serial, end_serial, pVidInfo, index);
            }

        }
        else if (pFrame->fram_type == P_FRAME)
        {

            if ((pGopInfo = GetGopInfoBySerial(serial - 1, index)) != NULL)
            {
                // CheckReCache( serial, pVidInfo, __LINE__ ); -- DEBUG ONLY
                if ((ret = CacheMng_Video_CacheLock(serial, pVidInfo)) == 0)
                {
                    pGopInfo->lock++;
                    pGopInfo->end_serial = serial;
                }
                else
                {
                    if (ret == (GopRet_t) CACHE_NOMEM)
                        ret = GOP_NOMEM;
                    else
                        ret = GOP_FAIL;
                }

            }
            else
            {
                ret = GOP_ERROR_OP;
            }
        }
        else
            ERR("Unkown fram type in %s\n", __func__);
    }
    Sem_unlock(STREAM_SEM_GOP);

    return ret;
}

/**
 * @brief	Unlock GOP by serial
 * @param	"int serial" : serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int index" : index
 * @return	GOP_COMPLETE (0) : success ; others : fail
 */
/* ===================================================================
 *  @func     UnlockGopBySerial
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
GopRet_t UnlockGopBySerial(int serial, VIDEO_BLK_INFO * pVidInfo, int index)
{
    GopInfo_t *pGopInfo;

    GopRet_t ret = GOP_COMPLETE;

    int cur_serial;

    if (pVidInfo == NULL || serial < 0)
        return GOP_INVALID_PRM;
    cur_serial = GetCurrentSerialNo(pVidInfo);
    Sem_lock(STREAM_SEM_GOP);
    if ((pGopInfo = GetGopInfoBySerial(serial, index)) != NULL)
    {
        pGopInfo->lock--;
    }
    else
    {
        Sem_unlock(STREAM_SEM_GOP);
        return GOP_INVALID_PRM;
    }
    if (pGopInfo->lock == 0)
    {
        if (pGopInfo->start_serial <= cur_serial &&
            pGopInfo->end_serial >= cur_serial)
            ret = GOP_INCOMPLETE;
        if (UnlockGop(pGopInfo->start_serial, pGopInfo->end_serial, pVidInfo) !=
            GOP_COMPLETE)
            ERR("Unlock fail, Memory leak will occure\n");
        if (DeleteGop(pGopInfo, index) != GOP_COMPLETE)
            ERR("Can not found GOP at address 0x%x\n", (unsigned int) pGopInfo);
    }
    Sem_unlock(STREAM_SEM_GOP);

    return ret;
}

/**
 * @brief	Get GOP info by serial
 * @param	"int serial" : serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int index" : index
 * @return	GOP_COMPLETE (0) : success ; others : fail
 */
/* ===================================================================
 *  @func     GetGopInfoBySerial
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
GopInfo_t *GetGopInfoBySerial(int serial, int index)
{
    GopInfo_t *pGopInfo;

    if (gGopList[index] == NULL)
        return NULL;
    for (pGopInfo = gGopList[index]; pGopInfo != NULL;
         pGopInfo = pGopInfo->next)
        if (pGopInfo->start_serial <= serial && pGopInfo->end_serial >= serial)
            return pGopInfo;
    return NULL;
}

/**
 * @brief	Add new GOP
 * @param	"int start_serial" : start serial no.
 * @param	"int end_serial" : end serial no.
 * @param	"int index" : index
 * @return	GOP_COMPLETE (0) : success ; GOP_NOMEM : fail
 */
/* ===================================================================
 *  @func     AddNewGop
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
GopRet_t AddNewGop(int start_serial, int end_serial, int index)
{
    GopInfo_t *NewGop;

    NewGop = GopInfoMalloc();
    if (NewGop == NULL)
    {
        return GOP_NOMEM;
    }
    memset(NewGop, 0, sizeof(GopInfo_t));
    NewGop->start_serial = start_serial;
    NewGop->end_serial = end_serial;
    NewGop->lock = 1;
    NewGop->next = gGopList[index];
    gGopList[index] = NewGop;
    return GOP_COMPLETE;
}

/**
 * @brief	Delete GOP
 * @param	"GopInfo_t* pGop" : GOP pointer
 * @param	"int index" : index
 * @return	GOP_COMPLETE (0) : success ; others : fail
 */
/* ===================================================================
 *  @func     DeleteGop
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
GopRet_t DeleteGop(GopInfo_t * pGop, int index)
{
    GopInfo_t *pGopInfo;

    if (pGop == NULL)
        return GOP_INVALID_PRM;
    if (gGopList[index] == pGop)
    {
        gGopList[index] = gGopList[index]->next;
        GopInfoFree(pGop);
        return GOP_COMPLETE;
    }
    for (pGopInfo = gGopList[index]; pGopInfo != NULL;
         pGopInfo = pGopInfo->next)
        if (pGopInfo->next == pGop)
        {
            pGopInfo->next = pGop->next;
            GopInfoFree(pGop);
            return GOP_COMPLETE;
        }
    return GOP_FAIL;
}

/**
 * @brief	Get GOP last Pframe serial
 * @param	"int start_serial" : start serial
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	LastP serial ; -1 : fail
 */
/* ===================================================================
 *  @func     GetGopLastPwithStartSerial
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
int GetGopLastPwithStartSerial(int start_serial, VIDEO_BLK_INFO * pVidInfo)
{
    int LastP;

    VIDEO_FRAME *pFrame;

    for (LastP = start_serial + 1; (pFrame =
                                    MemMng_GetFrameBySerial(LastP,
                                                            pVidInfo)) != NULL;
         LastP++)
        if (pFrame->fram_type == I_FRAME)
            return LastP - 1;
    return -1;
}

/**
 * @brief	Lock GOP
 * @param	"int start_serial" : start serial no.
 * @param	"int end_serial" : end serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	GOP_COMPLETE(0) : success ; others : fail
 */
/* ===================================================================
 *  @func     LockGop
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
GopRet_t LockGop(int start_serial, int end_serial, VIDEO_BLK_INFO * pVidInfo)
{
    int serial;

    int ret;

    for (serial = start_serial; serial <= end_serial; serial++)
    {
        // CheckReCache( serial, pVidInfo, __LINE__ ); -- DEBUG ONLY
        if ((ret = CacheMng_Video_CacheLock(serial, pVidInfo)) != 0)
        {
            while (--serial >= start_serial)
            {
                CacheMng_Video_CacheUnlock(serial, pVidInfo);
            }
            OSA_printf("Lock Gop fail from %d to %d\n", start_serial, end_serial);
            if (ret == CACHE_NOMEM)
                return GOP_NOMEM;
            else
                return GOP_FAIL;
        }
    }
    return GOP_COMPLETE;
}

/**
 * @brief	Lock GOP
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int index" : index
 * @return	none
 */
/* ===================================================================
 *  @func     GopCleanup
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
void GopCleanup(VIDEO_BLK_INFO * pVidInfo, int index)
{
    GopInfo_t *pGopInfo;

    Sem_lock(STREAM_SEM_GOP);
    while (gGopList[index] != NULL)
    {
        pGopInfo = gGopList[index];
        UnlockGop(pGopInfo->start_serial, pGopInfo->end_serial, pVidInfo);
        gGopList[index] = gGopList[index]->next;
        GopInfoFree(pGopInfo);
    }
    Sem_unlock(STREAM_SEM_GOP);
}

/**
 * @brief	Add and lock GOP
 * @param	"int start_serial" : start serial no.
 * @param	"int end_serial" : end serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int index" : index
 * @return	0 : success ; others : fail
 */
/* ===================================================================
 *  @func     AddAndLockGop
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
GopRet_t AddAndLockGop(int start_serial, int end_serial,
                       VIDEO_BLK_INFO * pVidInfo, int index)
{
    GopRet_t ret;

    GopInfo_t *pGopInfo;

    ret = AddNewGop(start_serial, end_serial, index);
    if (ret)
        return ret;
    ret = LockGop(start_serial, end_serial, pVidInfo);
    if (ret)
    {
        pGopInfo = GetGopInfoBySerial(start_serial, index);
        if (pGopInfo != NULL)
            DeleteGop(pGopInfo, index);
        else
            OSA_printf("Incompelete GOP (%d ~ %d) was not deleted by %s\n",
                start_serial, end_serial, __func__);
    }
    return ret;
}

/**
 * @brief	Unlock GOP
 * @param	"int start_serial" : start serial no.
 * @param	"int end_serial" : end serial no.
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @return	GOP_COMPLETE(0) : success ; others : fail
 */
/* ===================================================================
 *  @func     UnlockGop
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
GopRet_t UnlockGop(int start_serial, int end_serial, VIDEO_BLK_INFO * pVidInfo)
{
    int serial;

    GopRet_t ret = GOP_COMPLETE;

    for (serial = start_serial; serial <= end_serial; serial++)
        if (CacheMng_Video_CacheUnlock(serial, pVidInfo) != 0)
        {
            OSA_printf("Fail at unlock frame %d\n", serial);
            ret = GOP_FAIL;
        }
    return ret;
}

/**
 * @brief	Lock current GOP
 * @param	"VIDEO_BLK_INFO *pVidInfo"
 * @param	"int index" : index
 * @return	GOP_COMPLETE(0) : success ; others : fail
 */
/* ===================================================================
 *  @func     LockCurrentGopP
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
GopRet_t LockCurrentGopP(VIDEO_BLK_INFO * pVidInfo, int index)
{
    int ret, cur_serial = GetCurrentSerialNo(pVidInfo);

    GopInfo_t *pGopInfo;

    Sem_lock(STREAM_SEM_GOP);
    pGopInfo = GetGopInfoBySerial(cur_serial - 1, index);
    if (pGopInfo == NULL)
    {
        ERR("cur_serial = %d\n", cur_serial);
        ShowAllGop(index);
        Sem_unlock(STREAM_SEM_GOP);
        return GOP_ERROR_OP;
    }
    // if(GetGopInfoBySerial(cur_serial,index)!= NULL)
    // return GOP_COMPLETE;
    // CheckReCache( cur_serial, pVidInfo, __LINE__ ); -- DEBUG ONLY
    if ((ret = CacheMng_Video_CacheLock(cur_serial, pVidInfo)) != 0)
    {
        DBG("LockCurrentGopP: %d\n", ret);
        UnlockGop(pGopInfo->start_serial, pGopInfo->end_serial, pVidInfo);
        DeleteGop(pGopInfo, index);
        Sem_unlock(STREAM_SEM_GOP);
        if (ret == CACHE_NOMEM)
            return GOP_NOMEM;
        else
            return GOP_FAIL;
    }
    pGopInfo->end_serial = cur_serial;
    Sem_unlock(STREAM_SEM_GOP);
    // if(GetGopInfoBySerial(cur_serial,index)== NULL)
    // return GOP_FAIL;
    return GOP_COMPLETE;
}

/**
 * @brief	Show all GOP
 * @param	"int index" : index
 * @return	GOP_COMPLETE(0) : success ; others : fail
 */
/* ===================================================================
 *  @func     ShowAllGop
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
void ShowAllGop(int index)
{
    GopInfo_t *pGopInfo;

    ERR("ShowAllGop Start\n");
    for (pGopInfo = gGopList[index]; pGopInfo != NULL;
         pGopInfo = pGopInfo->next)
    {
        ERR("start_serial: %d\n", pGopInfo->start_serial);
        ERR("end_serial: %d\n", pGopInfo->end_serial);
        ERR("lock: %d\n", pGopInfo->lock);
    }
    ERR("ShowAllGop End\n");
}
