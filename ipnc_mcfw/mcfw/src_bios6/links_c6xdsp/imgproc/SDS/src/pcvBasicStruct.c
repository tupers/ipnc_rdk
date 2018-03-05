

#ifndef CXX_PCVBASICSTRUCT_C
#	include "pcvBasicStruct.h"
#endif

#ifndef CXX_PCVMISC_H
#	include "pcvMisc.h"
#endif

//pcvList operations
VOID pcvListCreateCell(pcvListCell** ppCell)
{
	pcvListCell* pListCell = NULL;
	pListCell =	(pcvListCell*)MV_Malloc(sizeof(pcvListCell));
	if (NULL != pListCell){
		*ppCell = pListCell;
	}
}

VOID pcvListDestroyCell(pcvListCell* pCell)
{
	if (NULL != pCell){
		MV_Free(pCell);
	}

}

BOOL pcvListInit(pcvList* pList)
{
	BOOL bRet = FALSE;
	if (NULL != pList){
		pList->pFirstCell = NULL;
		pList->pLastCell = NULL;
		pList->lItem = 0;
		bRet = TRUE;
	}

	return bRet;
}

VOID pcvListDeInit(pcvList* pList)
{
	pcvListClear(pList);
}

BOOL pcvListAdd(pcvList* pList, VOID *pData)
{
	pcvListCell*	pCell = NULL;
	BOOL bRet = FALSE;

	if (NULL != pList){
		pcvListCreateCell(&pCell);
		if (NULL != pCell){

			pCell->data = pData;
			++(pList->lItem);

			if (NULL != pList->pLastCell) {
				pCell->prev = pList->pLastCell;
				pList->pLastCell->next = pCell;
				pList->pLastCell = pCell;
			}
			else {
				pList->pFirstCell = pCell;
				pList->pLastCell = pCell;
			}

			bRet = TRUE;
		}

	}

	return bRet;

}

BOOL pcvListInsert(pcvList* pList, VOID* pData)
{
	pcvListCell*	pCell = NULL;
	BOOL bRet = FALSE;
	
	if (NULL != pList){

		pcvListCreateCell(&pCell);
		if (NULL != pCell){

			pCell->data = pData;
			++(pList->lItem);

			if (NULL != pList->pFirstCell){
				pCell->next = pList->pFirstCell;
				pList->pFirstCell->prev = pCell;
				pList->pFirstCell = pCell;
			} else {
				pList->pFirstCell = pCell;
				pList->pLastCell = pCell;
			}

			bRet = TRUE;
		}

	}

	return bRet;
}


BOOL pcvListInsertPos(pcvList* pList, VOID* pData, DWORD count)
{
	pcvListCell*	pCell = NULL;
	pcvListCell*	pNextCell = NULL;
	BOOL bRet = FALSE;

	if (NULL != pList){
		pcvListCreateCell(&pCell);
		if (NULL != pCell){
			INT i = 0;

			pCell->data = pData;
			++(pList->lItem);

			pNextCell = NULL;
			for (pNextCell = pList->pFirstCell, i = 1; NULL != pNextCell ;pNextCell = pNextCell->next, i++) {
				if (i == count)	break;
			}

			if (NULL != pNextCell){

				pCell->prev = pNextCell->prev;
				pCell->next = pNextCell;
				if (pNextCell == pList->pFirstCell){
					pList->pFirstCell = pCell;
				}else{			
					pNextCell->prev->next = pCell; /* modified by Asai '95.8.3 */
				}

				pNextCell->prev = pCell;
				return TRUE;
			}

			if (NULL != pList->pLastCell) {
				pCell->prev = pList->pLastCell;
				pList->pLastCell->next = pCell;
				pList->pLastCell = pCell;
			}
			else {
				pList->pFirstCell = pCell;
				pList->pLastCell = pCell;
			}

			return TRUE;

		}
	}

	return bRet;

}


