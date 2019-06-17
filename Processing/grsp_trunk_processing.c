/*
============================================
 檔案名稱：grsp_trunk_processing.c
 程式功能：RFID Scanner Trunk 處理模組
 設計者：羅富元 (5107056012)
 版權擁有者：Seamus Berloz (軒摩斯貝爾羅)
 最後更新日期：2019/06/09 - 11:32:29
 其他資訊：
============================================
*/
/*---------------------------------------------------------------------------*/
/* 包含檔定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 自定義包含檔 =- */
/* 硬體抽象層程式庫定義包含檔 */
#ifndef GHLD_LIBRARY
  #include "include/ghld_hallib_define.h"
#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 標準包含檔 =- */
/* 在未搭配硬體抽象層程式庫定義時引入相關標準包含檔 */
#if ((!defined(GHLD_LIBRARY)) || (GHLD_LIBRARY == GHLD_LIBRARY_NONE))
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 自定義包含檔 =- */
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之常數定義宣告 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之巨集定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之資料結構型態列舉定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用資料結構型態定義宣告 =- */
/* Trunk Menu 參照列表資料結構型態定義宣告 */
typedef struct _GRSP_TRUNK_MENU_REFERENCE_DATAGRAM_STRUCTURE
  {
    int iMenuItemListLength;
    const GRSTMIITEMDS *lpMenuItemList;
  } GRSPTMRDS, *LPGRSPTMRDS;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 狀態機步驟常數列舉定義宣告 =- */
