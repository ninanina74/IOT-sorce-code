/*
============================================
 �ɮצW�١Ggrsp_trunk_load_resource_processing.c
 �{���\��GRFID Scanner Trunk Load Resource �B�z
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
#include "Resource/Information/grsi_resource_information.h"
#include "Common/Information/grsi_buzzer_information.h"
#include "grsp_trunk_processing.h"

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��`�Ʃw�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ������w�q */

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ŧi���A�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= ���A���B�J�`�ƦC�|�w�q�ŧi =- */
/* Trunk Status ���A���B�J�`�ƦC�|�w�q�ŧi */
enum _GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_DEFINITION
  {
    /* -= ���A���w�� =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY,
    
    /* -= ���A���B�J =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ITEM_QUERY,
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_ITEM_QUERY_COMPLETE,
    
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY,
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_STATUS_QUERY_COMPLETE,
    
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_LOCATION_QUERY,
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_LOCATION_QUERY_COMPLETE,
    
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED,
    
    /* -= ���A����^ =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_REVERSION,
    
    /* -= ���A���N�~ =- */
    GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_DEFAULT = 0x7fffffff
  };

/*---------------------------------------------------------------------------*/
/* �Ȩѥ��ҲձM�Τ��ܼƩw�q */

/*---------------------------------------------------------------------------*/
/* �禡�쫬�w�q */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �򥻤��Ψ�� =- */
/* ���A���{�� */
GRESULT GRSPTrunkLoadResourceFSMD(GRSTRUNKCDS *lpControlDS);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* -= �^�I�{�� =- */
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength);
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult);

