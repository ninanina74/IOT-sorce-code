/*
============================================
 �ɮצW�١Ggrsp_trunk_processing.c
 �{���\��GRFID Scanner Trunk �B�z�Ҳ�
 �]�p�̡Gù�I�� (5107056012)
 ���v�֦��̡GSeamus Berloz (�a��������ù)
 �̫��s����G2019/06/09 - 11:32:29
 ��L��T�G
============================================
*/
/*---------------------------------------------------------------------------*/
/* �]�t�ɩw�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �۩w�q�]�t�� =- */
/* �w���H�h�{���w�w�q�]�t�� */
#ifndef GHLD_LIBRARY
  #include "include/ghld_hallib_define.h"
#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �зǥ]�t�� =- */
/* �b���f�t�w���H�h�{���w�w�q�ɤޤJ�����зǥ]�t�� */
#if ((!defined(GHLD_LIBRARY)) || (GHLD_LIBRARY == GHLD_LIBRARY_NONE))
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �۩w�q�]�t�� =- */
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��`�Ʃw�q�ŧi */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ������w�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ���Ƶ��c���A�C�|�w�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�θ�Ƶ��c���A�w�q�ŧi =- */
/* Trunk Menu �ѷӦC���Ƶ��c���A�w�q�ŧi */
typedef struct _GRSP_TRUNK_MENU_REFERENCE_DATAGRAM_STRUCTURE
  {
    int iMenuItemListLength;
    const GRSTMIITEMDS *lpMenuItemList;
  } GRSPTMRDS, *LPGRSPTMRDS;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���A���B�J�`�ƦC�|�w�q�ŧi =- */
