/*
============================================
 檔案名稱：grsp_trunk_processing.h
 程式功能：RFID Scanner Trunk 處理模組
 設計者：羅富元 (5107056012)
 版權擁有者：Seamus Berloz (軒摩斯貝爾羅)
 最後更新日期：2019/06/09 上午 11:32:29
 其他資訊：
============================================
*/
/*---------------------------------------------------------------------------*/
/* 標頭定義宣告 */
#ifndef _GRSP_TRUNK_PROCESSING_H
#define _GRSP_TRUNK_PROCESSING_H

/*---------------------------------------------------------------------------*/
/* 包含檔定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 自定義包含檔定義宣告 =- */
#include "Trunk/Queue/grsq_trunk_menu_item_queue.h"
#include "Trunk/Tree/grst_trunk_item_tree.h"
#include "Common/Queue/grsq_result_queue.h"
#include "include/grtc_real_time_clock.h"
#include "Trunk/grs_trunk.h"

/*---------------------------------------------------------------------------*/
/* 全域常數定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用全域常數定義 =- */

/*---------------------------------------------------------------------------*/
/* 全域巨集定義宣告 */

/*---------------------------------------------------------------------------*/
/* 全域資料結構型態列舉定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用資料結構定義宣告 =- */
/* Trunk 控制組資料結構定義宣告 */
struct _GRSTP_TRUNK_CONTROL_DATAGRAM_STRUCTURE
  {
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 資料數據組 =- */
    /* 狀態旗標 */
    int iStatusFlags;
    
    /* Button */
    int iButtonPressFlags;
    int iButtonReleaseFlags;
    
    /* Display */
    int iAnimationProgressIndex;
    
    /* Scanner */
    int iScannerTagAmount;
    int iScannerControlAmount;
    
    /* 計時器 */
    long lMillisecondValue;
    long lMillisecondValueLimit;
    
    /* Menu */
    int iMenuIndex;
    int iStatusIdentifier;
    int iLocationIdentifier;
    int iExecutionSelection;
    char *lpSelectedMenuTitleName;
    
    /*-----------------------------------------*/
    /* 狀態機步驟 */
    int iOperationFSMDStep;
    int iMenuOperationFSMDStep;
    int iStatusOperationFSMDStep;
    int iTagScanOperationFSMDStep;
    int iDatetimeOperationFSMDStep;
    int iMenuOperationFSMDNextStep;
    int iTagCommitOperationFSMDStep;
    int iLoadResourceOperationFSMDStep;
    
    #ifdef GRSP_TRUNK_DUMP_FSMD_STEP_ENABLED
    
    int iOperationFSMDStepRecord;
    int iMenuOperationFSMDStepRecord;
    int iStatusOperationFSMDStepRecord;
    int iTagScanOperationFSMDStepRecord;
    int iDatetimeOperationFSMDStepRecord;
    int iTagCommitOperationFSMDStepRecord;
    int iLoadResourceOperationFSMDStepRecord;
    
    #endif
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= 控制物件組 =- */
    /*-----------------------------------------*/
    /* 主要控制物件 */
    /*--------------------*/
    /* 外部物件 */
    /* 日誌 */
    GSystemJournal *pSystemJournal; /* 系統日誌記錄器物件 */
    /* 資源 */
    GRSQueryQueue *pResourceQueryQueue; /* Resource 查詢佇列物件 */
    /* 人機 */
    GRSDisplayQueue *pDisplayQueue; /* Display 佇列物件 */
    GRSDatagramQueue *pBuzzerDatagramQueue; /* Buzzer 資料包佇列物件 */
    GRSButtonRegisterQueue *pButtonRegisterQueue; /* Button Register 佇列物件 */
    /* Scanner */
    GRSQueryQueue *pScannerQueryQueue; /* Scanner 查詢佇列物件 */
    GRSInstructionQueue *pScannerInstructionQueue; /* Scanner 指令佇列物件 */
    
    /*--------------------*/
    /* 內部物件 */
    /* RTC */
    GRealTimeClock *pRealTimeClock; /* 即時時鐘物件 */
    /* Query */
    GRSTItemTree *pItemTree; /* Item Tree 物件 */
    GRSResultQueue *pQueryResultQueue; /* Query Result 佇列物件 */
    /* Menu */
    GRSTMenuItemQueue *pCurrentMenuItemQueue;
    GRSTMenuItemQueue *pMenuItemQueue[GRSTMI_TRUNK_MENU_AMOUNT];
  };

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用旗標欄位常數列舉資料型態宣告定義 =- */
/* Trunk 狀態旗標欄位常數列舉定義宣告 */
enum _GRSTP_TRUNK_STATUS_FLAGS_FIELD_DEFINITION
  {
    /* -= 狀態旗標欄位列表 =- */
    GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_COMPLETE,
    GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_SUCCEEDED,
    
    /* -= 狀態旗標欄位總數 =- */
    GRSTP_TRUNK_STATUS_FLAGS_FIELD_AMOUNT,
  };

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 常用旗標常數列舉資料型態宣告定義 =- */
/* Trunk 狀態旗標常數列舉定義宣告 */
enum _GRSTP_TRUNK_STATUS_FLAGS_DEFINITION
  {
    /* -= 狀態旗標列表 =- */
    GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE = GGD_GET_FLAG_MASK(GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_COMPLETE),
    GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED = GGD_GET_FLAG_MASK(GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_SUCCEEDED),
    
  };

/*---------------------------------------------------------------------------*/
/* 公用變數定義宣告 */

/*---------------------------------------------------------------------------*/
/* 公用程序定義宣告 */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= 公用程序 =- */
/* 狀態機程序 */
extern GRESULT GRSPTrunkMenuFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkStatusFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkDatetimeFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkTagScanFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkTagCommitFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkLoadResourceFSMD(GRSTRUNKCDS *lpControlDS);


/*---------------------------------------------------------------------------*/
/* 標頭定義宣告結束 */
#endif

/*---------------------------------------------------------------------------*/
/* 檔案結束 */

