/*
============================================
 �ɮצW�١Ggrsp_trunk_datetime_processing.c
 �{���\��GRFID Scanner Trunk Datatime �B�z
 �]�p�̡Gù�I�� (5107056012)
 ���v�֦��̡GSeamus Berloz (�a��������ù)
 �̫��s����G2019/06/08 - 11:32:29
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
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��`�Ʃw�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�α`�Ʃw�q�ŧi =- */
/* �ɶ���s�ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_DATETIME_REFRESH_INTERVAL_MILLISECOND_VALUE_LIMIT 500

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ������w�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ŧi���A�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���A���B�J�`�ƦC�|�w�q�ŧi =- */
/* Trunk Datetime ���A���B�J�`�ƦC�|�w�q�ŧi */
enum _GRSP_TRUNK_DATETIME_FSMD_STEP_DEFINITION
  {
    /* -= ���A���w�� =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_READY,
    
    /* -= ���A���B�J =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY,
    GRSP_TRUNK_DATETIME_FSMD_STEP_WAIT_TIME_INTERVAL,
    
    /* -= ���A����^ =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_REVERSION,
    
    /* -= ���A���N�~ =- */
    GRSP_TRUNK_DATETIME_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ܼƩw�q */

/*---------------------------------------------------------------------------*/
/* �禡�쫬�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �򥻤��Ψ�� =- */
/* ���A���{�� */
GRESULT GRSPTrunkDatetimeFSMD(GRSTRUNKCDS *lpControlDS);

/*---------------------------------------------------------------------------*/
/* Datetime �B�z���A�� */
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
    /* -= �����J���A�ܤư������A�� =- */
    switch(lpControlDS->iDatetimeOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���w�� =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_READY:
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
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ��ܤ���ɶ� =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRTCTIME *lpLocalTime = NULL;
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* ���o���a�ɶ� */
            if((lpLocalTime = GRTC_GET_METHOD(lpControlDS->pRealTimeClock)->GetLocalTime(lpControlDS->pRealTimeClock, NULL)) == NULL)
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�i����o���a�ɶ��ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", GGD_ERROR_UNEXPECTED);
                
                return GGD_ERROR_UNEXPECTED;
              }
            
            /*-----------------------------------------*/
            /* ���� Display ��ƥΩ���ܤ�� */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%04d-%02d-%02d",
                lpLocalTime->iYears + GRTC_TIME_SINCE_YEARS, lpLocalTime->iMonths + 1, lpLocalTime->iMDay);
            
            gdisDisplayDS.iXAxis = 0;
            gdisDisplayDS.iYAxis = 0;
            
            /* ��J Display ��� */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* ���� Display ��ƥΩ���ܮɶ� */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "-= %02d:%02d:%02d =-",
                lpLocalTime->iHours, lpLocalTime->iMinutes, lpLocalTime->iSecond);
            
            gdisDisplayDS.iXAxis = 1;
            gdisDisplayDS.iYAxis = 1;
            
            /* ��J Display ��� */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �ǳưh�X���� */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            
            /*-----------------------------------------*/
            /* �]�w�p�ɾ� */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_DATETIME_REFRESH_INTERVAL_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* ���A���B�J���� */
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_WAIT_TIME_INTERVAL;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���Ԯɶ����j =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_WAIT_TIME_INTERVAL:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �ˬd�h�X���� */
            if((lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT) || (lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE))
              {
                GRSDIDDS gddsBuzzerDatagramDS;
                
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
                lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_REVERSION;
              }
            /*-----------------------------------------*/
            /* �p�ɮɶ����j */
            else if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* ���A���B�J���� */
                lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_DATETIME_DISPLAY;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A����^ =- */
        case GRSP_TRUNK_DATETIME_FSMD_STEP_REVERSION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_OK;
            
            /* ���A�B�J��^ */
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���N�~ =- */
        default:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A�B�J��^ */
            lpControlDS->iDatetimeOperationFSMDStep = GRSP_TRUNK_DATETIME_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* �ɮ׵��� */

