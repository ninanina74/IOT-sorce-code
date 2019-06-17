/*
============================================
 檔案名稱：grsp_trunk_tag_commit_processing.c
 程式功能：RFID Scanner Trunk Tag Commit 處理
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
#include "Resource/Information/grsi_resource_information.h"
#include "Scanner/Information/grsi_scanner_information.h"
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之常數定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用常數定義宣告 =- */
/* 顯示錯誤時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_TAG_COMMIT_ERROR_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT 1200
/* 顯示完成時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_TAG_COMMIT_FINISH_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT 750
/* 顯示間隔時間毫秒值常數表宣告定義 */
#define GRSP_TRUNK_TAG_COMMIT_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT 250

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之巨集定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之宣告型態定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 狀態機步驟常數列舉定義宣告 =- */
/* Trunk Tag Scan 狀態機步驟常數列舉定義宣告 */
enum _GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_DEFINITION
  {
    /* -= 狀態機預備 =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY,
    
    /* -= 狀態機步驟 =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_TITLE_DISPLAY,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCANNER_TAG_QUERY,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_TAG_QUERY_COMPLETE,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_COMMIT_COMPLETE,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_PROGRESS_DISPLAY,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_OCCURRED,
    
    /* -= 狀態機返回 =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_REVERSION,
    
    /* -= 狀態機意外 =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之變數定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用運作變數定義宣告 =- */
/* Tag Scan 動畫圖樣列表變數定義宣告 */
static const char *g_lpGRSPTrunkTagCommitAnimationPatternList[] =
  {
    /*-----------------------------------------*/
    /* -= 列表 =- */
    "|     <|",
    "|    < |",
    "|   <  |",
    "|  <   |",
    "| <    |",
    "|<     |",
    
    /*-----------------------------------------*/
    /* -= 列表結尾 =- */
    NULL,
  };

/*---------------------------------------------------------------------------*/
/* 函式原型定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 基本公用函數 =- */
/* 狀態機程序 */
GRESULT GRSPTrunkTagCommitFSMD(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 回呼程序 =- */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength);
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult);
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult);

