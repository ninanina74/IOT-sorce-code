/*
============================================
 �ɮצW�١Ggrsp_trunk_processing.h
 �{���\��GRFID Scanner Trunk �B�z�Ҳ�
 �]�p�̡Gù�I�� (5107056012)
 ���v�֦��̡GSeamus Berloz (�a��������ù)
 �̫��s����G2019/06/09 �W�� 11:32:29
 ��L��T�G
============================================
*/
/*---------------------------------------------------------------------------*/
/* ���Y�w�q�ŧi */
#ifndef _GRSP_TRUNK_PROCESSING_H
#define _GRSP_TRUNK_PROCESSING_H

/*---------------------------------------------------------------------------*/
/* �]�t�ɩw�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �۩w�q�]�t�ɩw�q�ŧi =- */
#include "Trunk/Queue/grsq_trunk_menu_item_queue.h"
#include "Trunk/Tree/grst_trunk_item_tree.h"
#include "Common/Queue/grsq_result_queue.h"
#include "include/grtc_real_time_clock.h"
#include "Trunk/grs_trunk.h"

/*---------------------------------------------------------------------------*/
/* ����`�Ʃw�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�Υ���`�Ʃw�q =- */

/*---------------------------------------------------------------------------*/
/* ���쥨���w�q�ŧi */

/*---------------------------------------------------------------------------*/
/* �����Ƶ��c���A�C�|�w�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�θ�Ƶ��c�w�q�ŧi =- */
/* Trunk ����ո�Ƶ��c�w�q�ŧi */
struct _GRSTP_TRUNK_CONTROL_DATAGRAM_STRUCTURE
  {
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* -= ��Ƽƾڲ� =- */
    /* ���A�X�� */
    int iStatusFlags;
    
    /* Button */
    int iButtonPressFlags;
    int iButtonReleaseFlags;
    
    /* Display */
    int iAnimationProgressIndex;
    
    /* Scanner */
    int iScannerTagAmount;
    int iScannerControlAmount;
    
    /* �p�ɾ� */
    long lMillisecondValue;
    long lMillisecondValueLimit;
    
    /* Menu */
    int iMenuIndex;
    int iStatusIdentifier;
    int iLocationIdentifier;
    int iExecutionSelection;
    char *lpSelectedMenuTitleName;
    
    /*-----------------------------------------*/
    /* ���A���B�J */
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
    /* -= ������ =- */
    /*-----------------------------------------*/
    /* �D�n����� */
    /*--------------------*/
    /* �~������ */
    /* ��x */
    GSystemJournal *pSystemJournal; /* �t�Τ�x�O�������� */
    /* �귽 */
    GRSQueryQueue *pResourceQueryQueue; /* Resource �d�ߦ�C���� */
    /* �H�� */
    GRSDisplayQueue *pDisplayQueue; /* Display ��C���� */
    GRSDatagramQueue *pBuzzerDatagramQueue; /* Buzzer ��ƥ]��C���� */
    GRSButtonRegisterQueue *pButtonRegisterQueue; /* Button Register ��C���� */
    /* Scanner */
    GRSQueryQueue *pScannerQueryQueue; /* Scanner �d�ߦ�C���� */
    GRSInstructionQueue *pScannerInstructionQueue; /* Scanner ���O��C���� */
    
    /*--------------------*/
    /* �������� */
    /* RTC */
    GRealTimeClock *pRealTimeClock; /* �Y�ɮ������� */
    /* Query */
    GRSTItemTree *pItemTree; /* Item Tree ���� */
    GRSResultQueue *pQueryResultQueue; /* Query Result ��C���� */
    /* Menu */
    GRSTMenuItemQueue *pCurrentMenuItemQueue;
    GRSTMenuItemQueue *pMenuItemQueue[GRSTMI_TRUNK_MENU_AMOUNT];
  };

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�κX�����`�ƦC�|��ƫ��A�ŧi�w�q =- */
/* Trunk ���A�X�����`�ƦC�|�w�q�ŧi */
enum _GRSTP_TRUNK_STATUS_FLAGS_FIELD_DEFINITION
  {
    /* -= ���A�X�����C�� =- */
    GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_COMPLETE,
    GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_SUCCEEDED,
    
    /* -= ���A�X������`�� =- */
    GRSTP_TRUNK_STATUS_FLAGS_FIELD_AMOUNT,
  };

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�κX�б`�ƦC�|��ƫ��A�ŧi�w�q =- */
/* Trunk ���A�X�б`�ƦC�|�w�q�ŧi */
enum _GRSTP_TRUNK_STATUS_FLAGS_DEFINITION
  {
    /* -= ���A�X�ЦC�� =- */
    GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE = GGD_GET_FLAG_MASK(GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_COMPLETE),
    GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED = GGD_GET_FLAG_MASK(GRSTP_TRUNK_STATUS_FLAGS_FIELD_RESOURCE_QUERY_SUCCEEDED),
    
  };

/*---------------------------------------------------------------------------*/
/* �����ܼƩw�q�ŧi */

/*---------------------------------------------------------------------------*/
/* ���ε{�ǩw�q�ŧi */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���ε{�� =- */
/* ���A���{�� */
extern GRESULT GRSPTrunkMenuFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkStatusFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkDatetimeFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkTagScanFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkTagCommitFSMD(GRSTRUNKCDS *lpControlDS);
extern GRESULT GRSPTrunkLoadResourceFSMD(GRSTRUNKCDS *lpControlDS);


/*---------------------------------------------------------------------------*/
/* ���Y�w�q�ŧi���� */
#endif

/*---------------------------------------------------------------------------*/
/* �ɮ׵��� */