/*---------------------------------------------------------------------------*/
/* Status �B�z���A�� */
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
    /* -= �����J���A�ܤư������A�� =- */
    switch(lpControlDS->iLoadResourceOperationFSMDStep)
      {
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���w�� =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A���B�J���� */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ITEM_QUERY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Item Query =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ITEM_QUERY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
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
            /* �[�J Query ��� */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Query ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �]�m�X�� */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* ���A���B�J���� */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_ITEM_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���� Item Query ���� =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_ITEM_QUERY_COMPLETE:
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
                GSJ_WARNING(lpControlDS->pSystemJournal, "�귽�GStatus �d�߾ާ@�B�z�A�^�I�^�ǬO�GResource query failed �I\n");
                
                /* �X�г]�m */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /* ���A���B�J���� */
                lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
              }
            /*-----------------------------------------*/
            /* �ާ@���� */
            else
              {
                int iResultAmount = 0;
                
                /*-----------------------------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*-----------------------------------------*/
                /* �ˬd Result �ƶq */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /* ���A���B�J���� */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY;
                  }
                /*-----------------------------------------*/
                /* Result �B�z */
                else
                  {
                    /*-----------------------------------------*/
                    /* �j��B�z�Ҧ� Result */
                    while(TRUE)
                      {
                        GRSTIIDS *lpForFoundItemDS = NULL;
                        GRSTIIDS *lpItemDS = NULL;
                        
                        void *lpResultDatagramDS = NULL;
                        
                        /*--------------------*/
                        /* ���X Result ��� */
                        if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Result ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        else if(hr != GRSRQ_ACCESS_OK)
                          {
                            break;
                          }
                        
                        //GSJ_ADVANCE(lpControlDS->pSystemJournal, "Item(%d)=[%s]", ((GRSRIITEMDS *)lpResultDatagramDS)->iIdentifier, ((GRSRIITEMDS *)lpResultDatagramDS)->strTagIdentifier);
                        
                        /*--------------------*/
                        /* �����ˬd */
                        if(GGD_FAILED(hr = GRSTIT_GET_METHOD(lpControlDS->pItemTree)->Find(lpControlDS->pItemTree, &lpForFoundItemDS, ((GRSRIITEMDS *)lpResultDatagramDS)->strTagIdentifier, GAVLT_ACCESS_METHOD_COPY)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Trunk Item Tree ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        /* ��Ƥw�g�s�b */
                        else if(hr == GRSTIT_ACCESS_OK)
                          {
                            /* Result ��ƧR���A�᭱�Τ���F */
                            GGD_MEMORY_RELEASE(lpResultDatagramDS);
                            
                            continue;
                          }
                        
                        /*--------------------*/
                        /* Item �O����Ŷ��t�m */
                        if((lpItemDS = (GRSTIIDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTIIDS))) == NULL)
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Item �O����Ŷ��t�m�ɡA�O���餣���I���~�X�G0x%08X�C\n", GGD_ERROR_OUT_OF_MEMORY);
                            
                            return GGD_ERROR_OUT_OF_MEMORY;
                          }
                        
                        /*--------------------*/
                        /* ���� */
                        memset(lpItemDS, 0, sizeof(GRSTIIDS));
                        lpItemDS->iIdentifier = ((GRSRIITEMDS *)lpResultDatagramDS)->iIdentifier;
                        snprintf(lpItemDS->strTagIdentifier, GRSTII_TRUNK_ITEM_TAG_IDENTIFIER_STRING_LENGTH_MAXIMUM, ((GRSRIITEMDS *)lpResultDatagramDS)->strTagIdentifier);
                        
                        /*--------------------*/
                        /* �O�s */
                        if(GGD_FAILED(hr = GRSTIT_GET_METHOD(lpControlDS->pItemTree)->Add(lpControlDS->pItemTree, lpItemDS)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Trunk Item Tree ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        
                        /*--------------------*/
                        /* Result ��ƧR���A�᭱�Τ���F */
                        GGD_MEMORY_RELEASE(lpResultDatagramDS);
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Status Query =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_STATUS_QUERY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
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
            /* �[�J Query ��� */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Query ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �]�m�X�� */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* ���A���B�J���� */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_STATUS_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���� Status Query ���� =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_STATUS_QUERY_COMPLETE:
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
                GSJ_WARNING(lpControlDS->pSystemJournal, "�귽�GStatus �d�߾ާ@�B�z�A�^�I�^�ǬO�GResource query failed �I\n");
                
                /* �X�г]�m */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /* ���A���B�J���� */
                lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
              }
            /*-----------------------------------------*/
            /* �ާ@���� */
            else
              {
                int iResultAmount = 0;
                
                /*-----------------------------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*-----------------------------------------*/
                /* �ˬd Result �ƶq */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /* ���A���B�J���� */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
                  }
                /*-----------------------------------------*/
                /* Result �B�z */
                else
                  {
                    int iResultIndex;
                    
                    /*-----------------------------------------*/
                    /* Status �w�}��� */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[GRSTMI_TRUNK_MENU_STATUS];
                    
                    /*-----------------------------------------*/
                    /* �w���M����l�C�� */
                    GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Clear(lpControlDS->pCurrentMenuItemQueue);
                    
                    /*-----------------------------------------*/
                    /* �j��B�z�Ҧ� Result */
                    iResultIndex = 0;
                    while(TRUE)
                      {
                        GRSTMIITEMDS *lpMenuItemDS = NULL;
                        void *lpResultDatagramDS = NULL;
                        
                        /*--------------------*/
                        /* ���X Result ��� */
                        if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Result ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        else if(hr != GRSRQ_ACCESS_OK)
                          {
                            break;
                          }
                        
                        //GSJ_ADVANCE(lpControlDS->pSystemJournal, "Status(%d)=[%s]", ((GRSRISTATUSDS *)lpResultDatagramDS)->iIdentifier, ((GRSRISTATUSDS *)lpResultDatagramDS)->strCode);
                        
                        /*--------------------*/
                        /* Menu Item �O����Ŷ��t�m */
                        if((lpMenuItemDS = (GRSTMIITEMDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTMIITEMDS))) == NULL)
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Menu Item �O����Ŷ��t�m�ɡA�O���餣���I���~�X�G0x%08X�C\n", GGD_ERROR_OUT_OF_MEMORY);
                            
                            return GGD_ERROR_OUT_OF_MEMORY;
                          }
                        
                        /*--------------------*/
                        /* ���� */
                        memset(lpMenuItemDS, 0, sizeof(GRSTMIITEMDS));
                        lpMenuItemDS->iIndex = iResultIndex;
                        lpMenuItemDS->iEnterMenuIndex = (-1);
                        lpMenuItemDS->iEscapeMenuIndex = GRSTMI_TRUNK_MENU_MAIN;
                        lpMenuItemDS->iEnterMode = GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT;
                        lpMenuItemDS->iEntrainedDataValue = ((GRSRISTATUSDS *)lpResultDatagramDS)->iIdentifier;
                        snprintf(lpMenuItemDS->strTitleName, GRSTMI_TRUNK_MENU_ITEM_TITLE_NAME_LENGTH_MAXIMUM, ((GRSRISTATUSDS *)lpResultDatagramDS)->strCode);
                        lpMenuItemDS->iDisplayLine = 1;
                        
                        /*--------------------*/
                        /* �O�s */
                        if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Add(lpControlDS->pCurrentMenuItemQueue, lpMenuItemDS)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        
                        /*--------------------*/
                        /* Result ��ƧR���A�᭱�Τ���F */
                        GGD_MEMORY_RELEASE(lpResultDatagramDS);
                        
                        /*--------------------*/
                        /* Result ���޲֭p */
                        iResultIndex ++;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_LOCATION_QUERY;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= Location Query =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_LOCATION_QUERY:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
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
            /* �[�J Query ��� */
            if(GGD_FAILED(hr = GRSQQ_GET_METHOD(lpControlDS->pResourceQueryQueue)->Add(lpControlDS->pResourceQueryQueue, &gqdsQueryDS)))
              {
                GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Query ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                
                return hr;
              }
            
            /*-----------------------------------------*/
            /* �]�m�X�� */
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
            lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
            
            /* ���A���B�J���� */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_LOCATION_QUERY_COMPLETE;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���� Loation Query ���� =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_WAIT_LOCATION_QUERY_COMPLETE:
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
                GSJ_WARNING(lpControlDS->pSystemJournal, "�귽�GLocation �d�߾ާ@�B�z�A�^�I�^�ǬO�GResource query failed �I\n");
                
                /* �X�г]�m */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                
                /* ���A���B�J���� */
                lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
              }
            /*-----------------------------------------*/
            /* �ާ@���� */
            else
              {
                int iResultAmount = 0;
                
                /*-----------------------------------------*/
                /* �M���X�� */
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_COMPLETE;
                lpControlDS->iStatusFlags &= ~GRSTP_TRUNK_STATUS_FLAG_RESOURCE_QUERY_SUCCEEDED;
                
                /*-----------------------------------------*/
                /* �ˬd Result �ƶq */
                if((iResultAmount = GSQ_GET_METHOD((GSerialQueue *)(lpControlDS->pQueryResultQueue))->GetNodeAmount((GSerialQueue *)(lpControlDS->pQueryResultQueue))) <= 0)
                  {
                    /* ���A���B�J���� */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED;
                  }
                /*-----------------------------------------*/
                /* Result �B�z */
                else
                  {
                    int iResultIndex;
                    
                    /*-----------------------------------------*/
                    /* Location �w�}��� */
                    lpControlDS->pCurrentMenuItemQueue = lpControlDS->pMenuItemQueue[GRSTMI_TRUNK_MENU_LOCATION];
                    
                    /*-----------------------------------------*/
                    /* �w���M����l�C�� */
                    GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Clear(lpControlDS->pCurrentMenuItemQueue);
                    
                    /*-----------------------------------------*/
                    /* �j��B�z�Ҧ� Result */
                    iResultIndex = 0;
                    while(TRUE)
                      {
                        GRSTMIITEMDS *lpMenuItemDS = NULL;
                        void *lpResultDatagramDS = NULL;
                        
                        /*--------------------*/
                        /* ���X Result ��� */
                        if(GGD_FAILED(hr = GRSRQ_GET_METHOD(lpControlDS->pQueryResultQueue)->Get(lpControlDS->pQueryResultQueue, &lpResultDatagramDS, GSQ_ACCESS_METHOD_RELEASE)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Result ��C��C����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        else if(hr != GRSRQ_ACCESS_OK)
                          {
                            break;
                          }
                        
                        //GSJ_ADVANCE(lpControlDS->pSystemJournal, "Location(%d)=[%s]", ((GRSRILOCATIONDS *)lpResultDatagramDS)->iIdentifier, ((GRSRILOCATIONDS *)lpResultDatagramDS)->strCode);
                        
                        /*--------------------*/
                        /* Menu Item �O����Ŷ��t�m */
                        if((lpMenuItemDS = (GRSTMIITEMDS *)GGD_MEMORY_ALLOCATE(sizeof(GRSTMIITEMDS))) == NULL)
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ Menu Item �O����Ŷ��t�m�ɡA�O���餣���I���~�X�G0x%08X�C\n", GGD_ERROR_OUT_OF_MEMORY);
                            
                            return GGD_ERROR_OUT_OF_MEMORY;
                          }
                        
                        /*--------------------*/
                        /* ���� */
                        memset(lpMenuItemDS, 0, sizeof(GRSTMIITEMDS));
                        lpMenuItemDS->iIndex = iResultIndex;
                        lpMenuItemDS->iEnterMenuIndex = (-1);
                        lpMenuItemDS->iEscapeMenuIndex = GRSTMI_TRUNK_MENU_MAIN;
                        lpMenuItemDS->iEnterMode = GRSTMI_TRUNK_MENU_ENTER_MODE_SELECT;
                        lpMenuItemDS->iEntrainedDataValue = ((GRSRILOCATIONDS *)lpResultDatagramDS)->iIdentifier;
                        snprintf(lpMenuItemDS->strTitleName, GRSTMI_TRUNK_MENU_ITEM_TITLE_NAME_LENGTH_MAXIMUM, ((GRSRILOCATIONDS *)lpResultDatagramDS)->strCode);
                        lpMenuItemDS->iDisplayLine = 1;
                        
                        /*--------------------*/
                        /* �O�s */
                        if(GGD_FAILED(hr = GRSTMIQ_GET_METHOD(lpControlDS->pCurrentMenuItemQueue)->Add(lpControlDS->pCurrentMenuItemQueue, lpMenuItemDS)))
                          {
                            GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSTMenuItemQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                            
                            return hr;
                          }
                        
                        /*--------------------*/
                        /* Result ��ƧR���A�᭱�Τ���F */
                        GGD_MEMORY_RELEASE(lpResultDatagramDS);
                        
                        /*--------------------*/
                        /* Result ���޲֭p */
                        iResultIndex ++;
                      }
                    
                    /* ���A���B�J���� */
                    lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_REVERSION;
                  }
              }
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���~�o�� =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_ERROR_OCCURRED:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�o�Ϳ��~ */
            hResult = GGD_FALSE;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            int i;
            
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
            
            /* ���A�B�J��^ */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A����^ =- */
        case GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_REVERSION:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�w�g���� */
            hResult = GGD_OK;
            
            GRSDIDDS gddsBuzzerDatagramDS;
            int i;
            
            /*---------------------*/
            /* ��J Buzzer ��� */
            for(i = 0; i < 2; i ++)
              {
                memset(&gddsBuzzerDatagramDS, 0, sizeof(GRSDIDDS));
                gddsBuzzerDatagramDS.iControlCode = GRSBI_BUZZER_SOUND_SHORT_SHORT_BEEP;
                if(GGD_FAILED(hr = GRSDQ_GET_METHOD(lpControlDS->pBuzzerDatagramQueue)->Add(lpControlDS->pBuzzerDatagramQueue, &gddsBuzzerDatagramDS)))
                  {
                    GSJ_ERROR(lpControlDS->pSystemJournal, "�ާ@ GRSDatagramQueue ����ɡA�o�Ϳ��~�I���~�X�G0x%08X�C\n", hr);
                    
                    return hr;
                  }
              }
            
            /* ���A�B�J��^ */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY;
            
            break;
          }
        
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
        /* -= ���A���N�~ =- */
        default:
          {
            /* �^���W�h�޿豱��A�ثe�B�@�|������ */
            hResult = GGD_STILL_ACTIVE;
            
            /* ���A�B�J��^ */
            lpControlDS->iLoadResourceOperationFSMDStep = GRSP_TRUNK_LOAD_RESOURCE_FSMD_STEP_READY;
          }
      }
    
    return hResult;
  }

/*---------------------------------------------------------------------------*/
/* Query ���G�B�z�{�� */
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryResultProcessor(void *lpParameter, int iQueryCode, void *lpResultDatagramDS, int iResultDatagramDSLength)
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
/* Query �����B�z�{�� */
static GRESULT GRSRCALLBACK GRSPTrunkLoadResourceQueryCompleteProcessor(void *lpParameter, int iQueryCode, int iNumberOfRowsModified, int iResultDatagramDSAmount, GRESULT hResult)
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

