/*
============================================
 �ɮצW�١Ggrsp_trunk_tag_scan_processing.c
 �{���\��GRFID Scanner Trunk Tag Scan �B�z
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
#include "Scanner/Information/grsi_scanner_information.h"
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��`�Ʃw�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�α`�Ʃw�q�ŧi =- */
/* �h�X�ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_TAG_SCAN_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT 1200
/* ��ܶ��j�ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_TAG_SCAN_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT 125

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ������w�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ŧi���A�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���A���B�J�`�ƦC�|�w�q�ŧi =- */
/* Trunk Tag Scan ���A���B�J�`�ƦC�|�w�q�ŧi */
enum _GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEFINITION
  {
    /* -= ���A���w�� =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY,
    
    /* -= ���A���B�J =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TITLE_DISPLAY,
    
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_STATUS_QUERY,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_QUERY_COMPLETE,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_ERROR_DISPLAY,
    
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_STARTUP_INSTRUCTION,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_RELEASE,
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_ENDLE_INSTRUCTION,
    
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_ERROR_OCCURRED,
    
    /* -= ���A����^ =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_REVERSION,
    
    /* -= ���A���N�~ =- */
    GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ܼƩw�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�ιB�@�ܼƩw�q�ŧi =- */
/* Tag Scan �ʵe�ϼ˦C���ܼƩw�q�ŧi */
static const char *g_lpGRSPTrunkTagScanAnimationPatternList[] =
  {
    /*-----------------------------------------*/
    /* -= �C�� =- */
    "|>     |",
    "| >    |",
    "|  >   |",
    "|   >  |",
    "|    > |",
    "|     >|",
    
    /*-----------------------------------------*/
    /* -= �C���� =- */
    NULL,
  };

/*---------------------------------------------------------------------------*/
/* �禡�쫬�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �򥻤��Ψ�� =- */
/* ���A���{�� */
GRESULT GRSPTrunkTagScanFSMD(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �^�I�{�� =- */
static GRESULT GRSRCALLBACK GRSPTrunkTagScanQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iScannerControlAmount, int iScannerTagAmount, GRESULT hResult);

/*---------------------------------------------------------------------------*/
/* Tag Scan �B�z���A�� */
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
    /* -= �����J���A�ܤư������A�� =- */
    switch(lpControlDS->iTagScanOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���w�� =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY:
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
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TITLE_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Copyright Display =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TITLE_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* ���� Display ��ƥΩ���ܼ��D */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "Tag Scan");
            
            gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* �m�� */
            gdisDisplayDS.iYAxis = 0;
            
            /* ��J Display ��� */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /* ���A���B�J���� */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_STATUS_QUERY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �˸m���A�d�� =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_STATUS_QUERY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSQIQUERYDS gqdsQueryDS;
            
            /*-----------------------------------------*/
            /* Scanner Query Datagram Structure initialize */
            memset(&gqdsQueryDS, 0, sizeof(GRSQIQUERYDS));
            gqdsQueryDS.iQueryCode = GRSSI_SCANNER_QUERY_CODE_STATUS;
            gqdsQueryDS.lpUserParameter = lpControlDS;
            gqdsQueryDS.OnQueryComplete = GRSPTrunkTagScanQueryCompleteProcessor;
            
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
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���Ը˸m���A�d�ߧ��� =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_QUERY_COMPLETE:
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
                int i;
                
                /*---------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܪ��A */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "Device Error");
                
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
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_SCAN_DISPLAY_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
                
                /* ���A���B�J���� */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_ERROR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* �d�ߧ��� */
            else
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܪ��A */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "|      ||%06d|", lpControlDS->iScannerTagAmount);
                
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
                
                /*---------------------*/
                /* �ǳƫ��� */
                lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
                lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
                
                /* ���A���B�J���� */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �˸m���~��� =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_DEVICE_ERROR_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �p�ɮɶ����j */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* ���A���B�J���� */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_ERROR_OCCURRED;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���ԫ��� =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �ˬd���� */
            /* Trigger */
            if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_TRIGGER)
              {
                /* ���A���B�J���� */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_STARTUP_INSTRUCTION;
              }
            /* Escape */
            else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                
                /*---------------------*/
                /* ��J Buzzer ��� */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /* ���A���B�J���� */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_REVERSION;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �e�X Tag Scan Startup ���O =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_STARTUP_INSTRUCTION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            GRSIIINSDS gidsInstructionDS;
            
            GSJ_ADVANCE(lpControlDS->pSystemJournal, "�e�J���y�Ұʫ��O�A�o�ʱ��y...");
            
            /*-----------------------------------------*/
            /* ��J Scan ���O��� */
            memset(&gidsInstructionDS, 0, sizeof(GRSIIINSDS));
            gidsInstructionDS.iInstruction = GRSII_INSTRUCTION_SCAN_STARTUP;
            if(GGD_FAILED(hr = GRSIQ_GET_METHOD(lpControlDS->pScannerInstructionQueue)->Add(lpControlDS->pScannerInstructionQueue, &gidsInstructionDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSInstructionQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* ��J Buzzer ��� */
            memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
            gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
            if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �ǳƫ��� */
            lpControlDS->iButtonReleaseFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
            
            /*-----------------------------------------*/
            /* �ǳ���� */
            lpControlDS->iAnimationProgressIndex = 0;
            
            /*-----------------------------------------*/
            /* �]�w�p�ɾ� */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_SCAN_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* ���A���B�J���� */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_RELEASE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���ԫ����} =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_RELEASE:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �ˬd���� */
            /* Trigger */
            if(lpControlDS->iButtonReleaseFlags & GRSBRI_BUTTON_FLAG_TRIGGER)
              {
                /* ���A���B�J���� */
                lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_ENDLE_INSTRUCTION;
              }
            /*-----------------------------------------*/
            /* �p����ܮɶ����j */
            else if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                const char *lpAnimationPatternPointer = NULL;
                GRSDISIDDS gdisDisplayDS;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܰʵe�ĪG */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                lpControlDS->iAnimationProgressIndex %= sizeof(g_lpGRSPTrunkTagScanAnimationPatternList) / sizeof(const char *);
                lpAnimationPatternPointer = g_lpGRSPTrunkTagScanAnimationPatternList[lpControlDS->iAnimationProgressIndex];
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
                
                lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_TAG_SCAN_DISPLAY_INTERVAL_TIME_MILLISECOND_VALUE_LIMIT;
                lpControlDS->lMillisecondValue = GSTTimeGetTime();
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �e�X Tag Scan Endle ���O =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_TAG_SCAN_ENDLE_INSTRUCTION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            GRSIIINSDS gidsInstructionDS;
            
            GSJ_ADVANCE(lpControlDS->pSystemJournal, "�e�J���y������O...");
            
            /*-----------------------------------------*/
            /* ��J Scan ���O��� */
            memset(&gidsInstructionDS, 0, sizeof(GRSIIINSDS));
            gidsInstructionDS.iInstruction = GRSII_INSTRUCTION_SCAN_ENDLE;
            if(GGD_FAILED(hr = GRSIQ_GET_METHOD(lpControlDS->pScannerInstructionQueue)->Add(lpControlDS->pScannerInstructionQueue, &gidsInstructionDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSInstructionQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* ��J Buzzer ��� */
            memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
            gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
            if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �ǳƫ��� */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_TRIGGER;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            
            /* ���A���B�J���� */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_WAIT_FOR_BUTTON_PRESS;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= �o�Ϳ��~ =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_ERROR_OCCURRED:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_FALSE;
            
            /* ���A�B�J��^ */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A����^ =- */
        case GRSP_TRUNK_TAG_SCAN_FSMD_STEP_REVERSION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_OK;
            
            /* ���A�B�J��^ */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���N�~ =- */
        default:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A�B�J��^ */
            lpControlDS->iTagScanOperationFSMDStep = GRSP_TRUNK_TAG_SCAN_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Query �����B�z�{�� */
static GRESULT GRSRCALLBACK GRSPTrunkTagScanQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iScannerControlAmount, int iScannerTagAmount, GRESULT hResult)
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
/* �ɮ׵��� */

