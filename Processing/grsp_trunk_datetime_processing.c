/*
============================================
 檔案名稱：grsp_trunk_datetime_processing.c
 程式功能：RFID Scanner Trunk Datatime 處理
 設計者：羅富元 (5107056012)
 版權擁有者：Seamus Berloz (軒摩斯貝爾羅)
 最後更新日期：2019/06/08 - 11:32:29
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
/* 時間刷新時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_DATETIME_REFRESH_INTERVAL_MILLISECOND_VALUE_LIMIT 500

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之巨集定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之宣告型態定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 狀態機步驟常數列舉定義宣告 =- */
/* Trunk Datetime 狀態機步驟常數列舉定義宣告 */
enum _GRSP_TRUNK_DATETIME_FSMD_STEP_DEFINITION
  {
    /* -= 狀態機預備 =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_READY,
    
    /* -= 狀態機步驟 =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY,
    GRSP_TRUNK_DATETIME_FSMD_STEP_WAIT_TIME_INTERVAL,
    
    /* -= 狀態機返回 =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_REVERSION,
    
    /* -= 狀態機意外 =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之變數定義 */

/*---------------------------------------------------------------------------*/
/* 函式原型定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 基本公用函數 =- */
/* 狀態機程序 */
GRESULT GRSPTrunkDatetimeFSMD(GRSTRUNKCDS *lpControlDS);

/*---------------------------------------------------------------------------*/
/* Datetime 處理狀態機 */
GRESULT GRSPTrunkDatetimeFSMD(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr, hResult;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    if(lpControlDS->iDatetimeOperationFSMDStep != lpControlDS->iDatetimeOperationFSMDStepRecord)
      {
        GSJ_MESSAGE(lpControlDS->pSystemJournal, "Trunk datetime FSMD step: %d...", lpControlDS->iDatetimeOperationFSMDStep);
        
        lpControlDS->iDatetimeOperationFSMDStepRecord = lpControlDS->iDatetimeOperationFSMDStep;
      }
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 執行輸入狀態變化偵測狀態機 =- */
    switch(lpControlDS->iDatetimeOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機預備 =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_READY:
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
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 顯示日期時間 =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRTCTIME *lpLocalTime = NULL;
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* 取得本地時間 */
            if((lpLocalTime = GRTC_GET_METHOD(lpControlDS->pRealTimeClock)->GetLocalTime(lpControlDS->pRealTimeClock, NULL)) == NULL)
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "進行取得本地時間時，發生錯誤！錯誤碼：0x%08X。\n", GGD_ERROR_UNEXPECTED);
                
                return GGD_ERROR_UNEXPECTED;
              }
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於顯示日期 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%04d-%02d-%02d",
                lpLocalTime->iYears + GRTC_TIME_SINCE_YEARS, lpLocalTime->iMonths + 1, lpLocalTime->iMDay);
            
            gdisDisplayDS.iXAxis = 0;
            gdisDisplayDS.iYAxis = 0;
            
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於顯示時間 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "-= %02d:%02d:%02d =-",
                lpLocalTime->iHours, lpLocalTime->iMinutes, lpLocalTime->iSecond);
            
            gdisDisplayDS.iXAxis = 1;
            gdisDisplayDS.iYAxis = 1;
            
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 準備退出按鍵 */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            
            /*-----------------------------------------*/
            /* 設定計時器 */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_DATETIME_REFRESH_INTERVAL_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* 狀態機步驟切換 */
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_WAIT_TIME_INTERVAL;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候時間間隔 =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_WAIT_TIME_INTERVAL:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 檢查退出按鍵 */
            if((lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT) || (lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE))
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                
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
                lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_REVERSION;
              }
            /*-----------------------------------------*/
            /* 計時時間間隔 */
            else if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機返回 =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_REVERSION:
          {
            /* 回應上層邏輯控制，目前運作已經完成 */
            hResult = GGD_OK;
            
            /* 狀態步驟返回 */
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機意外 =- */
        default:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態步驟返回 */
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* 檔案結束 */