/* Trunk 狀態機步驟常數列舉定義宣告 */
enum _GRSP_TRUNK_FSMD_STEP_DEFINITION
  {
    /* -= 狀態機預備 =- */
    GRSP_TRUNK_FSMD_STEP_READY,
    
    /* -= 狀態機步驟 =- */
    GRSP_TRUNK_FSMD_STEP_HMI_SETUP,
    
    GRSP_TRUNK_FSMD_STEP_LOAD_RESOURCE,
    
    GRSP_TRUNK_FSMD_STEP_STATUS_DISPLAY,
    GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY,
    
    GRSP_TRUNK_FSMD_STEP_MENU,
    
    GRSP_TRUNK_FSMD_STEP_TAG_SCAN,
    GRSP_TRUNK_FSMD_STEP_TAG_COMMIT,
    
    /* -= 狀態機返回 =- */
    GRSP_TRUNK_FSMD_STEP_REVERSION,
    
    /* -= 狀態機意外 =- */
    GRSP_TRUNK_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之變數定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用運作變數定義宣告 =- */
/* Main Menu Item 樣板列表變數定義宣告 */
static const GRSTMIITEMDS g_gmidGRSPTrunkMainMenuItemSampleList[] =
  {
    /*-----------------------------------------*/
    /* -= 列表 =- */
    /*---------------------*/
    /* 掃描 */
      {
        /* 識別 */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_SCAN,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_EXECUTE,
        (-1),
        (-1),
        GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_SCAN,
        
        /* 數據與名稱 */
        0, 0, "Tag Scan"
      },
    /*---------------------*/
    /* 提交 */
      {
        /* 識別 */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_COMMIT,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_EXECUTE,
        (-1),
        (-1),
        GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_COMMIT,
        
        /* 數據與名稱 */
        0, 0, "Tag Commit"
      },
    /*---------------------*/
    /* 狀態 */
      {
        /* 識別 */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_STATUS,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_ENTER,
        GRSTMI_TRUNK_MENU_STATUS,
        (-1),
        (-1),
        
        /* 數據與名稱 */
        0, 0, "Status"
      },
    /*---------------------*/
    /* 位置 */
      {
        /* 識別 */
        GRSTMI_TRUNK_MAIN_MENU_ITEM_INDEX_LOCATION,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_ENTER,
        GRSTMI_TRUNK_MENU_LOCATION,
        (-1),
        (-1),
        
        /* 數據與名稱 */
        0, 0, "Location"
      },
    
    /*-----------------------------------------*/
    /* -= 列表結尾 =- */
      {(-1), 0, 0, 0, 0, 0, 0, ""},
  };

/* Status Menu Item 樣板列表變數定義宣告 */
static const GRSTMIITEMDS g_gmidGRSPTrunkStatusMenuItemSampleList[] =
  {
    /*-----------------------------------------*/
    /* -= 列表 =- */
    /*---------------------*/
    /* Wash Start */
      {
        /* 識別 */
        GRSTMI_TRUNK_STATUS_MENU_ITEM_INDEX_WASH_START,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* 數據與名稱 */
        1, 1, "Wash Start"
      },
    /*---------------------*/
    /* Wash Finish */
      {
        /* 識別 */
        GRSTMI_TRUNK_STATUS_MENU_ITEM_INDEX_WASH_FINISH,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* 數據與名稱 */
        2, 1, "Wash Finish"
      },
    
    /*-----------------------------------------*/
    /* -= 列表結尾 =- */
      {(-1), 0, 0, 0, 0, 0, 0, ""},
  };

/* Location Menu Item 樣板列表變數定義宣告 */
static const GRSTMIITEMDS g_gmidGRSPTrunkLocationMenuItemSampleList[] =
  {
    /*-----------------------------------------*/
    /* -= 列表 =- */
    /*---------------------*/
    /* Starting Point */
      {
        /* 識別 */
        GRSTMI_TRUNK_LOCATION_MENU_ITEM_INDEX_STARTING_POINT,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* 數據與名稱 */
        1, 1, "Starting Point"
      },
    /*---------------------*/
    /* Terminal Point */
      {
        /* 識別 */
        GRSTMI_TRUNK_LOCATION_MENU_ITEM_INDEX_TERMINAL_POINT,
        
        /* 操作 */
        GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT,
        (-1),
        GRSTMI_TRUNK_MENU_MAIN,
        (-1),
        
        /* 數據與名稱 */
        2, 1, "Terminal Point"
      },
    
    /*-----------------------------------------*/
    /* -= 列表結尾 =- */
      {(-1), 0, 0, 0, 0, 0, 0, ""},
  };

/* Trunk Menu 參照列表變數定義宣告 */
static const GRSPTMRDS g_gtrdGRSPTrunkMenuReferenceList[] =
  {
    /*-----------------------------------------*/
    /* -= 列表 =- */
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
    /* -= 列表結尾 =- */
      {0, NULL},
  };

/*---------------------------------------------------------------------------*/
/* 程序原型定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 基本公用程序 =- */
/* 初始運作與終止程序 */
GRSTRUNKCDS * GRSPTrunkInitialize(GRSTRUNKINIDS *lpInitializationDS);
GRESULT GRSPTrunkOperate(GRSTRUNKCDS *lpControlDS);
void GRSPTrunkTerminate(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 私用程序原型宣告定義 =- */
/* 處理程序 */
static GRESULT GRSPTrunkFSMD(GRSTRUNKCDS *lpControlDS);
/* 工具程序 */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 回呼程序 =- */
static GRESULT GRSRCALLBACK GRSPTrunkOnPress(void *lpParameter, int iButtonIndex);
static GRESULT GRSRCALLBACK GRSPTrunkOnRelease(void *lpParameter, int iButtonIndex);

/*---------------------------------------------------------------------------*/
/* Trunk 初始化處理程序 */
GRSTRUNKCDS * GRSPTrunkInitialize(GRSTRUNKINIDS *lpInitializationDS)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    GRESULT hr;
    
    int i, iIndex;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 參數檢查 =- */
    if(lpInitializationDS == NULL)
      {
        return NULL;
      }
    /* 日誌 */
    else if(lpInitializationDS->pSystemJournal == NULL)
      {
        return NULL;
      }
    /* 資源 */
    else if((lpInitializationDS->pResourceQueryQueue == NULL) || (lpInitializationDS->pScannerQueryQueue == NULL))
      {
        return NULL;
      }
    /* 人機 */
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
    /* -= 初始化 Trunk 控制組資料結構空間 =- */
    if((lpControlDS = (GRSTRUNKCDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTRUNKCDS))) == NULL)
      {
        GSJ_ERROR(lpInitializationDS->pSystemJournal, "配置資料所需空間時，記憶體不足！錯誤碼：0x%08X。\n", GGD_ERROR_OUT_OF_MEMORY);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 初始預備 =- */
    memset(lpControlDS, 0, sizeof(GRSTRUNKCDS));
    /* 日誌 */
    lpControlDS->pSystemJournal = lpInitializationDS->pSystemJournal;
    /* 資源 */
    lpControlDS->pResourceQueryQueue = lpInitializationDS->pResourceQueryQueue;
    /* 人機 */
    lpControlDS->pDisplayQueue = lpInitializationDS->pDisplayQueue;
    lpControlDS->pBuzzerDatagramQueue = lpInitializationDS->pBuzzerDatagramQueue;
    lpControlDS->pButtonRegisterQueue = lpInitializationDS->pButtonRegisterQueue;
    /* Scanner */
    lpControlDS->pScannerQueryQueue = lpInitializationDS->pScannerQueryQueue;
    lpControlDS->pScannerInstructionQueue = lpInitializationDS->pScannerInstructionQueue;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 即時時鐘物件初始化 =- */
    /* 建立 */
    if((lpControlDS->pRealTimeClock = GRealTimeClockCreate()) == NULL)
      {
        GSJ_ERROR(lpInitializationDS->pSystemJournal, "Create GRealTimeClock FAILED! Error code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
        
        return NULL;
      }
    /* 初始化 */
    else if(GGD_FAILED(hr = GRTC_GET_METHOD(lpControlDS->pRealTimeClock)->Initialize(lpControlDS->pRealTimeClock)))
      {
        GSJ_ERROR(lpInitializationDS->pSystemJournal, "GRealTimeClock initialization FAILED! Error code: 0x%08X.\n", hr);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= Item Tree 物件 =- */
    /* 配置 */
    if((lpControlDS->pItemTree = GRSTItemTreeCreate()) == NULL)
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "Create GRSTItemTree FAILED! Error Code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
        
        return NULL;
      }
    /* 初始化 */
    else if(GGD_FAILED(hr = GAVLT_GET_METHOD((GAVLTree *)(lpControlDS->pItemTree))->Initialize((GAVLTree *)(lpControlDS->pItemTree))))
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "GRSTItemTree initialization FAILED! Error Code: 0x%08X.\n", hr);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* Query Result 佇列物件 */
    /* 配置 */
    if((lpControlDS->pQueryResultQueue = GRSResultQueueCreate()) == NULL)
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "Create GRSResultQueue FAILED! Error Code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
        
        return NULL;
      }
    /* 初始化 */
    else if(GGD_FAILED(hr = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->Initialize((GSerialQueue *)(lpControlDS->pQueryResultQueue))))
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "GRSResultQueue initialization FAILED! Error Code: 0x%08X.\n", hr);
        
        return NULL;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= Menu Item 佇列物件 =- */
    /* 建立並初始化 */
    for(iIndex = 0; iIndex < GRSTMI_TRUNK_MENU_AMOUNT; iIndex ++)
      {
        /* 配置 */
        if((lpControlDS->pMenuItemQueue[iIndex] = GRSTMenuItemQueueCreate()) == NULL)
          {
            GSJ_ERROR(lpInitializationDS->pSystemJournal, "Create GRSTMenuItemQueue FAILED! Error Code: 0x%08X.\n", GGD_ERROR_UNEXPECTED);
            
            return NULL;
          }
        /* 初始化 */
        else if(GGD_FAILED(hr = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pMenuItemQueue[iIndex]))->Initialize((GSerialQueue *)(lpControlDS->pMenuItemQueue[iIndex]))))
          {
            GSJ_ERROR(lpInitializationDS->pSystemJournal, "GRSTMenuItemQueue initialization FAILED! Error Code: 0x%08X.\n", hr);
            
            return NULL;
          }
        
        /* 基本載裝 */
        for(i = 0; g_gtrdGRSPTrunkMenuReferenceList[iIndex].lpMenuItemList[i].iIndex >= 0; i ++)
          {
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            
            /* Menu Item 記憶體空間配置 */
            if((lpMenuItemDS = (GRSTMIITEMDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTMIITEMDS))) == NULL)
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Menu Item 記憶體空間配置時，記憶體不足！錯誤碼：0x%08X。\n", GGD_ERROR_OUT_OF_MEMORY);
                
                return NULL;
              }
            
            /* 保存 */
            memcpy(lpMenuItemDS, &(g_gtrdGRSPTrunkMenuReferenceList[iIndex].lpMenuItemList[i]), sizeof(GRSTMIITEMDS));
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pMenuItemQueue[iIndex])->Add(lpControlDS->pMenuItemQueue[iIndex], lpMenuItemDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return NULL;
              }
          }
      }
    
    return lpControlDS;
  }