/*---------------------------------------------------------------------------*/
/* Tag Commit 處理狀態機 */
GRESULT GRSPTrunkTagCommitFSMD(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr, hResult;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    if(lpControlDS->iTagCommitOperationFSMDStep != lpControlDS->iTagCommitOperationFSMDStepRecord)
      {
        GSJ_MESSAGE(lpControlDS->pSystemJournal, "Trunk tag commit FSMD step: %d...", lpControlDS->iTagCommitOperationFSMDStep);
        
        lpControlDS->iTagCommitOperationFSMDStepRecord = lpControlDS->iTagCommitOperationFSMDStep;
      }
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 執行輸入狀態變化偵測狀態機 =- */
    switch(lpControlDS->iTagCommitOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機預備 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY:
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
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_TITLE_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Copyright Display =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_TITLE_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* 載裝 Display 資料用於顯示標題 */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "Tag Commit");
            
            gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* 置中 */
            gdisDisplayDS.iYAxis = 0;
            
            /* 塞入 Display 資料 */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Display 佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /* 狀態機步驟切換 */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCANNER_TAG_QUERY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Scanner Tag Query =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCANNER_TAG_QUERY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSQIQUERYDS gqdsQueryDS;
            
            /*-----------------------------------------*/
            /* Scanner Query Datagram Structure initialize */
            memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
            gqdsQueryDS.iQueryCode = GRSSI_SCANNER_QUERY_CODE_TAG;
            gqdsQueryDS.lpUserParameter = lpControlDS;
            gqdsQueryDS.OnQueryResult = GRSPTrunkTagCommitScannerQueryResultProcessor;
            gqdsQueryDS.OnQueryComplete = GRSPTrunkTagCommitScannerQueryCompleteProcessor;
            
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
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_TAG_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候 Scanner Tag 查詢完畢 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_TAG_QUERY_COMPLETE:
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
                int i, iLengthRemainders;
                
                /*---------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示狀態 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Query Error");
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Query Error", iLengthRemainders, "                ");
                
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
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_ERROR_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* 狀態機步驟切換 */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* 查詢完成 */
            else
              {
                int iResultAmount, iLengthRemainders;
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* 檢查 Result 數量 */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /*---------------------*/
                    /* 載裝 Display 資料用於顯示狀態 */
                    memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                    
                    iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Have No Tag");
                    gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Have No Tag", iLengthRemainders, "                ");
                    
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
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                        
                        return hr;
                      }
                    
                    /*---------------------*/
                    /* 設定計時器 */
                    lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_FINISH_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                    lpControlDS->lMillisecondValue = GSTTimeGetTime();
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY;
                  }
                else
                  {
                    /*---------------------*/
                    /* 載裝 Display 資料用於顯示狀態 */
                    memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                    
                    gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "|      ||%06d|", iResultAmount);
                    
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
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Scan Record Commit =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTIIDS *lpForFoundItemDS = NULL;
            void *lpResultDatagramDS = NULL;
            
            /*-----------------------------------------*/
            /* 取出 Result 資料 */
            if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Result 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            else if(hr != GRSRQ_ACCESS_OK)
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                int iLengthRemainders;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示狀態 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Completed");
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Completed", iLengthRemainders, "                ");
                
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
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* 設定計時器 */
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_FINISH_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* 狀態機步驟切換 */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY;
              }
            /*-----------------------------------------*/
            /* 搜查 Item Tree，指定的 Tag 是否存在 */
            else if(GGD_FAILED(hr = GRSTIT_GET_METHOD(lpControlDS->pItemTree)->Find(lpControlDS->pItemTree, &lpForFoundItemDS, ((GRSSITAGDS *)lpResultDatagramDS)->strTagIdentifier, GAVLT_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Item Tree 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            /* 不存在的 Tag，直接丟棄 */
            else if(hr != GRSTIT_ACCESS_OK)
              {
                GGD_MEMORY_RELEASE(lpResultDatagramDS);
              }
            else
              {
                GRSQIQUERYDS gqdsQueryDS;
                
                /*---------------------*/
                /* Resource Query Datagram Structure initialize */
                memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
                gqdsQueryDS.iQueryCode = GRSRI_RESOURCE_QUERY_CODE_SCANRECORD_ADD_RECORD;
                gqdsQueryDS.lpUserParameter = lpControlDS;
                gqdsQueryDS.OnQueryComplete = GRSPTrunkTagCommitResourceQueryCompleteProcessor;
                gqdsQueryDS.iInputDatagramDSLength = sizeof(GRSRISRECORDDS);
                if((gqdsQueryDS.lpInputDatagramDS = GGD_MEMORY_ALLOCATE(gqdsQueryDS.iInputDatagramDSLength)) == NULL)
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "記憶體空間配置時，記憶體不足！錯誤碼：0x%08X。\n", GGD_ERROR_OUT_OF_MEMORY);
                    
                    return GGD_ERROR_OUT_OF_MEMORY;
                  }
                
                /*---------------------*/
                /* 載裝基本項目 */
                memset(gqdsQueryDS.lpInputDatagramDS, 0, gqdsQueryDS.iInputDatagramDSLength);
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iDeviceIdentifier = 1;
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iItemIdentifier = lpForFoundItemDS->iIdentifier;
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iStatusIdentifier = lpControlDS->iStatusIdentifier;
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iLocationIdentifier = lpControlDS->iLocationIdentifier;
                
                /*---------------------*/
                /* 加入 Query 資料 */
                if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Query 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* 設置旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* Result 資料刪除，後面用不到了 */
                GGD_MEMORY_RELEASE(lpResultDatagramDS);
                
                /* 狀態機步驟切換 */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_COMMIT_COMPLETE;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候 Scan Record Commit 完畢 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_COMMIT_COMPLETE:
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
                int i, iLengthRemainders;
                
                GSJ_WARNING(lpControlDS->pSystemJournal, "資源：Scan Record 寫入操作處理，回呼回傳是：Resource query failed ！\n");
                
                /*---------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示狀態 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Commit Error");
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Commit Error", iLengthRemainders, "                ");
                
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
                /* 清空 Result 佇列 */
                GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Clear(lpControlDS->pQueryResultQueue);
                
                /*---------------------*/
                /* 設定計時器 */
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_ERROR_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* 狀態機步驟切換 */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* 操作完成 */
            else
              {
                const char *lpAnimationPatternPointer = NULL;
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* 載裝 Display 資料用於顯示動畫效果 */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                lpControlDS->iAnimationProgressIndex %= sizeof(g_lpGRSPTrunkTagCommitAnimationPatternList) / sizeof(const char *);
                lpAnimationPatternPointer = g_lpGRSPTrunkTagCommitAnimationPatternList[lpControlDS->iAnimationProgressIndex];
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
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 顯示完成 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 計時時間間隔 */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_REVERSION;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 顯示錯誤 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* 計時時間間隔 */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* 狀態機步驟切換 */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_OCCURRED;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 發生錯誤 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_OCCURRED:
          {
            /* 回應上層邏輯控制，目前運作已經結束 */
            hResult = GGD_FALSE;
            
            /* 狀態步驟返回 */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機返回 =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_REVERSION:
          {
            /* 回應上層邏輯控制，目前運作已經完成 */
            hResult = GGD_OK;
            
            /* 狀態步驟返回 */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機意外 =- */
        default:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態步驟返回 */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Scanner Query 結果處理程序 */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength)
  {
    void *lpResultDatagramDSPointer = NULL;
    
    GRSTRUNKCDS *lpControlDS = NULL;
    GRESULT hr;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* 從參數中取回 Message 資訊資料結構指標 */
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* 為 Result 配置記憶體空間 */
    if((lpResultDatagramDSPointer = GGD_MEMORY_ALLOCATE(iResultDatagramDSLength)) == NULL)
      {
        return GGD_ERROR_OUT_OF_MEMORY;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* 存入 Result 資料 */
    memcpy(lpResultDatagramDSPointer, lpResultDatagramDS, iResultDatagramDSLength);
    if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Add(lpControlDS->pQueryResultQueue, lpResultDatagramDSPointer)))
      {
        return hr;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Scanner Query 完成處理程序 */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult)
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
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Resource Query 完成處理程序 */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult)
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
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* 檔案結束 */

