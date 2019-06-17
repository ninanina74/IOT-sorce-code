/*
============================================
 檔案名稱：grsp_trunk_menu_processing.c
 程式功能：RFID Scanner Trunk Menu 處理
 設計者：羅富元 (5107056012)
 版權擁有者：Seamus Berloz (軒摩斯貝爾羅)
 最後更新日期：2019/06/09 - 11:32:29
 其他資訊：
============================================
*/
/*---------------------------------------------------------------------------*/
/* 包含標頭檔 */
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
/* -= 自定義標頭檔 =- */
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之常數定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用常數定義宣告 =- */
/* 顯示時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_MENU_SHOW_MILLISECOND_VALUE_LIMIT 1200
/* 退出時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_MENU_TIMEOUT_MILLISECOND_VALUE_LIMIT 8000

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之巨集定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之宣告型態定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 狀態機步驟常數列舉定義宣告 =- */
/* Trunk Menu 狀態機步驟常數列舉定義宣告 */
enum _GRSP_TRUNK_MENU_FSMD_STEP_DEFINITION
  {
    /* -= 狀態機預備 =- */
    GRSP_TRUNK_MENU_FSMD_STEP_READY,
    
    /* -= 狀態機步驟 =- */
    GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY,
    
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY,
    GRSP_TRUNK_MENU_FSMD_STEP_WAIT_TIME_INTERVAL,
    
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ENTER,
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ROTATE,
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ESCAPE,
    
    GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION,
    GRSP_TRUNK_MENU_FSMD_STEP_SHOW_TIME_INTERVAL,
    
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT,
    
    /* -= 狀態機返回 =- */
    GRSP_TRUNK_MENU_FSMD_STEP_REVERSION,
    
    /* -= 狀態機意外 =- */
    GRSP_TRUNK_MENU_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之變數定義 */

/*---------------------------------------------------------------------------*/
/* 函式原型定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 基本公用函數 =- */
/* 狀態機程序 */
GRESULT GRSPTrunkMenuFSMD(GRSTRUNKCDS *lpControlDS);

/*---------------------------------------------------------------------------*/
/* Status 處理狀態機 */
GRESULT GRSPTrunkMenuFSMD(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr, hResult;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    if(lpControlDS->iMenuOperationFSMDStep != lpControlDS->iMenuOperationFSMDStepRecord)
      {
        GSJ_MESSAGE(lpControlDS->pSystemJournal, "Trunk menu FSMD step: %d...", lpControlDS->iMenuOperationFSMDStep);
        
        lpControlDS->iMenuOperationFSMDStepRecord = lpControlDS->iMenuOperationFSMDStep;
      }
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 執行輸入狀態變化偵測狀態機 =- */
    switch(lpControlDS->iMenuOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機預備 =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_READY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 初始定址選單 */
            lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[GRSTMI_TRUNK_MENU_MAIN];
            lpControlDS->iMenuIndex = GRSTMI_TRUNK_MENU_MAIN;
            
            /* 狀態機步驟切換 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Clear Display =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於清除顯示 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            gdisDisplayDS.iFlags = GRSDISI_DISPLAY_FLAG_CONTROL | GRSDISI_DISPLAY_FLAG_HOMING | GRSDISI_DISPLAY_FLAG_CLEAR;
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /* 狀態機步驟切換 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Display =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* 取得 Menu Item */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Get(lpControlDS->pCurrentMenuItemQueue, &lpMenuItemDS, GSQ_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 如果不成功 */
            else if(hr != GRSTMIQ_ACCESS_OK)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            else
              {
                int iLengthRemainders;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示標題 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen(lpMenuItemDS->strTitleName);
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", lpMenuItemDS->strTitleName, iLengthRemainders, "                ");
                
                gdisDisplayDS.iXAxis = 0;
                gdisDisplayDS.iYAxis = lpMenuItemDS->iDisplayLine;
                
                /*---------------------*/
                /* 塞入 Display 資料 */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
              }
            
            /*-----------------------------------------*/
            /* 準備按鍵 */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            
            /*-----------------------------------------*/
            /* 設定逾時計時器 */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_MENU_TIMEOUT_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* 狀態機步驟切換 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_WAIT_TIME_INTERVAL;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候時間間隔 =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_WAIT_TIME_INTERVAL:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 檢查按鍵 */
            /* Select Button */
            if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ROTATE;
              }
            /* Escape Button */
            else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ESCAPE;
              }
            /* Enter Button */
            else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ENTER)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ENTER;
              }
            /*-----------------------------------------*/
            /* 計時逾時時間 */
            else if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Rotate =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ROTATE:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
                
            /*-----------------------------------------*/
            /* 塞入 Buzzer 資料 */
            memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
            gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
            if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* Menu Item 滾動 */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Rotate(lpControlDS->pCurrentMenuItemQueue)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /* 狀態機步驟切換 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Escape =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ESCAPE:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            GRSDIDDS gddsBuzzerDatagramDS;
            
            /*-----------------------------------------*/
            /* 取得 Menu Item */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Get(lpControlDS->pCurrentMenuItemQueue, &lpMenuItemDS, GSQ_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 如果不成功 */
            else if(hr != GRSTMIQ_ACCESS_OK)
              {
                /*---------------------*/
                /* 塞入 Buzzer 資料 */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            /*-----------------------------------------*/
            /* 檢查退出後的上層選單 */
            else if((lpMenuItemDS->iEscapeMenuIndex >= 0) && (lpMenuItemDS->iEscapeMenuIndex < GRSTMI_TRUNK_MENU_AMOUNT))
              {
                /*---------------------*/
                /* 切換選單 */
                lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[lpMenuItemDS->iEscapeMenuIndex];
                lpControlDS->iMenuIndex = lpMenuItemDS->iEscapeMenuIndex;
                
                /*---------------------*/
                /* 塞入 Buzzer 資料 */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* 當前已經是頂層，直接退出 */
            else
              {
                /*---------------------*/
                /* 塞入 Buzzer 資料 */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Enter =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ENTER:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            GRSDIDDS gddsBuzzerDatagramDS;
            
            /*-----------------------------------------*/
            /* 取得 Menu Item */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Get(lpControlDS->pCurrentMenuItemQueue, &lpMenuItemDS, GSQ_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 如果不成功 */
            else if(hr != GRSTMIQ_ACCESS_OK)
              {
                /*---------------------*/
                /* 塞入 Buzzer 資料 */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            /*-----------------------------------------*/
            /* 進入選單 */
            else if(lpMenuItemDS->iEnterMode == GRSTMI_TRUNK_MENU_ENTER_MODE_ENTER)
              {
                /*---------------------*/
                /* 檢查進入後的下層選單 */
                if((lpMenuItemDS->iEnterMenuIndex >= 0) && (lpMenuItemDS->iEnterMenuIndex < GRSTMI_TRUNK_MENU_AMOUNT))
                  {
                    /* 切換選單 */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[lpMenuItemDS->iEnterMenuIndex];
                    lpControlDS->iMenuIndex = lpMenuItemDS->iEnterMenuIndex;
                    
                    /* 塞入 Buzzer 資料 */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                        
                        return hr;
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY;
                  }
                /* 發生意外 */
                else
                  {
                    /* 塞入 Buzzer 資料 */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                        
                        return hr;
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
                  }
              }
            /*-----------------------------------------*/
            /* 操作選取 */
            else if(lpMenuItemDS->iEnterMode == GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT)
              {
                /*---------------------*/
                /* 檢查選取後的上層選單 */
                if((lpMenuItemDS->iEscapeMenuIndex >= 0) && (lpMenuItemDS->iEscapeMenuIndex < GRSTMI_TRUNK_MENU_AMOUNT))
                  {
                    /* 複製指標，稍候顯示用 */
                    lpControlDS->lpSelectedMenuTitleName = lpMenuItemDS->strTitleName;
                    
                    /* 保存選擇 */
                    if(lpControlDS->iMenuIndex == GRSTMI_TRUNK_MENU_STATUS)
                      {
                        lpControlDS->iStatusIdentifier = lpMenuItemDS->iEntrainedDataValue;
                      }
                    else if(lpControlDS->iMenuIndex == GRSTMI_TRUNK_MENU_LOCATION)
                      {
                        lpControlDS->iLocationIdentifier = lpMenuItemDS->iEntrainedDataValue;
                      }
                    
                    //GSJ_ADVANCE(lpControlDS->pSystemJournal, "lpSelectedMenuTitleName=[%s]", lpControlDS->lpSelectedMenuTitleName);
                    //GSJ_ADVANCE(lpControlDS->pSystemJournal, "iStatusIdentifier=%d, iLocationIdentifier=%d, iEntrainedDataValue=%d", lpControlDS->iStatusIdentifier, lpControlDS->iLocationIdentifier, lpMenuItemDS->iEntrainedDataValue);
                    
                    /* 切換選單 */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[lpMenuItemDS->iEscapeMenuIndex];
                    lpControlDS->iMenuIndex = lpMenuItemDS->iEscapeMenuIndex;
                    
                    /* 塞入 Buzzer 資料 */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                        
                        return hr;
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iMenuOperationFSMDNextStep = GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY;
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION;
                  }
                /* 發生意外 */
                else
                  {
                    /* 塞入 Buzzer 資料 */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                        
                        return hr;
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
                  }
              }
            /*-----------------------------------------*/
            /* 操作執行 */
            else if(lpMenuItemDS->iEnterMode == GRSTMI_TRUNK_MENU_ENTER_MODE_EXECUTE)
              {
                /* 保存選擇 */
                lpControlDS->iExecutionSelection = lpMenuItemDS->iExecutionSelection;
                
                /* 複製指標，稍候顯示用 */
                lpControlDS->lpSelectedMenuTitleName = lpMenuItemDS->strTitleName;
                
                /* 塞入 Buzzer 資料 */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDNextStep = GRSP_TRUNK_MENU_FSMD_STEP_REVERSION;
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Show Selection =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於清除顯示 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            gdisDisplayDS.iFlags = GRSDISI_DISPLAY_FLAG_CONTROL | GRSDISI_DISPLAY_FLAG_HOMING | GRSDISI_DISPLAY_FLAG_CLEAR;
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於顯示選擇 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, lpControlDS->lpSelectedMenuTitleName);
            
            gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* 第一排置中 */
            gdisDisplayDS.iYAxis = 0;
            
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*---------------------*/
            /* 設定逾時計時器 */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_MENU_SHOW_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* 狀態機步驟切換 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_SHOW_TIME_INTERVAL;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Show Time Interval =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_SHOW_TIME_INTERVAL:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 計時逾時時間 */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iMenuOperationFSMDStep = lpControlDS->iMenuOperationFSMDNextStep;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Quit =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT:
          {
            /* 回應上層邏輯控制，目前運作已經結束 */
            hResult = GGD_FALSE;
            
            /*-----------------------------------------*/
            /* 離開前清理按鍵 */
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            
            /* 狀態步驟返回 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機返回 =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_REVERSION:
          {
            /* 回應上層邏輯控制，目前運作已經完成 */
            hResult = GGD_OK;
            
            /*-----------------------------------------*/
            /* 離開前清理按鍵 */
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            
            /* 狀態步驟返回 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機意外 =- */
        default:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態步驟返回 */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* 檔案結束 */

