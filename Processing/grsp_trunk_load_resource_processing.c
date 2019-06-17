/*
============================================
 檔案名稱：grsp_trunk_load_resource_processing.c
 程式功能：RFID Scanner Trunk Load Resource 處理
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
#include "Resource/Information/grsi_resource_information.h"
#include "Common/Information/grsi_buzzer_information.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之常數定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之巨集定義 */

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之宣告型態定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 狀態機步驟常數列舉定義宣告 =- */
/* Trunk Status 狀態機步驟常數列舉定義宣告 */
enum _GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_DEFINITION
  {
    /* -= 狀態機預備 =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY,
    
    /* -= 狀態機步驟 =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ITEM_QUERY,
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_ITEM_QUERY_COMPLETE,
    
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY,
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_STATUS_QUERY_COMPLETE,
    
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_LOCATION_QUERY,
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_LOCATION_QUERY_COMPLETE,
    
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED,
    
    /* -= 狀態機返回 =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_REVERSION,
    
    /* -= 狀態機意外 =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* 僅供本模組專用之變數定義 */

/*---------------------------------------------------------------------------*/
/* 函式原型定義 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 基本公用函數 =- */
/* 狀態機程序 */
GRESULT GRSPTrunkLoadResourceFSMD(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 回呼程序 =- */
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength);
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult);

