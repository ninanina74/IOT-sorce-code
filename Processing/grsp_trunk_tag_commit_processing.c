/*
============================================
 �ɮצW�١Ggrsp_trunk_tag_commit_processing.c
 �{���\��GRFID Scanner Trunk Tag Commit �B�z
 �]�p�̡Gù�I�� (5107056012)
 ���v�֦��̡GSeamus Berloz (�a��������ù)
 �̫��s����G2019/06/09 - 11:32:29
 ��L��T�G
============================================
*/
/*---------------------------------------------------------------------------*/
/* �]�t���Y�� */
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
/* -= �۩w�q���Y�� =- */
#include "Resource/Information/grsi_resource_information.h"
#include "Scanner/Information/grsi_scanner_information.h"
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��`�Ʃw�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�α`�Ʃw�q�ŧi =- */
/* ��ܿ��~�ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_TAG_COMMIT_ERROR_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT 1200
/* ��ܧ����ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_TAG_COMMIT_FINISH_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT 750
/* ��ܶ��j�ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_TAG_COMMIT_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT 250

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ������w�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ŧi���A�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���A���B�J�`�ƦC�|�w�q�ŧi =- */
/* Trunk Tag Scan ���A���B�J�`�ƦC�|�w�q�ŧi */
enum _GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_DEFINITION
  {
    /* -= ���A���w�� =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY,
    
    /* -= ���A���B�J =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_TITLE_DISPLAY,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCANNER_TAG_QUERY,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_TAG_QUERY_COMPLETE,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_COMMIT_COMPLETE,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_PROGRESS_DISPLAY,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY,
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY,
    
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_OCCURRED,
    
    /* -= ���A����^ =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_REVERSION,
    
    /* -= ���A���N�~ =- */
    GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ܼƩw�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�ιB�@�ܼƩw�q�ŧi =- */
/* Tag Scan �ʵe�ϼ˦C���ܼƩw�q�ŧi */
static const char *g_lpGRSPTrunkTagCommitAnimationPatternList[] =
  {
    /*-----------------------------------------*/
    /* -= �C�� =- */
    "|     <|",
    "|    < |",
    "|   <  |",
    "|  <   |",
    "| <    |",
    "|<     |",
    
    /*-----------------------------------------*/
    /* -= �C���� =- */
    NULL,
  };

/*---------------------------------------------------------------------------*/
/* �禡�쫬�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �򥻤��Ψ�� =- */
/* ���A���{�� */
GRESULT GRSPTrunkTagCommitFSMD(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �^�I�{�� =- */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength);
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult);
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult);