/* Trunk ���A���B�J�`�ƦC�|�w�q�ŧi */
enum _GRSP_TRUNK_FSMD_STEP_DEFINITION
  {
    /* -= ���A���w�� =- */
    GRSP_TRUNK_FSMD_STEP_READY,
    
    /* -= ���A���B�J =- */
    GRSP_TRUNK_FSMD_STEP_HMI_SETUP,
    
    GRSP_TRUNK_FSMD_STEP_LOAD_RESOURCE,
    
    GRSP_TRUNK_FSMD_STEP_STATUS_DISPLAY,
    GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY,
    
    GRSP_TRUNK_FSMD_STEP_MENU,
    
    GRSP_TRUNK_FSMD_STEP_TAG_SCAN,
    GRSP_TRUNK_FSMD_STEP_TAG_COMMIT,
    
    /* -= ���A����^ =- */
    GRSP_TRUNK_FSMD_STEP_REVERSION,
    
    /* -= ���A���N�~ =- */
    GRSP_TRUNK_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ܼƩw�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�ιB�@�ܼƩw�q�ŧi =- */
/* Main Menu Item �˪O�C���ܼƩw�q�ŧi */
static const GRSTMIITEMDS g_gmidGRSPTrunkMainMenuItemSampleList[] =
  {
    /*-----------------------------------------*/
    /* -= �C�� =- */
    /*---------------------*/
    /* ���y */
      {
        /* �ѧO */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_SCAN,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_EXECUTE,
        (-1),
        (-1),
        GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_SCAN,
        
        /* �ƾڻP�W�� */
        0, 0, "Tag Scan"
      },
    /*---------------------*/
    /* ���� */
      {
        /* �ѧO */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_COMMIT,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_EXECUTE,
        (-1),
        (-1),
        GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_COMMIT,
        
        /* �ƾڻP�W�� */
        0, 0, "Tag Commit"
      },
    /*---------------------*/
    /* ���A */
      {
        /* �ѧO */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_STATUS,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_ENTER,
        GRSTMI_TRUNK_MENU_STATUS,
        (-1),
        (-1),
        
        /* �ƾڻP�W�� */
        0, 0, "Status"
      },
    /*---------------------*/
    /* ��m */
      {
        /* �ѧO */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_LOCATION,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_ENTER,
        GRSTMI_TRUNK_MENU_LOCATION,
        (-1),
        (-1),
        
        /* �ƾڻP�W�� */
        0, 0, "Location"
      },
    
    /*-----------------------------------------*/
    /* -= �C���� =- */
      {(-1), 0, 0, 0, 0, 0, 0, ""},
  };

/* Status Menu Item �˪O�C���ܼƩw�q�ŧi */
static const GRSTMIITEMDS g_gmidGRSPTrunkStatusMenuItemSampleList[] =
  {
    /*-----------------------------------------*/
    /* -= �C�� =- */
    /*---------------------*/
    /* Wash Start */
      {
        /* �ѧO */
        GRSTMI_TRUNK_STATUS_MENU_ITEM_INDEX_WASH_START,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* �ƾڻP�W�� */
        1, 1, "Wash Start"
      },
    /*---------------------*/
    /* Wash Finish */
      {
        /* �ѧO */
        GRSTMI_TRUNK_STATUS_MENU_ITEM_INDEX_WASH_FINISH,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* �ƾڻP�W�� */
        2, 1, "Wash Finish"
      },
    
    /*-----------------------------------------*/
    /* -= �C���� =- */
      {(-1), 0, 0, 0, 0, 0, 0, ""},
  };

/* Location Menu Item �˪O�C���ܼƩw�q�ŧi */
static const GRSTMIITEMDS g_gmidGRSPTrunkLocationMenuItemSampleList[] =
  {
    /*-----------------------------------------*/
    /* -= �C�� =- */
    /*---------------------*/
    /* Starting Point */
      {
        /* �ѧO */
        GRSTMI_TRUNK_LOCATION_MENU_ITEM_INDEX_STARTING_POINT,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* �ƾڻP�W�� */
        1, 1, "Starting Point"
      },
    /*---------------------*/
    /* Terminal Point */
      {
        /* �ѧO */
        GRSTMI_TRUNK_LOCATION_MENU_ITEM_INDEX_TERMINAL_POINT,
        
        /* �ާ@ */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* �ƾڻP�W�� */
        2, 1, "Terminal Point"
      },
    
    /*-----------------------------------------*/
    /* -= �C���� =- */
      {(-1), 0, 0, 0, 0, 0, 0, ""},
  };

/* Trunk Menu �ѷӦC���ܼƩw�q�ŧi */
static const GRSPTMRDS g_gtrdGRSPTrunkMenuReferenceList[] =
  {
    /*-----------------------------------------*/
    /* -= �C�� =- */
    /*---------------------*/
    /* Main Menu Item List */
      {
        sizeof(g_gmidGRSPTrunkMainMenuItemSampleList) / sizeof(GRSTMIITEMDS),
        g_gmidGRSPTrunkMainMenuItemSampleList
      },
    /*---------------------*/
    /* Status Menu Item List */
      {
        sizeof(g_gmidGRSPTrunkStatusMenuItemSampleList) / sizeof(GRSTMIITEMDS),
        g_gmidGRSPTrunkStatusMenuItemSampleList
      },
    /*---------------------*/
    /* Location Menu Item List */
      {
        sizeof(g_gmidGRSPTrunkLocationMenuItemSampleList) / sizeof(GRSTMIITEMDS),
        g_gmidGRSPTrunkLocationMenuItemSampleList
      },
    
    /*-----------------------------------------*/
    /* -= �C���� =- */
      {0, NULL},
  };

/*---------------------------------------------------------------------------*/
/* �{�ǭ쫬�w�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �򥻤��ε{�� =- */
/* ��l�B�@�P�פ�{�� */
GRSTRUNKCDS * GRSPTrunkInitialize(GRSTRUNKINIDS *lpInitializationDS);
GRESULT GRSPTrunkOperate(GRSTRUNKCDS *lpControlDS);
void GRSPTrunkTerminate(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �p�ε{�ǭ쫬�ŧi�w�q =- */
/* �B�z�{�� */
static GRESULT GRSPTrunkFSMD(GRSTRUNKCDS *lpControlDS);
/* �u��{�� */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �^�I�{�� =- */
static GRESULT GRSRCALLBACK GRSPTrunkOnPress(void *lpParameter, int iButtonIndex);
static GRESULT GRSRCALLBACK GRSPTrunkOnRelease(void *lpParameter, int iButtonIndex);

/*---------------------------------------------------------------------------*/
/* Trunk ��l�ƳB�z�{�� */
GRSTRUNKCDS * GRSPTrunkInitialize(GRSTRUNKINIDS *lpInitializationDS)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    GRESULT hr;
    
    int i, iIndex;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= �Ѽ��ˬd =- */
    if(lpInitializationDS == NULL)
      {
        return NULL;
      }
    /* ��x */
    else if(lpInitializationDS->pSystemJournal == NULL)
      {
        return NULL;
      }
    /* �귽 */
    else if((lpInitializationDS->pResourceQueryQueue == NULL) || (lpInitializationDS->pScannerQueryQueue == NULL))
      {
        return NULL;
      }
    /* �H�� */
    else if((lpInitializationDS->pDisplayQueue == NULL) || (lpInitializationDS->pBuzzerDatagramQueue == NULL) || (lpInitializationDS->pButtonRegisterQueue == NULL))
      {
        return NULL;
      }
    /* Scanner */
    else if(lpInitializationDS->pScannerInstructionQueue == NULL)
      {
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= ��l�� Trunk ����ո�Ƶ��c�Ŷ� =- */
    if((lpControlDS = (GRSTRUNKCDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTRUNKCDS))) == NULL)
      {
        GSJ_ERROR(lpInitializationDS->pSystemJournal, "�t�m��ƩһݪŶ��ɡA�O���餣���I���~�X�G0x%08X�C\n", GGD_ERROR_OUT_OF_MEMORY);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= ��l�w�� =- */
    memset(lpControlDS, 0, sizeof(GRSTRUNKCDS));
    /* ��x */
    lpControlDS->pSystemJournal = lpInitializationDS->pSystemJournal;
    /* �귽 */
    lpControlDS->pResourceQueryQueue = lpInitializationDS->pResourceQueryQueue;
    /* �H�� */
    lpControlDS->pDisplayQueue = lpInitializationDS->pDisplayQueue;
    lpControlDS->pBuzzerDatagramQueue = lpInitializationDS->pBuzzerDatagramQueue;
    lpControlDS->pButtonRegisterQueue = lpInitializationDS->pButtonRegisterQueue;
    /* Scanner */
    lpControlDS->pScannerQueryQueue = lpInitializationDS->pScannerQueryQueue;
    lpControlDS->pScannerInstructionQueue = lpInitializationDS->pScannerInstructionQueue;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= �Y�ɮ��������l�� =- */
    /* �إ� */
    if((lpControlDS->pRealTimeClock = GRealTimeClockCreate()) == NULL)
      {
        GSJ_ERROR(lpInitializationDS->pSystemJournal, "Create GRealTimeClock FAILED! Error code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
        
        return NULL;
      }
    /* ��l�� */
    else if(GGD_FAILED(hr = GRTC_GET_METHOD(lpControlDS->pRealTimeClock)->Initialize(lpControlDS->pRealTimeClock)))
      {
        GSJ_ERROR(lpInitializationDS->pSystemJournal, "GRealTimeClock initialization FAILED! Error code: 0x%08X.\n", hr);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= Item Tree ���� =- */
    /* �t�m */
    if((lpControlDS->pItemTree = GRSTItemTreeCreate()) == NULL)
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "Create GRSTItemTree FAILED! Error Code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
        
        return NULL;
      }
    /* ��l�� */
    else if(GGD_FAILED(hr = GAVLT_GET_METHOD((GAVLTree *)(lpControlDS->pItemTree))->Initialize((GAVLTree *)(lpControlDS->pItemTree))))
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "GRSTItemTree initialization FAILED! Error Code: 0x%08X.\n", hr);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* Query Result ��C���� */
    /* �t�m */
    if((lpControlDS->pQueryResultQueue = GRSResultQueueCreate()) == NULL)
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "Create GRSResultQueue FAILED! Error Code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
        
        return NULL;
      }
    /* ��l�� */
    else if(GGD_FAILED(hr = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->Initialize((GSerialQueue *)(lpControlDS->pQueryResultQueue))))
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "GRSResultQueue initialization FAILED! Error Code: 0x%08X.\n", hr);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= Menu Item ��C���� =- */
    /* �إߨê�l�� */
    for(iIndex = 0; iIndex < GRSTMI_TRUNK_MENU_AMOUNT; iIndex ++)
      {
        /* �t�m */
        if((lpControlDS->pMenuItemQueue[iIndex] = GRSTMenuItemQueueCreate()) == NULL)
          {
            GSJ_ERROR(lpInitializationDS->pSystemJournal, "Create GRSTMenuItemQueue FAILED! Error Code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
            
            return NULL;
          }
        /* ��l�� */
        else if(GGD_FAILED(hr = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pMenuItemQueue[iIndex]))->Initialize((GSerialQueue *)(lpControlDS->pMenuItemQueue[iIndex]))))
          {
            GSJ_ERROR(lpInitializationDS->pSystemJournal, "GRSTMenuItemQueue initialization FAILED! Error Code: 0x%08X.\n", hr);
            
            return NULL;
          }
        
        /* �򥻸��� */
        for(i = 0; g_gtrdGRSPTrunkMenuReferenceList[iIndex].lpMenuItemList[i].iIndex >= 0; i ++)
          {
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            
            /* Menu Item �O����Ŷ��t�m */
            if((lpMenuItemDS = (GRSTMIITEMDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTMIITEMDS))) == NULL)
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Menu Item �O����Ŷ��t�m�ɡA�O���餣���I���~�X�G0x%08X�C\n", GGD_ERROR_OUT_OF_MEMORY);
                
                return NULL;
              }
            
            /* �O�s */
            memcpy(lpMenuItemDS, &(g_gtrdGRSPTrunkMenuReferenceList[iIndex].lpMenuItemList[i]), sizeof(GRSTMIITEMDS));
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pMenuItemQueue[iIndex])->Add(lpControlDS->pMenuItemQueue[iIndex], lpMenuItemDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return NULL;
              }
          }
      }
    
    return lpControlDS;
  }