/*---------------------------------------------------------------------------*/
/* Trunk 終止處理程序 */
void GRSPTrunkTerminate(GRSTRUNKCDS *lpControlDS)
  {
    int iIndex;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 參數檢查 =- */
    if(lpControlDS == NULL)
      {
        return;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 釋放控制物件組 =- */
    /* 釋放佇列與 Tree 物件 */
    GOC_SAFE_DESTROY(lpControlDS->pItemTree);
    GOC_SAFE_DESTROY(lpControlDS->pQueryResultQueue);
    for(iIndex = 0; iIndex < GRSTMI_TRUNK_MENU_AMOUNT; iIndex ++)
      {
        GOC_SAFE_DESTROY(lpControlDS->pMenuItemQueue[iIndex]);
      }
    
    /* 即時時鐘物件 */
    GOC_SAFE_DESTROY(lpControlDS->pRealTimeClock);
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 釋放控制組資料結構空間 =- */
    GGD_MEMORY_RELEASE(lpControlDS);
    
  }

/*---------------------------------------------------------------------------*/
/* Trunk 運作處理程序 */
GRESULT GRSPTrunkOperate(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 參數檢查 =- */
    if(lpControlDS == NULL)
      {
        return GGD_ERROR_INVALID_ARGUMENT;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 執行運作狀態機 =- */
    if(GGD_FAILED(hr = GRSPTrunkFSMD(lpControlDS)))
      {
        GSJ_ERROR(lpControlDS->pSystemJournal, "執行運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
        
        return hr;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Trunk 處理狀態機 */
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
    /* -= 執行輸入狀態變化偵測狀態機 =- */
    switch(lpControlDS->iOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機預備 =- */
        case GRSP_TRUNK_FSMD_STEP_READY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態機步驟切換 */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_HMI_SETUP;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 人機介面設置 =- */
        case GRSP_TRUNK_FSMD_STEP_HMI_SETUP:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSBRIREGISTERDS gbrdButtonRegisterDS;
            
            /*-----------------------------------------*/
            /* 註冊按鍵資料 */
            memset(&gbrdButtonRegisterDS, 0, sizeof(GRSBRIREGISTERDS));
            gbrdButtonRegisterDS.iValidFlags = GRSBRI_BUTTON_FLAG_SELECT | GRSBRI_BUTTON_FLAG_ENTER | GRSBRI_BUTTON_FLAG_ESCAPE | GRSBRI_BUTTON_FLAG_TRIGGER;
            gbrdButtonRegisterDS.OnPress = GRSPTrunkOnPress;
            gbrdButtonRegisterDS.OnRelease = GRSPTrunkOnRelease;
            gbrdButtonRegisterDS.lpUserParameter = lpControlDS;
            if(GGD_FAILED(hr = GRSBRQ_GET_METHOD(lpControlDS->pButtonRegisterQueue)->Add(lpControlDS->pButtonRegisterQueue, &gbrdButtonRegisterDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSButtonRegisterQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 按鍵初始狀態 */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
            
            /* 狀態機步驟切換 */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_LOAD_RESOURCE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Status Display =- */
        case GRSP_TRUNK_FSMD_STEP_LOAD_RESOURCE:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 執行 Load Resource 運作狀態機 */
            if(GGD_FAILED(hr = GRSPTrunkLoadResourceFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "執行 Load Resource 運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            else if(hr == GGD_OK)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            else if(hr == GGD_FALSE)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Status Display =- */
        case GRSP_TRUNK_FSMD_STEP_STATUS_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 執行 Status 運作狀態機 */
            if(GGD_FAILED(hr = GRSPTrunkStatusFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "執行 Status 運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
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
                
                /* 狀態機步驟切換 */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Datetime Display =- */
        case GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 執行 Datetime 運作狀態機 */
            if(GGD_FAILED(hr = GRSPTrunkDatetimeFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "執行 Datetime 運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            else if(hr == GGD_OK)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_MENU;
                  }
                else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_STATUS_DISPLAY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu =- */
        case GRSP_TRUNK_FSMD_STEP_MENU:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 執行 Menu 運作狀態機 */
            if(GGD_FAILED(hr = GRSPTrunkMenuFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "執行 Menu 運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 運作遭到退出 */
            else if(hr == GGD_FALSE)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
              }
            /* 操作產生執行目標 */
            else if(hr == GGD_OK)
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ENTER)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
                  }
                
                /* 檢查執行目標 */
                if(lpControlDS->iExecutionSelection == GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_SCAN)
                  {
                    /* 狀態機步驟切換 */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_TAG_SCAN;
                  }
                else if(lpControlDS->iExecutionSelection == GRSTMI_TRUNK_MENU_EXECUTION_SELECTION_COMMIT)
                  {
                    /* 狀態機步驟切換 */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_TAG_COMMIT;
                  }
                else
                  {
                    /* 狀態機步驟切換 */
                    lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_DATETIME_DISPLAY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Tag Scan =- */
        case GRSP_TRUNK_FSMD_STEP_TAG_SCAN:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 執行 Tag Scan 運作狀態機 */
            if(GGD_FAILED(hr = GRSPTrunkTagScanFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "執行 Tag Scan 運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 操作結束 */
            else if((hr == GGD_OK) || (hr == GGD_FALSE))
              {
                if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
                  {
                    lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_MENU;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Tag Commit =- */
        case GRSP_TRUNK_FSMD_STEP_TAG_COMMIT:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 執行 Tag Commit 運作狀態機 */
            if(GGD_FAILED(hr = GRSPTrunkTagCommitFSMD(lpControlDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "執行 Tag Scan 運作狀態機處理時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 操作結束 */
            else if((hr == GGD_OK) || (hr == GGD_FALSE))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_MENU;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機返回 =- */
        case GRSP_TRUNK_FSMD_STEP_REVERSION:
          {
            /* 回應上層邏輯控制，目前運作已經完成 */
            hResult = GGD_OK;
            
            /* 狀態步驟返回 */
            lpControlDS->iOperationFSMDStep = GRSP_TRUNK_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機意外 =- */
        default:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態步驟返回 */
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
    /* 分析並設立對應旗標 */
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
    /* 分析並設立對應旗標 */
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
/* 檔案結束 */