/*---------------------------------------------------------------------------*/
/* Tag Commit �B�z���A�� */
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
    /* -= �����J���A�ܤư������A�� =- */
    switch(lpControlDS->iTagCommitOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���w�� =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* ���� Display ��ƥΩ�M����� */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            gdisDisplayDS.iFlags = GRSDISI_DISPLAY_FLAG_CONTROL | GRSDISI_DISPLAY_FLAG_HOMING | GRSDISI_DISPLAY_FLAG_CLEAR;
            /* ��J Display ��� */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /* ���A���B�J���� */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_TITLE_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Copyright Display =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_TITLE_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* ���� Display ��ƥΩ���ܼ��D */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "Tag Commit");
            
            gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* �m�� */
            gdisDisplayDS.iYAxis = 0;
            
            /* ��J Display ��� */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /* ���A���B�J���� */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCANNER_TAG_QUERY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Scanner Tag Query =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCANNER_TAG_QUERY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
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
            /* �[�J Query ��� */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pScannerQueryQueue)->Add(lpControlDS->pScannerQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Query ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �]�m�X�� */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* ���A���B�J���� */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_TAG_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���� Scanner Tag �d�ߧ��� =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_TAG_QUERY_COMPLETE:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
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
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܪ��A */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Query Error");
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Query Error", iLengthRemainders, "                ");
                
                gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* �m�� */
                gdisDisplayDS.iYAxis = 1;
                
                /* ��J Display ��� */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* ��J Buzzer ��� */
                for(i = 0; i < 2; i ++)
                  {
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_BEEP;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                  }
                
                /*---------------------*/
                /* �]�w�p�ɾ� */
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_ERROR_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* �d�ߧ��� */
            else
              {
                int iResultAmount, iLengthRemainders;
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* �ˬd Result �ƶq */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /*---------------------*/
                    /* ���� Display ��ƥΩ���ܪ��A */
                    memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                    
                    iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Have No Tag");
                    gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Have No Tag", iLengthRemainders, "                ");
                    
                    gdisDisplayDS.iXAxis = 0;
                    gdisDisplayDS.iYAxis = 1;
                    
                    /* ��J Display ��� */
                    if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /*---------------------*/
                    /* ��J Buzzer ��� */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /*---------------------*/
                    /* �]�w�p�ɾ� */
                    lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_FINISH_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                    lpControlDS->lMillisecondValue = GSTTimeGetTime();
                    
                    /* ���A���B�J���� */
                    lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY;
                  }
                else
                  {
                    /*---------------------*/
                    /* ���� Display ��ƥΩ���ܪ��A */
                    memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                    
                    gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "|      ||%06d|", iResultAmount);
                    
                    gdisDisplayDS.iXAxis = 0;
                    gdisDisplayDS.iYAxis = 1;
                    
                    /* ��J Display ��� */
                    if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /*---------------------*/
                    /* ��J Buzzer ��� */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Scan Record Commit =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTIIDS *lpForFoundItemDS = NULL;
            void *lpResultDatagramDS = NULL;
            
            /*-----------------------------------------*/
            /* ���X Result ��� */
            if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Result ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            else if(hr != GRSRQ_ACCESS_OK)
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                int iLengthRemainders;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܪ��A */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Completed");
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Completed", iLengthRemainders, "                ");
                
                gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* �m�� */
                gdisDisplayDS.iYAxis = 1;
                
                /* ��J Display ��� */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* ��J Buzzer ��� */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* �]�w�p�ɾ� */
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_FINISH_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY;
              }
            /*-----------------------------------------*/
            /* �j�d Item Tree�A���w�� Tag �O�_�s�b */
            else if(GGD_FAILED(hr = GRSTIT_GET_METHOD(lpControlDS->pItemTree)->Find(lpControlDS->pItemTree, &lpForFoundItemDS, ((GRSSITAGDS *)lpResultDatagramDS)->strTagIdentifier, GAVLT_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Item Tree ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* ���s�b�� Tag�A������� */
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
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�O����Ŷ��t�m�ɡA�O���餣���I���~�X�G0x%08X�C\n", GGD_ERROR_OUT_OF_MEMORY);
                    
                    return GGD_ERROR_OUT_OF_MEMORY;
                  }
                
                /*---------------------*/
                /* ���˰򥻶��� */
                memset(gqdsQueryDS.lpInputDatagramDS, 0, gqdsQueryDS.iInputDatagramDSLength);
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iDeviceIdentifier = 1;
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iItemIdentifier = lpForFoundItemDS->iIdentifier;
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iStatusIdentifier = lpControlDS->iStatusIdentifier;
                ((GRSRISRECORDDS *)(gqdsQueryDS.lpInputDatagramDS))->iLocationIdentifier = lpControlDS->iLocationIdentifier;
                
                /*---------------------*/
                /* �[�J Query ��� */
                if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Query ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* �]�m�X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* Result ��ƧR���A�᭱�Τ���F */
                GGD_MEMORY_RELEASE(lpResultDatagramDS);
                
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_COMMIT_COMPLETE;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���� Scan Record Commit ���� =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_WAIT_COMMIT_COMPLETE:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
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
                
                GSJ_WARNING(lpControlDS->pSystemJournal, "�귽�GScan Record �g�J�ާ@�B�z�A�^�I�^�ǬO�GResource query failed �I\n");
                
                /*---------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܪ��A */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen("Commit Error");
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", "Commit Error", iLengthRemainders, "                ");
                
                gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* �m�� */
                gdisDisplayDS.iYAxis = 1;
                
                /* ��J Display ��� */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* ��J Buzzer ��� */
                for(i = 0; i < 2; i ++)
                  {
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_BEEP;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                  }
                
                /*---------------------*/
                /* �M�� Result ��C */
                GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Clear(lpControlDS->pQueryResultQueue);
                
                /*---------------------*/
                /* �]�w�p�ɾ� */
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_COMMIT_ERROR_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* �ާ@���� */
            else
              {
                const char *lpAnimationPatternPointer = NULL;
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܰʵe�ĪG */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                lpControlDS->iAnimationProgressIndex %= sizeof(g_lpGRSPTrunkTagCommitAnimationPatternList) / sizeof(const char *);
                lpAnimationPatternPointer = g_lpGRSPTrunkTagCommitAnimationPatternList[lpControlDS->iAnimationProgressIndex];
                lpControlDS->iAnimationProgressIndex ++;
                
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, lpAnimationPatternPointer);
                
                gdisDisplayDS.iXAxis = 0;
                gdisDisplayDS.iYAxis = 1;
                
                /* ��J Display ��� */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /*---------------------*/
                /* ��J Buzzer ��� */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_SCAN_RECORD_COMMIT;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ��ܧ��� =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_FINISH_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �p�ɮɶ����j */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_REVERSION;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ��ܿ��~ =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �p�ɮɶ����j */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* ���A���B�J���� */
                lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_OCCURRED;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �o�Ϳ��~ =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_ERROR_OCCURRED:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_FALSE;
            
            /* ���A�B�J��^ */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A����^ =- */
        case GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_REVERSION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_OK;
            
            /* ���A�B�J��^ */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���N�~ =- */
        default:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A�B�J��^ */
            lpControlDS->iTagCommitOperationFSMDStep = GRSP_TRUNK_TAG_COMMIT_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Scanner Query ���G�B�z�{�� */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength)
  {
    void *lpResultDatagramDSPointer = NULL;
    
    GRSTRUNKCDS *lpControlDS = NULL;
    GRESULT hr;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* �q�ѼƤ����^ Message ��T��Ƶ��c���� */
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* �� Result �t�m�O����Ŷ� */
    if((lpResultDatagramDSPointer = GGD_MEMORY_ALLOCATE(iResultDatagramDSLength)) == NULL)
      {
        return GGD_ERROR_OUT_OF_MEMORY;
      }
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* �s�J Result ��� */
    memcpy(lpResultDatagramDSPointer, lpResultDatagramDS, iResultDatagramDSLength);
    if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Add(lpControlDS->pQueryResultQueue, lpResultDatagramDSPointer)))
      {
        return hr;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Scanner Query �����B�z�{�� */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitScannerQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* �q�ѼƤ����^ Message ��T��Ƶ��c���� */
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* ��s�X�� */
    lpControlDS->iStatusFlags |= GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
    if(hResult == GGD_OK)
      {
        lpControlDS->iStatusFlags |= GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* Resource Query �����B�z�{�� */
static GRESULT GRSRCALLBACK GRSPTrunkTagCommitResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult)
  {
    GRSTRUNKCDS *lpControlDS = NULL;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* �q�ѼƤ����^ Message ��T��Ƶ��c���� */
    lpControlDS = (GRSTRUNKCDS *)lpParameter;
    
    /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
    /* ��s�X�� */
    lpControlDS->iStatusFlags |= GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
    if(hResult == GGD_OK)
      {
        lpControlDS->iStatusFlags |= GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
      }
    
    return GGD_OK;
  }

/*---------------------------------------------------------------------------*/
/* �ɮ׵��� */