/*---------------------------------------------------------------------------*/
/* Trunk �פ�B�z�{�� */
void GRSPTrunkTerminate(GRSTRUNKCDS *lpControlDS)
  {
    int iIndex;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= �Ѽ��ˬd =- */
    if(lpControlDS == NULL)
      {
        return;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= ���񱱨��� =- */
    /* �����C�P Tree ���� */
    GOC_SAFE_DESTROY(lpControlDS->pItemTree);
    GOC_SAFE_DESTROY(lpControlDS->pQueryResultQueue);
    for(iIndex = 0; iIndex < GRSTMI_TRUNK_MENU_AMOUNT; iIndex ++)
      {
        GOC_SAFE_DESTROY(lpControlDS->pMenuItemQueue[iIndex]);
      }
    
    /* �Y�ɮ������� */
    GOC_SAFE_DESTROY(lpControlDS->pRealTimeClock);
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= ���񱱨�ո�Ƶ��c�Ŷ� =- */
    GGD_MEMORY_RELEASE(lpControlDS);
    
  }

/*---------------------------------------------------------------------------*/
/* Trunk �B�@�B�z�{�� */
GRESULT GRSPTrunkOperate(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= �Ѽ��ˬd =- */
    if(lpControlDS == NULL)
      {
        return GGD_ERROR_INVALID_ARGUMENT;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= ����B�@���A�� =- */
    if(GGD_FAILED(hr = GRSPTrunkFSMD(lpControlDS)))
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "����B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
        
        return hr;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Trunk �B�z���A�� */
static GRESULT GRSPTrunkFSMD(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr, hResult;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    if(lpControlDS->iOperationFSMDStep != lpControlDS->iOperationFSMDStepRecord)
      {
        GSJ_MESSAGE(lpControlDS->pSystemJournal, "Trunk FSMD step: %d...", lpControlDS->iOperationFSMDStep);
        
        lpControlDS->iOperationFSMDStepRecord = lpControlDS->iOperationFSMDStep;
      }
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= �����J���A�ܤư������A�� =- */
    switch(lpControlDS->iOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���w�� =- */
        case GRSP_TRUNK_FSMD_STEP_READY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A���B�J���� */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_HMI_SETUP;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �H�������]�m =- */
        case GRSP_TRUNK_FSMD_STEP_HMI_SETUP:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSBRIREGISTERDS gbrdButtonRegisterDS;
            
            /*-----------------------------------------*/
            /* ���U������ */
            memset(&gbrdButtonRegisterDS, 0, sizeof(GRSBRIREGISTERDS));
            gbrdButtonRegisterDS.iValidFlags = GRSBRI_BUTTON_FLAG_SELECT | GRSBRI_BUTTON_FLAG_ENTER | GRSBRI_BUTTON_FLAG_ESCAPE | GRSBRI_BUTTON_FLAG_TRIGGER;
            gbrdButtonRegisterDS.OnPress = GRSPTrunkOnPress;
            gbrdButtonRegisterDS.OnRelease = GRSPTrunkOnRelease;
            gbrdButtonRegisterDS.lpUserParameter = lpControlDS;
            if(GGD_FAILED(hr = GRSBRQ_GET_METHOD(lpControlDS->pButtonRegisterQueue)->Add(lpControlDS->pButtonRegisterQueue, &gbrdButtonRegisterDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSButtonRegisterQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �����l���A */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
            
            /* ���A���B�J���� */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_LOAD_RESOURCE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Status Display =- */
        case GRSP_TRUNK_FSMD_STEP_LOAD_RESOURCE:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ���� Load Resource �B�@���A�� */
            if(GGD_FAILED(hr = GRSPTrunkLoadResourceFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "���� Load Resource �B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            else if(hr == GGD_OK)
              {
                /* ���A���B�J���� */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            else if(hr == GGD_FALSE)
              {
                /* ���A���B�J���� */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Status Display =- */
        case GRSP_TRUNK_FSMD_STEP_STATUS_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ���� Status �B�@���A�� */
            if(GGD_FAILED(hr = GRSPTrunkStatusFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "���� Status �B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            else if(hr == GGD_OK)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
                  }
                else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                  }
                
                /* ���A���B�J���� */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Datetime Display =- */
        case GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ���� Datetime �B�@���A�� */
            if(GGD_FAILED(hr = GRSPTrunkDatetimeFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "���� Datetime �B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            else if(hr == GGD_OK)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
                    
                    /* ���A���B�J���� */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_MENU;
                  }
                else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                    
                    /* ���A���B�J���� */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_STATUS_DISPLAY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu =- */
        case GRSP_TRUNK_FSMD_STEP_MENU:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ���� Menu �B�@���A�� */
            if(GGD_FAILED(hr = GRSPTrunkMenuFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "���� Menu �B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* �B�@�D��h�X */
            else if(hr == GGD_FALSE)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                  }
                
                /* ���A���B�J���� */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            /* �ާ@���Ͱ���ؼ� */
            else if(hr == GGD_OK)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ENTER)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
                  }
                
                /* �ˬd����ؼ� */
                if(lpControlDS->iExecutionSelection == GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_SCAN)
                  {
                    /* ���A���B�J���� */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_TAG_SCAN;
                  }
                else if(lpControlDS->iExecutionSelection == GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_COMMIT)
                  {
                    /* ���A���B�J���� */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_TAG_COMMIT;
                  }
                else
                  {
                    /* ���A���B�J���� */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Tag Scan =- */
        case GRSP_TRUNK_FSMD_STEP_TAG_SCAN:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ���� Tag Scan �B�@���A�� */
            if(GGD_FAILED(hr = GRSPTrunkTagScanFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "���� Tag Scan �B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* �ާ@���� */
            else if((hr == GGD_OK) || (hr == GGD_FALSE))
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                  }
                
                /* ���A���B�J���� */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_MENU;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Tag Commit =- */
        case GRSP_TRUNK_FSMD_STEP_TAG_COMMIT:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ���� Tag Commit �B�@���A�� */
            if(GGD_FAILED(hr = GRSPTrunkTagCommitFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "���� Tag Scan �B�@���A���B�z�ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* �ާ@���� */
            else if((hr == GGD_OK) || (hr == GGD_FALSE))
              {
                /* ���A���B�J���� */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_MENU;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A����^ =- */
        case GRSP_TRUNK_FSMD_STEP_REVERSION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_OK;
            
            /* ���A�B�J��^ */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���N�~ =- */
        default:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A�B�J��^ */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Button Press Callback */
static GRESULT GRSRCALLBACK GRSPTrunkOnPress(void *lpParameter, int iButtonIndex)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    //GSJ_ADVANCE(lpControlDS->pSystemJournal, "GRSPTrunkOnPress...%d", iButtonIndex);
    
    /*-----------------------------------------*/
    /* ���R�ó]�߹����X�� */
    /* Select Button */
    if(iButtonIndex == GRSBRI_BUTTON_SELECT)
      {
        lpControlDS->iButtonPressFlags |= GRSBRI_BUTTON_FLAG_SELECT;
      }
    /* Enter Button */
    else if(iButtonIndex == GRSBRI_BUTTON_ENTER)
      {
        lpControlDS->iButtonPressFlags |= GRSBRI_BUTTON_FLAG_ENTER;
      }
    /* Escape Button */
    else if(iButtonIndex == GRSBRI_BUTTON_ESCAPE)
      {
        lpControlDS->iButtonPressFlags |= GRSBRI_BUTTON_FLAG_ESCAPE;
      }
    /* Trigger Button */
    else if(iButtonIndex == GRSBRI_BUTTON_TRIGGER)
      {
        lpControlDS->iButtonPressFlags |= GRSBRI_BUTTON_FLAG_TRIGGER;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Button Release Callback */
static GRESULT GRSRCALLBACK GRSPTrunkOnRelease(void *lpParameter, int iButtonIndex)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    //GSJ_ADVANCE(lpControlDS->pSystemJournal, "GRSPTrunkOnRelease...%d", iButtonIndex);
    
    /*-----------------------------------------*/
    /* ���R�ó]�߹����X�� */
    /* Select Button */
    if(iButtonIndex == GRSBRI_BUTTON_SELECT)
      {
        lpControlDS->iButtonReleaseFlags |= GRSBRI_BUTTON_FLAG_SELECT;
      }
    /* Enter Button */
    else if(iButtonIndex == GRSBRI_BUTTON_ENTER)
      {
        lpControlDS->iButtonReleaseFlags |= GRSBRI_BUTTON_FLAG_ENTER;
      }
    /* Escape Button */
    else if(iButtonIndex == GRSBRI_BUTTON_ESCAPE)
      {
        lpControlDS->iButtonReleaseFlags |= GRSBRI_BUTTON_FLAG_ESCAPE;
      }
    /* Trigger Button */
    else if(iButtonIndex == GRSBRI_BUTTON_TRIGGER)
      {
        lpControlDS->iButtonReleaseFlags |= GRSBRI_BUTTON_FLAG_TRIGGER;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* �ɮ׵��� */

