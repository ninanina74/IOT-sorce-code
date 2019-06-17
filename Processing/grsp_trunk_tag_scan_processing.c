/*
============================================
 檔案名稱：grsp_trunk_tag_scan_processing.c
 程式功能：RFID Scanner Trunk Tag Scan 處理
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
#include "Scanner/Information/grsi_scanner_information.h"
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之常數定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用常數定義宣告 =- */
/* 退出時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_TAG_SCAN_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT 1200
/* 顯示間隔時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_TAG_SCAN_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT 125

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之巨集定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之宣告型態定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 狀態機步驟常數列舉定義宣告 =- */
/* Trunk Tag Scan 狀態機步驟常數列舉定義宣告 */
enum _GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEFINITION
  {
    /* -= 狀態機預備 =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY,
    
    /* -= 狀態機步驟 =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TITLE_DISPLAY,
    
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_STATUS_QUERY,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_QUERY_COMPLETE,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_ERROR_DISPLAY,
    
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_STARTUP_INSTRUCTION,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_RELEASE,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_ENDLE_INSTRUCTION,
    
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_ERROR_OCCURRED,
    
    /* -= 狀態機返回 =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_REVERSION,
    
    /* -= 狀態機意外 =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之變數定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用運作變數定義宣告 =- */
/* Tag Scan 動畫圖樣列表變數定義宣告 */
static const char *g_lpGRSPTrunkTagScanAnimationPatternList[] =
  {
    /*-----------------------------------------*/
    /* -= 列表 =- */
    "|>     |",
    "| >    |",
    "|  >   |",
    "|   >  |",
    "|    > |",
    "|     >|",
    
    /*-----------------------------------------*/
    /* -= 列表結尾 =- */
    NULL,
  };

/*---------------------------------------------------------------------------*/
/* 函式原型定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 基本公用函數 =- */
/* 狀態機程序 */
GRESULT GRSPTrunkTagScanFSMD(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 回呼程序 =- */
static GRESULT GRSRCALLBACK GRSPTrunkTagScanQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iScannerControlAmount, int iScannerTagAmount, GRESULT hResult);

/*---------------------------------------------------------------------------*/
/* Tag Scan 處理狀態機 */
GRESULT GRSPTrunkTagScanFSMD(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr, hResult;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    if(lpControlDS->iTagScanOperationFSMDStep != lpControlDS->iTagScanOperationFSMDStepRecord)
      {
        GSJ_MESSAGE(lpControlDS->pSystemJournal, "Trunk tag scan FSMD step: %d...", lpControlDS->iTagScanOperationFSMDStep);
        
        lpControlDS->iTagScanOperationFSMDStepRecord = lpControlDS->iTagScanOperationFSMDStep;
      }
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 執行輸入狀態變化偵測狀態機 =- */
    switch(lpControlDS->iTagScanOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機預備 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY:
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
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TITLE_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Copyright Display =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TITLE_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於顯示標題 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "Tag Scan");
            
            gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* 置中 */
            gdisDisplayDS.iYAxis = 0;
            
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /* 狀態機步驟切換 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_STATUS_QUERY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 裝置狀態查詢 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_STATUS_QUERY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSQIQUERYDS gqdsQueryDS;
            
            /*-----------------------------------------*/
            /* Scanner Query Datagram Structure initialize */
            memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
            gqdsQueryDS.iQueryCode = GRSSI_SCANNER_QUERY_CODE_STATUS;
            gqdsQueryDS.lpUserParameter = lpControlDS;
            gqdsQueryDS.OnQueryComplete = GRSPTrunkTagScanQueryCompleteProcessor;
            
            /*-----------------------------------------*/
            /* 加入 Query 資料 */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pScannerQueryQueue)->Add(lpControlDS->pScannerQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Query 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 設置旗標 */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* 狀態機步驟切換 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候裝置狀態查詢完畢 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_QUERY_COMPLETE:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* waiting for resource query complete */
            if(!(lpControlDS->iStatusFlags & GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE))
              {
                ;
              }
            /*-----------------------------------------*/
            /* resource query failed  */
            else if(!(lpControlDS->iStatusFlags & GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED))
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                int i;
                
                /*---------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示狀態 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "Device Error");
                
                gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* 置中 */
                gdisDisplayDS.iYAxis = 1;
                
                /* 塞入 Display 資料 */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* 塞入 Buzzer 資料 */
                for(i = 0; i < 2; i ++)
                  {
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_BEEP;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                        
                        return hr;
                      }
                  }
                
                /*---------------------*/
                /* 設定計時器 */
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_SCAN_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* 狀態機步驟切換 */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_ERROR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* 查詢完成 */
            else
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示狀態 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "|      ||%06d|", lpControlDS->iScannerTagAmount);
                
                gdisDisplayDS.iXAxis = 0;
                gdisDisplayDS.iYAxis = 1;
                
                /* 塞入 Display 資料 */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* 塞入 Buzzer 資料 */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* 準備按鍵 */
                lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
                lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                
                /* 狀態機步驟切換 */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 裝置錯誤顯示 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_ERROR_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 計時時間間隔 */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_ERROR_OCCURRED;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候按鍵 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 檢查按鍵 */
            /* Trigger */
            if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_TRIGGER)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_STARTUP_INSTRUCTION;
              }
            /* Escape */
            else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                
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
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_REVERSION;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 送出 Tag Scan Startup 指令 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_STARTUP_INSTRUCTION:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            GRSIIINSDS gidsInstructionDS;
            
            GSJ_ADVANCE(lpControlDS->pSystemJournal, "送入掃描啟動指令，發動掃描...");
            
            /*-----------------------------------------*/
            /* 塞入 Scan 指令資料 */
            memset(&gidsInstructionDS, 0, sizeof(GRSIIINSDS));
            gidsInstructionDS.iInstruction = GRSII_INSTRUCTION_SCAN_STARTUP;
            if(GGD_FAILED(hr = GRSIQ_GET_METHOD(lpControlDS->pScannerInstructionQueue)->Add(lpControlDS->pScannerInstructionQueue, &gidsInstructionDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSInstructionQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 塞入 Buzzer 資料 */
            memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
            gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
            if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 準備按鍵 */
            lpControlDS->iButtonReleaseFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
            
            /*-----------------------------------------*/
            /* 準備顯示 */
            lpControlDS->iAnimationProgressIndex = 0;
            
            /*-----------------------------------------*/
            /* 設定計時器 */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_SCAN_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* 狀態機步驟切換 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_RELEASE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候按鍵放開 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_RELEASE:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 檢查按鍵 */
            /* Trigger */
            if(lpControlDS->iButtonReleaseFlags & GRSBRI_BUTTON_FLAG_TRIGGER)
              {
                /* 狀態機步驟切換 */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_ENDLE_INSTRUCTION;
              }
            /*-----------------------------------------*/
            /* 計時顯示時間間隔 */
            else if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                const char *lpAnimationPatternPointer = NULL;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示動畫效果 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                lpControlDS->iAnimationProgressIndex %= sizeof(g_lpGRSPTrunkTagScanAnimationPatternList) / sizeof(const char *);
                lpAnimationPatternPointer = g_lpGRSPTrunkTagScanAnimationPatternList[lpControlDS->iAnimationProgressIndex];
                lpControlDS->iAnimationProgressIndex ++;
                
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, lpAnimationPatternPointer);
                
                gdisDisplayDS.iXAxis = 0;
                gdisDisplayDS.iYAxis = 1;
                
                /* 塞入 Display 資料 */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_SCAN_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 送出 Tag Scan Endle 指令 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_ENDLE_INSTRUCTION:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            GRSIIINSDS gidsInstructionDS;
            
            GSJ_ADVANCE(lpControlDS->pSystemJournal, "送入掃描停止指令...");
            
            /*-----------------------------------------*/
            /* 塞入 Scan 指令資料 */
            memset(&gidsInstructionDS, 0, sizeof(GRSIIINSDS));
            gidsInstructionDS.iInstruction = GRSII_INSTRUCTION_SCAN_ENDLE;
            if(GGD_FAILED(hr = GRSIQ_GET_METHOD(lpControlDS->pScannerInstructionQueue)->Add(lpControlDS->pScannerInstructionQueue, &gidsInstructionDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSInstructionQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 塞入 Buzzer 資料 */
            memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
            gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
            if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 準備按鍵 */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            
            /* 狀態機步驟切換 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 發生錯誤 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_ERROR_OCCURRED:
          {
            /* 回應上層邏輯控制，目前運作已經結束 */
            hResult = GGD_FALSE;
            
            /* 狀態步驟返回 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機返回 =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_REVERSION:
          {
            /* 回應上層邏輯控制，目前運作已經完成 */
            hResult = GGD_OK;
            
            /* 狀態步驟返回 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機意外 =- */
        default:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態步驟返回 */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Query 完成處理程序 */
static GRESULT GRSRCALLBACK GRSPTrunkTagScanQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iScannerControlAmount, int iScannerTagAmount, GRESULT hResult)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* 從參數中取回 Message 資訊資料結構指標 */
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* 更新旗標 */
    lpControlDS->iStatusFlags |= GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
    if(hResult == GGD_OK)
      {
        lpControlDS->iStatusFlags |= GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
        
        lpControlDS->iScannerTagAmount = iScannerTagAmount;
        lpControlDS->iScannerControlAmount = iScannerControlAmount;
      }
    else
      {
        lpControlDS->iScannerTagAmount = 0;
        lpControlDS->iScannerControlAmount = 0;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* 檔案結束 */