/*---------------------------------------------------------------------------*/
/* Status 處理狀態機 */
GRESULT GRSPTrunkLoadResourceFSMD(GRSTRUNKCDS *lpControlDS)
  {
    GRESULT hr, hResult;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    if(lpControlDS->iLoadResourceOperationFSMDStep != lpControlDS->iLoadResourceOperationFSMDStepRecord)
      {
        GSJ_MESSAGE(lpControlDS->pSystemJournal, "Trunk load resource FSMD step: %d...", lpControlDS->iLoadResourceOperationFSMDStep);
        
        lpControlDS->iLoadResourceOperationFSMDStepRecord = lpControlDS->iLoadResourceOperationFSMDStep;
      }
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 執行輸入狀態變化偵測狀態機 =- */
    switch(lpControlDS->iLoadResourceOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機預備 =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態機步驟切換 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ITEM_QUERY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Item Query =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ITEM_QUERY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSQIQUERYDS gqdsQueryDS;
            
            /*-----------------------------------------*/
            /* Resource Query Datagram Structure initialize */
            memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
            gqdsQueryDS.iQueryCode = GRSRI_RESOURCE_QUERY_CODE_ITEM_GET_ALL_ITEM;
            gqdsQueryDS.lpUserParameter = lpControlDS;
            gqdsQueryDS.OnQueryResult = GRSPTrunkLoadResourceQueryResultProcessor;
            gqdsQueryDS.OnQueryComplete = GRSPTrunkLoadResourceQueryCompleteProcessor;
            
            /*-----------------------------------------*/
            /* 加入 Query 資料 */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Query 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 設置旗標 */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* 狀態機步驟切換 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_ITEM_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候 Item Query 完成 =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_ITEM_QUERY_COMPLETE:
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
                GSJ_WARNING(lpControlDS->pSystemJournal, "資源：Status 查詢操作處理，回呼回傳是：Resource query failed ！\n");
                
                /* 旗標設置 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /* 狀態機步驟切換 */
                lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
              }
            /*-----------------------------------------*/
            /* 操作完成 */
            else
              {
                int iResultAmount = 0;
                
                /*-----------------------------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*-----------------------------------------*/
                /* 檢查 Result 數量 */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /* 狀態機步驟切換 */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY;
                  }
                /*-----------------------------------------*/
                /* Result 處理 */
                else
                  {
                    /*-----------------------------------------*/
                    /* 迴圈處理所有 Result */
                    while(TRUE)
                      {
                        GRSTIIDS *lpForFoundItemDS = NULL;
                        GRSTIIDS *lpItemDS = NULL;
                        
                        void *lpResultDatagramDS = NULL;
                        
                        /*--------------------*/
                        /* 取出 Result 資料 */
                        if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Result 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        else if(hr != GRSRQ_ACCESS_OK)
                          {
                            break;
                          }
                        
                        //GSJ_ADVANCE(lpControlDS->pSystemJournal, "Item(%d)=[%s]", ((GRSRIITEMDS *)lpResultDatagramDS)->iIdentifier, ((GRSRIITEMDS *)lpResultDatagramDS)->strTagIdentifier);
                        
                        /*--------------------*/
                        /* 重覆檢查 */
                        if(GGD_FAILED(hr = GRSTIT_GET_METHOD(lpControlDS->pItemTree)->Find(lpControlDS->pItemTree, &lpForFoundItemDS, ((GRSRIITEMDS *)lpResultDatagramDS)->strTagIdentifier, GAVLT_ACCESS_METHOD_COPY)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Trunk Item Tree 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        /* 資料已經存在 */
                        else if(hr == GRSTIT_ACCESS_OK)
                          {
                            /* Result 資料刪除，後面用不到了 */
                            GGD_MEMORY_RELEASE(lpResultDatagramDS);
                            
                            continue;
                          }
                        
                        /*--------------------*/
                        /* Item 記憶體空間配置 */
                        if((lpItemDS = (GRSTIIDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTIIDS))) == NULL)
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Item 記憶體空間配置時，記憶體不足！錯誤碼：0x%08X。\n", GGD_ERROR_OUT_OF_MEMORY);
                            
                            return GGD_ERROR_OUT_OF_MEMORY;
                          }
                        
                        /*--------------------*/
                        /* 載裝 */
                        memset(lpItemDS, 0, sizeof(GRSTIIDS));
                        lpItemDS->iIdentifier = ((GRSRIITEMDS *)lpResultDatagramDS)->iIdentifier;
                        snprintf(lpItemDS->strTagIdentifier, GRSTII_TRUNK_ITEM_TAG_IDENTIFIER_STRING_LENGTH_MAXIMUM, ((GRSRIITEMDS *)lpResultDatagramDS)->strTagIdentifier);
                        
                        /*--------------------*/
                        /* 保存 */
                        if(GGD_FAILED(hr = GRSTIT_GET_METHOD(lpControlDS->pItemTree)->Add(lpControlDS->pItemTree, lpItemDS)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Trunk Item Tree 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        
                        /*--------------------*/
                        /* Result 資料刪除，後面用不到了 */
                        GGD_MEMORY_RELEASE(lpResultDatagramDS);
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Status Query =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSQIQUERYDS gqdsQueryDS;
            
            /*-----------------------------------------*/
            /* Resource Query Datagram Structure initialize */
            memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
            gqdsQueryDS.iQueryCode = GRSRI_RESOURCE_QUERY_CODE_STATUS_GET_ALL_STATUS;
            gqdsQueryDS.lpUserParameter = lpControlDS;
            gqdsQueryDS.OnQueryResult = GRSPTrunkLoadResourceQueryResultProcessor;
            gqdsQueryDS.OnQueryComplete = GRSPTrunkLoadResourceQueryCompleteProcessor;
            
            /*-----------------------------------------*/
            /* 加入 Query 資料 */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Query 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 設置旗標 */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* 狀態機步驟切換 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_STATUS_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候 Status Query 完成 =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_STATUS_QUERY_COMPLETE:
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
                GSJ_WARNING(lpControlDS->pSystemJournal, "資源：Status 查詢操作處理，回呼回傳是：Resource query failed ！\n");
                
                /* 旗標設置 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /* 狀態機步驟切換 */
                lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
              }
            /*-----------------------------------------*/
            /* 操作完成 */
            else
              {
                int iResultAmount = 0;
                
                /*-----------------------------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*-----------------------------------------*/
                /* 檢查 Result 數量 */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /* 狀態機步驟切換 */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
                  }
                /*-----------------------------------------*/
                /* Result 處理 */
                else
                  {
                    int iResultIndex;
                    
                    /*-----------------------------------------*/
                    /* Status 定址選單 */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[GRSTMI_TRUNK_MENU_STATUS];
                    
                    /*-----------------------------------------*/
                    /* 預先清除原始列表 */
                    GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Clear(lpControlDS->pCurrentMenuItemQueue);
                    
                    /*-----------------------------------------*/
                    /* 迴圈處理所有 Result */
                    iResultIndex = 0;
                    while(TRUE)
                      {
                        GRSTMIITEMDS *lpMenuItemDS = NULL;
                        void *lpResultDatagramDS = NULL;
                        
                        /*--------------------*/
                        /* 取出 Result 資料 */
                        if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Result 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        else if(hr != GRSRQ_ACCESS_OK)
                          {
                            break;
                          }
                        
                        //GSJ_ADVANCE(lpControlDS->pSystemJournal, "Status(%d)=[%s]", ((GRSRISTATUSDS *)lpResultDatagramDS)->iIdentifier, ((GRSRISTATUSDS *)lpResultDatagramDS)->strCode);
                        
                        /*--------------------*/
                        /* Menu Item 記憶體空間配置 */
                        if((lpMenuItemDS = (GRSTMIITEMDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTMIITEMDS))) == NULL)
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Menu Item 記憶體空間配置時，記憶體不足！錯誤碼：0x%08X。\n", GGD_ERROR_OUT_OF_MEMORY);
                            
                            return GGD_ERROR_OUT_OF_MEMORY;
                          }
                        
                        /*--------------------*/
                        /* 載裝 */
                        memset(lpMenuItemDS, 0, sizeof(GRSTMIITEMDS));
                        lpMenuItemDS->iIndex = iResultIndex;
                        lpMenuItemDS->iEnterMenuIndex = (-1);
                        lpMenuItemDS->iEscapeMenuIndex = GRSTMI_TRUNK_MENU_MAIN;
                        lpMenuItemDS->iEnterMode = GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT;
                        lpMenuItemDS->iEntrainedDataValue = ((GRSRISTATUSDS *)lpResultDatagramDS)->iIdentifier;
                        snprintf(lpMenuItemDS->strTitleName, GRSTMI_TRUNK_MENU_ITEM_TITLE_NAME_LENGTH_MAXIMUM, ((GRSRISTATUSDS *)lpResultDatagramDS)->strCode);
                        lpMenuItemDS->iDisplayLine = 1;
                        
                        /*--------------------*/
                        /* 保存 */
                        if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Add(lpControlDS->pCurrentMenuItemQueue, lpMenuItemDS)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        
                        /*--------------------*/
                        /* Result 資料刪除，後面用不到了 */
                        GGD_MEMORY_RELEASE(lpResultDatagramDS);
                        
                        /*--------------------*/
                        /* Result 索引累計 */
                        iResultIndex ++;
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_LOCATION_QUERY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Location Query =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_LOCATION_QUERY:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            GRSQIQUERYDS gqdsQueryDS;
            
            /*-----------------------------------------*/
            /* Resource Query Datagram Structure initialize */
            memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
            gqdsQueryDS.iQueryCode = GRSRI_RESOURCE_QUERY_CODE_LOCATION_GET_ALL_LOCATION;
            gqdsQueryDS.lpUserParameter = lpControlDS;
            gqdsQueryDS.OnQueryResult = GRSPTrunkLoadResourceQueryResultProcessor;
            gqdsQueryDS.OnQueryComplete = GRSPTrunkLoadResourceQueryCompleteProcessor;
            
            /*-----------------------------------------*/
            /* 加入 Query 資料 */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Query 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* 設置旗標 */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* 狀態機步驟切換 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_LOCATION_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 等候 Loation Query 完成 =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_LOCATION_QUERY_COMPLETE:
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
                GSJ_WARNING(lpControlDS->pSystemJournal, "資源：Location 查詢操作處理，回呼回傳是：Resource query failed ！\n");
                
                /* 旗標設置 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /* 狀態機步驟切換 */
                lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
              }
            /*-----------------------------------------*/
            /* 操作完成 */
            else
              {
                int iResultAmount = 0;
                
                /*-----------------------------------------*/
                /* 清除旗標 */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*-----------------------------------------*/
                /* 檢查 Result 數量 */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /* 狀態機步驟切換 */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
                  }
                /*-----------------------------------------*/
                /* Result 處理 */
                else
                  {
                    int iResultIndex;
                    
                    /*-----------------------------------------*/
                    /* Location 定址選單 */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[GRSTMI_TRUNK_MENU_LOCATION];
                    
                    /*-----------------------------------------*/
                    /* 預先清除原始列表 */
                    GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Clear(lpControlDS->pCurrentMenuItemQueue);
                    
                    /*-----------------------------------------*/
                    /* 迴圈處理所有 Result */
                    iResultIndex = 0;
                    while(TRUE)
                      {
                        GRSTMIITEMDS *lpMenuItemDS = NULL;
                        void *lpResultDatagramDS = NULL;
                        
                        /*--------------------*/
                        /* 取出 Result 資料 */
                        if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Result 串列佇列物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        else if(hr != GRSRQ_ACCESS_OK)
                          {
                            break;
                          }
                        
                        //GSJ_ADVANCE(lpControlDS->pSystemJournal, "Location(%d)=[%s]", ((GRSRILOCATIONDS *)lpResultDatagramDS)->iIdentifier, ((GRSRILOCATIONDS *)lpResultDatagramDS)->strCode);
                        
                        /*--------------------*/
                        /* Menu Item 記憶體空間配置 */
                        if((lpMenuItemDS = (GRSTMIITEMDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTMIITEMDS))) == NULL)
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 Menu Item 記憶體空間配置時，記憶體不足！錯誤碼：0x%08X。\n", GGD_ERROR_OUT_OF_MEMORY);
                            
                            return GGD_ERROR_OUT_OF_MEMORY;
                          }
                        
                        /*--------------------*/
                        /* 載裝 */
                        memset(lpMenuItemDS, 0, sizeof(GRSTMIITEMDS));
                        lpMenuItemDS->iIndex = iResultIndex;
                        lpMenuItemDS->iEnterMenuIndex = (-1);
                        lpMenuItemDS->iEscapeMenuIndex = GRSTMI_TRUNK_MENU_MAIN;
                        lpMenuItemDS->iEnterMode = GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT;
                        lpMenuItemDS->iEntrainedDataValue = ((GRSRILOCATIONDS *)lpResultDatagramDS)->iIdentifier;
                        snprintf(lpMenuItemDS->strTitleName, GRSTMI_TRUNK_MENU_ITEM_TITLE_NAME_LENGTH_MAXIMUM, ((GRSRILOCATIONDS *)lpResultDatagramDS)->strCode);
                        lpMenuItemDS->iDisplayLine = 1;
                        
                        /*--------------------*/
                        /* 保存 */
                        if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Add(lpControlDS->pCurrentMenuItemQueue, lpMenuItemDS)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSTMenuItemQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                            
                            return hr;
                          }
                        
                        /*--------------------*/
                        /* Result 資料刪除，後面用不到了 */
                        GGD_MEMORY_RELEASE(lpResultDatagramDS);
                        
                        /*--------------------*/
                        /* Result 索引累計 */
                        iResultIndex ++;
                      }
                    
                    /* 狀態機步驟切換 */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_REVERSION;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 錯誤發生 =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED:
          {
            /* 回應上層邏輯控制，目前運作發生錯誤 */
            hResult = GGD_FALSE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            int i;
            
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
            
            /* 狀態步驟返回 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機返回 =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_REVERSION:
          {
            /* 回應上層邏輯控制，目前運作已經完成 */
            hResult = GGD_OK;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            int i;
            
            /*---------------------*/
            /* 塞入 Buzzer 資料 */
            for(i = 0; i < 2; i ++)
              {
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "操作 GRSDatagramQueue 物件時，發生錯誤！錯誤碼：0x%08X。\n", hr);
                    
                    return hr;
                  }
              }
            
            /* 狀態步驟返回 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= 狀態機意外 =- */
        default:
          {
            /* 回應上層邏輯控制，目前運作尚未完成 */
            hResult = GGD_STILL_ACTIVE;
            
            /* 狀態步驟返回 */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Query 結果處理程序 */
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength)
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
/* Query 完成處理程序 */
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult)
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