VOID pcvListRemove(pcvList* pList, VOID* pData)
{
	if (NULL != pList){
		pcvListCell* pCell = NULL;

		for(pCell = pList->pFirstCell; NULL != pCell; pCell = pCell->next)

			if (pData == pCell->data){

				if (NULL != pCell) {
					if (pCell->next) pCell->next->prev = pCell->prev;
					if (pCell->prev) pCell->prev->next = pCell->next;
					if (pList->pFirstCell == pCell)  {
						pList->pFirstCell = pCell->next;
					}
					if (pList->pLastCell == pCell){
						pList->pLastCell = pCell->prev;
					}

					--(pList->lItem);

					pcvListDestroyCell(pCell);
				}

				break;
			}
	}

}

VOID*   pcvListPop(pcvList* pList)
{
	VOID *pData = NULL;

	if (NULL != pList){
		pcvListCell* pCell = pList->pFirstCell;
		if (NULL != pCell){
			pList->pFirstCell = pCell->next;
			if (pList->pLastCell == pCell){
				pList->pLastCell = NULL;
			}else{
				pList->pFirstCell->prev = NULL;
			}
            
            pData = pCell->data;
            --(pList->lItem);
            pcvListDestroyCell(pCell);
		}

	}

	return pData;

}

VOID*   pcvListNth(pcvList* pList, LONG lcnt)
{
	VOID *pData = NULL;

	if (NULL != pList){
		if ((lcnt <= pList->lItem) && (lcnt >= 0)){
		
			pcvListCell* pCell = NULL;

			for(pCell = pList->pLastCell; NULL != pCell; pCell = pCell->next){
				--lcnt;
				if (lcnt < 0){
					pData = pCell->data;
				}
			}
		}

	}

	return  pData;
}

VOID*   pcvListGetLast(pcvList* pList)
{
	VOID *pData = NULL;

	if (NULL != pList){
		if (NULL != pList->pLastCell){
			pData = pList->pLastCell->data;
		}

	}

	return pData;
}

VOID*   pcvListRemoveLast(pcvList* pList)
{
	pcvListCell* pCell = NULL;
	VOID* pData = NULL;

	if (NULL != pList){
		pCell = pList->pLastCell;

		if (NULL != pCell){
			pList->pLastCell = pCell->prev;

			if (pCell == pList->pFirstCell){
				pList->pFirstCell = NULL;
			}else{
				pList->pLastCell->next = NULL;
			}

			pData = pCell->data;
			--(pList->lItem);
			pcvListDestroyCell(pCell);
		}	
	}

	return pData;
	
}

LONG	pcvListLength(pcvList* pList)
{
	LONG lLen = 0;
	if (NULL != pList){
		lLen = pList->lItem;
	}

	return lLen;
}

VOID    pcvListClear(pcvList* pList){

	if (NULL != pList){

		pcvListCell*	pCell = NULL;
		pcvListCell*	pNext = NULL;

		pCell = pList->pFirstCell;
		while (pCell != NULL) {
			pNext = pCell->next;
			pcvListDestroyCell(pCell);
			pCell = pNext;
		}

		pList->pFirstCell = NULL;
		pList->pLastCell = NULL;
		pList->lItem = 0;
	}

}

//pcvQueue operations
BOOL	pcvQueueInit(pcvQueue* pQueue)
{
	if (NULL != pQueue){
		return pcvListInit(&(pQueue->list));
	}else{
		return FALSE;
	}

}

VOID	pcvQueueDeInit(pcvQueue* pQueue)
{
	if (NULL != pQueue){
		pcvListDeInit(&(pQueue->list));
	}
}

BOOL    pcvQueuePush(pcvQueue* pQueue,  VOID *pData)
{
	BOOL bRet = FALSE;
	if (NULL != pQueue){
		bRet = pcvListAdd(&(pQueue->list), pData);
	}
	
	return bRet;
}

VOID*   pcvQueuePop(pcvQueue* pQueue)
{
	VOID* pData = NULL;
	if (NULL != pQueue){
		pData = pcvListPop(&(pQueue->list));
	}

	return pData;
}

LONG    pcvQueueLength(pcvQueue* pQueue)
{
	LONG len = 0;
	if (NULL != pQueue){
		len = pcvListLength(&(pQueue->list));
	}
	return len;
}

VOID    pcvQueueClear(pcvQueue* pQueue)
{
	if (NULL != pQueue){
		pcvListClear(&(pQueue->list));
	}
}


/* EOF */
