/*
============================================
 �ɮצW�١Ggrsp_trunk_menu_processing.c
 �{���\��GRFID Scanner Trunk Menu �B�z
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
#include "Common/Information/grsi_buzzer_information.h"
#include "SystemTimeTool/gst_system_time.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��`�Ʃw�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �`�α`�Ʃw�q�ŧi =- */
/* ��ܮɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_MENU_SHOW_MILLISECOND_VALUE_LIMIT 1200
/* �h�X�ɶ��@��ȱ`�ƪ�ŧi�w�q */
#define GRSP_TRUNK_MENU_TIMEOUT_MILLISECOND_VALUE_LIMIT 8000

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ������w�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ŧi���A�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���A���B�J�`�ƦC�|�w�q�ŧi =- */
/* Trunk Menu ���A���B�J�`�ƦC�|�w�q�ŧi */
enum _GRSP_TRUNK_MENU_FSMD_STEP_DEFINITION
  {
    /* -= ���A���w�� =- */
    GRSP_TRUNK_MENU_FSMD_STEP_READY,
    
    /* -= ���A���B�J =- */
    GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY,
    
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY,
    GRSP_TRUNK_MENU_FSMD_STEP_WAIT_TIME_INTERVAL,
    
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ENTER,
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ROTATE,
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ESCAPE,
    
    GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION,
    GRSP_TRUNK_MENU_FSMD_STEP_SHOW_TIME_INTERVAL,
    
    GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT,
    
    /* -= ���A����^ =- */
    GRSP_TRUNK_MENU_FSMD_STEP_REVERSION,
    
    /* -= ���A���N�~ =- */
    GRSP_TRUNK_MENU_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ܼƩw�q */

/*---------------------------------------------------------------------------*/
/* �禡�쫬�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �򥻤��Ψ�� =- */
/* ���A���{�� */
GRESULT GRSPTrunkMenuFSMD(GRSTRUNKCDS *lpControlDS);

/*---------------------------------------------------------------------------*/
/* Status �B�z���A�� */
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
    /* -= �����J���A�ܤư������A�� =- */
    switch(lpControlDS->iMenuOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���w�� =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_READY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* ��l�w�}��� */
            lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[GRSTMI_TRUNK_MENU_MAIN];
            lpControlDS->iMenuIndex = GRSTMI_TRUNK_MENU_MAIN;
            
            /* ���A���B�J���� */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Clear Display =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY:
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
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Display =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            GRSDISIDDS gdisDisplayDS;
            
            /*-----------------------------------------*/
            /* ���o Menu Item */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Get(lpControlDS->pCurrentMenuItemQueue, &lpMenuItemDS, GSQ_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* �p�G�����\ */
            else if(hr != GRSTMIQ_ACCESS_OK)
              {
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            else
              {
                int iLengthRemainders;
                
                /*---------------------*/
                /* ���� Display ��ƥΩ���ܼ��D */
                memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
                
                iLengthRemainders = GRSDISI_DISPLAY_DATA_ARRAY_LENGTH - strlen(lpMenuItemDS->strTitleName);
                gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, "%s%.*s", lpMenuItemDS->strTitleName, iLengthRemainders, "                ");
                
                gdisDisplayDS.iXAxis = 0;
                gdisDisplayDS.iYAxis = lpMenuItemDS->iDisplayLine;
                
                /*---------------------*/
                /* ��J Display ��� */
                if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
              }
            
            /*-----------------------------------------*/
            /* �ǳƫ��� */
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            
            /*-----------------------------------------*/
            /* �]�w�O�ɭp�ɾ� */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_MENU_TIMEOUT_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* ���A���B�J���� */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_WAIT_TIME_INTERVAL;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���Ԯɶ����j =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_WAIT_TIME_INTERVAL:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �ˬd���� */
            /* Select Button */
            if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_SELECT)
              {
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ROTATE;
              }
            /* Escape Button */
            else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ESCAPE)
              {
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ESCAPE;
              }
            /* Enter Button */
            else if(lpControlDS->iButtonPressFlags & GRSBRI_BUTTON_FLAG_ENTER)
              {
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ENTER;
              }
            /*-----------------------------------------*/
            /* �p�ɹO�ɮɶ� */
            else if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Rotate =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ROTATE:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
                
            /*-----------------------------------------*/
            /* ��J Buzzer ��� */
            memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
            gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
            if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* Menu Item �u�� */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Rotate(lpControlDS->pCurrentMenuItemQueue)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /* ���A���B�J���� */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Escape =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ESCAPE:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            GRSDIDDS gddsBuzzerDatagramDS;
            
            /*-----------------------------------------*/
            /* ���o Menu Item */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Get(lpControlDS->pCurrentMenuItemQueue, &lpMenuItemDS, GSQ_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* �p�G�����\ */
            else if(hr != GRSTMIQ_ACCESS_OK)
              {
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
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            /*-----------------------------------------*/
            /* �ˬd�h�X�᪺�W�h��� */
            else if((lpMenuItemDS->iEscapeMenuIndex >= 0) && (lpMenuItemDS->iEscapeMenuIndex < GRSTMI_TRUNK_MENU_AMOUNT))
              {
                /*---------------------*/
                /* ������� */
                lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[lpMenuItemDS->iEscapeMenuIndex];
                lpControlDS->iMenuIndex = lpMenuItemDS->iEscapeMenuIndex;
                
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
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY;
              }
            /*-----------------------------------------*/
            /* ��e�w�g�O���h�A�����h�X */
            else
              {
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
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Item Enter =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_ENTER:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            GRSTMIITEMDS *lpMenuItemDS = NULL;
            GRSDIDDS gddsBuzzerDatagramDS;
            
            /*-----------------------------------------*/
            /* ���o Menu Item */
            if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Get(lpControlDS->pCurrentMenuItemQueue, &lpMenuItemDS, GSQ_ACCESS_METHOD_COPY)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            /* �p�G�����\ */
            else if(hr != GRSTMIQ_ACCESS_OK)
              {
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
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
              }
            /*-----------------------------------------*/
            /* �i�J��� */
            else if(lpMenuItemDS->iEnterMode == GRSTMI_TRUNK_MENU_ENTER_MODE_ENTER)
              {
                /*---------------------*/
                /* �ˬd�i�J�᪺�U�h��� */
                if((lpMenuItemDS->iEnterMenuIndex >= 0) && (lpMenuItemDS->iEnterMenuIndex < GRSTMI_TRUNK_MENU_AMOUNT))
                  {
                    /* ������� */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[lpMenuItemDS->iEnterMenuIndex];
                    lpControlDS->iMenuIndex = lpMenuItemDS->iEnterMenuIndex;
                    
                    /* ��J Buzzer ��� */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_ITEM_DISPLAY;
                  }
                /* �o�ͷN�~ */
                else
                  {
                    /* ��J Buzzer ��� */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
                  }
              }
            /*-----------------------------------------*/
            /* �ާ@��� */
            else if(lpMenuItemDS->iEnterMode == GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT)
              {
                /*---------------------*/
                /* �ˬd����᪺�W�h��� */
                if((lpMenuItemDS->iEscapeMenuIndex >= 0) && (lpMenuItemDS->iEscapeMenuIndex < GRSTMI_TRUNK_MENU_AMOUNT))
                  {
                    /* �ƻs���СA�y����ܥ� */
                    lpControlDS->lpSelectedMenuTitleName = lpMenuItemDS->strTitleName;
                    
                    /* �O�s��� */
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
                    
                    /* ������� */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[lpMenuItemDS->iEscapeMenuIndex];
                    lpControlDS->iMenuIndex = lpMenuItemDS->iEscapeMenuIndex;
                    
                    /* ��J Buzzer ��� */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iMenuOperationFSMDNextStep = GRSP_TRUNK_MENU_FSMD_STEP_CLEAR_DISPLAY;
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION;
                  }
                /* �o�ͷN�~ */
                else
                  {
                    /* ��J Buzzer ��� */
                    memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                    gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_LONG_TICK;
                    if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                      {
                        GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                        
                        return hr;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT;
                  }
              }
            /*-----------------------------------------*/
            /* �ާ@���� */
            else if(lpMenuItemDS->iEnterMode == GRSTMI_TRUNK_MENU_ENTER_MODE_EXECUTE)
              {
                /* �O�s��� */
                lpControlDS->iExecutionSelection = lpMenuItemDS->iExecutionSelection;
                
                /* �ƻs���СA�y����ܥ� */
                lpControlDS->lpSelectedMenuTitleName = lpMenuItemDS->strTitleName;
                
                /* ��J Buzzer ��� */
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
                
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDNextStep = GRSP_TRUNK_MENU_FSMD_STEP_REVERSION;
                lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Show Selection =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_SHOW_SELECTION:
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
            
            /*-----------------------------------------*/
            /* ���� Display ��ƥΩ���ܿ�� */
            memset(&gdisDisplayDS, 0, sizeof(GRSDISIDDS));
            
            gdisDisplayDS.iLength = snprintf((char *)(gdisDisplayDS.bytDataArray), GRSDISI_DISPLAY_DATA_ARRAY_LENGTH_MAXIMUM, lpControlDS->lpSelectedMenuTitleName);
            
            gdisDisplayDS.iXAxis = ((GRSDISI_DISPLAY_DATA_ARRAY_LENGTH / 2) - (gdisDisplayDS.iLength / 2)); /* �Ĥ@�Ƹm�� */
            gdisDisplayDS.iYAxis = 0;
            
            /* ��J Display ��� */
            if(GGD_FAILED(hr = GRSDISQ_GET_METHOD(lpControlDS->pDisplayQueue)->Add(lpControlDS->pDisplayQueue, &gdisDisplayDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Display ��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*---------------------*/
            /* �]�w�O�ɭp�ɾ� */
            lpControlDS->lMillisecondValueLimit = GRSP_TRUNK_MENU_SHOW_MILLISECOND_VALUE_LIMIT;
            lpControlDS->lMillisecondValue = GSTTimeGetTime();
            
            /* ���A���B�J���� */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_SHOW_TIME_INTERVAL;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Show Time Interval =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_SHOW_TIME_INTERVAL:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /*-----------------------------------------*/
            /* �p�ɹO�ɮɶ� */
            if(GSTTimeCheckTimeUp(lpControlDS->lMillisecondValue, lpControlDS->lMillisecondValueLimit))
              {
                /* ���A���B�J���� */
                lpControlDS->iMenuOperationFSMDStep = lpControlDS->iMenuOperationFSMDNextStep;
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Menu Quit =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_MENU_QUIT:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_FALSE;
            
            /*-----------------------------------------*/
            /* ���}�e�M�z���� */
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            
            /* ���A�B�J��^ */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A����^ =- */
        case GRSP_TRUNK_MENU_FSMD_STEP_REVERSION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_OK;
            
            /*-----------------------------------------*/
            /* ���}�e�M�z���� */
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_SELECT;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ESCAPE;
            //lpControlDS->iButtonPressFlags &= ~GRSBRI_BUTTON_FLAG_ENTER;
            
            /* ���A�B�J��^ */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���N�~ =- */
        default:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A�B�J��^ */
            lpControlDS->iMenuOperationFSMDStep = GRSP_TRUNK_MENU_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* �ɮ׵��� */

