/*
 * @(#)$Id: ZebraCrossingDetection.cpp 790 2011-03-01 08:13:13Z zengzhou $
 * @(#)Lane detection
 *
 * (c)  PIONEER CORPORATION  2011
 * All Rights Reserved.
 */
#include<stdio.h>
#include "SDS_Detection.h"

#include "math.h"
/////////
//extern unsigned int p1, p2, p3, p4, p5, p6, p7, p8, p9, p10;
//#include <xdc/runtime/System.h>
//#ifdef _C66X_
#include <ti/mathlib/mathlib.h>
#include <ti/dsplib/dsplib.h>
#include <include/imglib64plus.h>
//#include <ti/imglib/imglib.h>
unsigned int g_coord[921600];

//#include <ti/vlib/vlib.h>
//#endif
//#include <xdc/runtime/Log.h>
//#include <ti/uia/events/UIABenchmark.h>
////////
DSP_HistParams g_Hist_Params;
//#pragma DATA_SECTION(g_Hist_Params, ".ALGMem")
int g_pixelcount;
//#pragma DATA_SECTION(g_pixelcount, ".ALGMem")
VLD_VehicleLicenseDetectionParams g_VLD_Params;

//#pragma DATA_SECTION(g_VLD_Params, ".ALGMem")
//MV_MATRIX_S L2_labelmap;
//#pragma DATA_SECTION(L2_labelmap, ".ALGMem")
//unsigned char L2_labelmapdata[40000];
//#pragma DATA_SECTION(L2_labelmapdata, ".ALGMem")
//SDSSendData m_pResultData;//����ṹ��

//#include "comdef.h"
SDS_DParams SDS_dParams =
{ 0.18333, 0.66667, 0.05, 0.95,
		30, //0.7
		20, 25, 5.0000, 5.0000, 5.0000, 5.0000, 5.0000, 5.0000, 0.2, 0.4, 100,
		4, 30000, 50, 10, 30, 10, 5.0, 5, 3.5, 40.0, 10, 20 };
VOID SDS_SetDynamicParam(SDS_DParams *pPrm)
{
	memcpy(&SDS_dParams, pPrm, sizeof(SDS_DParams));
}
/* constructor */
VOID VLD_CreateInstance()
{
	//=========Data definition=========//

	//==========Implementation==========//
#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_CreateInstance && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("[ZCD] ZCD_CreateInstance() Start.\r\n");
#endif
	//Initial some params
	g_VLD_Params.m_piPreProcessFrame = NULL;
	g_VLD_Params.m_piCompressFrame = NULL;
	g_VLD_Params.m_piCompressFrame_R = NULL;
	g_VLD_Params.m_piCompressFrame_G = NULL;
	g_VLD_Params.m_piCompressFrame_B = NULL;
	g_VLD_Params.m_pWorkBuffer = NULL;
	//g_VLD_Params.m_psAGData = NULL;
	//g_VLD_Params.m_psV_ED = NULL;
	//g_VLD_Params.m_psSubV_ED = NULL;
	//g_VLD_Params.m_psSub_Gray = NULL;
	/////////////////////////////////
	g_VLD_Params.m_psLabelMap = NULL;
	g_VLD_Params.m_sQueueArr.iDataNum = MVZERO;
	g_VLD_Params.m_sQueueArr.piBuffer = MVNULL;
	g_VLD_Params.m_sBoundingBoxList.iDataNum = MVZERO;
	g_VLD_Params.m_sBoundingBoxList.piBuffer = MVNULL;
	g_VLD_Params.m_sAreaList.iDataNum = MVZERO;
	g_VLD_Params.m_sAreaList.piBuffer = MVNULL;
	////////////////////////////////
	g_VLD_Params.m_sValidBlotList.piBuffer = MVNULL;
	g_VLD_Params.m_sValidBlotList.iDataNum = MVZERO;
	g_VLD_Params.m_sValidBlotList2.piBuffer = MVNULL;
	g_VLD_Params.m_sValidBlotList2.iDataNum = MVZERO;
	g_VLD_Params.m_sValidBlotList3.piBuffer = MVNULL;
	g_VLD_Params.m_sValidBlotList3.iDataNum = MVZERO;

	g_VLD_Params.m_sContainerCodeIndex.piBuffer = NULL;
	g_VLD_Params.m_sContainerCodeIndex.iDataNum = MVZERO;
	g_VLD_Params.m_sContainerCodeIndex.iRowNum = MVZERO;
	g_VLD_Params.m_sContainerCodeIndex.iColNum = MVZERO;
	g_VLD_Params.m_sLevel_Value.iDataNum = MVZERO;
	g_VLD_Params.m_sLevel_Value.piBuffer = MVNULL;

//	g_VLD_Params.m_sADAS_GP_Params = NULL;
	g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag = 0;

#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG
	//VLDDA_CreateInstance();
#endif
	g_VLD_Params.m_iRunState = ADAS_MODEL_STATE_CREATE;

#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_CreateInstance && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("[ZCD] ZCD_CreateInstance() End.\r\n");
#endif
}	//ZCD_CreateInstance

/* destructor */
VOID VLD_DestroyInstance(void)
{
	//=========Data definition=========//

	//==========Implementation==========//
#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_DestroyInstance && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("[ZCD] ZCD_DestroyInstance() Start.\r\n");
#endif

	if (TESTBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
			VLDCF_SYSTEM_INIT_STATE_FLAG) == FALSE)
		return;

#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG
	//VLDDA_DestroyInstance();
#endif

	if (g_VLD_Params.m_piPreProcessFrame != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piPreProcessFrame, 1);
		g_VLD_Params.m_piPreProcessFrame = NULL;
	}

	if (g_VLD_Params.m_piCompressFrame != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piCompressFrame, 1);
		g_VLD_Params.m_piCompressFrame = NULL;
	}

	if (g_VLD_Params.m_piCompressFrame_R != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piCompressFrame_R, 1);
		g_VLD_Params.m_piCompressFrame_R = NULL;
	}

	if (g_VLD_Params.m_piCompressFrame_G != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piCompressFrame_G, 1);
		g_VLD_Params.m_piCompressFrame_G = NULL;
	}

	if (g_VLD_Params.m_piCompressFrame_B != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piCompressFrame_B, 1);
		g_VLD_Params.m_piCompressFrame_B = NULL;
	}

	//if(g_VLD_Params.m_psV_ED != NULL)
	//{
	//	MV_FreeMatrices_S(g_VLD_Params.m_psV_ED, 1);
	//	g_VLD_Params.m_psV_ED = NULL;
	//}

	/*if(g_VLD_Params.m_psSubV_ED != NULL)
	 {
	 MV_FreeMatrices_S(g_VLD_Params.m_psSubV_ED, 1);
	 g_VLD_Params.m_psSubV_ED = NULL;
	 }*/

	/*if(g_VLD_Params.m_psSub_Gray != NULL)
	 {
	 MV_FreeMatrices_S(g_VLD_Params.m_psSub_Gray, 1);
	 g_VLD_Params.m_psSub_Gray = NULL;
	 }*/
	///////////////////////////////////////////////////
	if (g_VLD_Params.m_psLabelMap != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_psLabelMap, 1);
		g_VLD_Params.m_psLabelMap = NULL;
	}
	if (g_VLD_Params.m_sQueueArr.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sQueueArr.piBuffer);
		g_VLD_Params.m_sQueueArr.piBuffer = MVNULL;
		g_VLD_Params.m_sQueueArr.iDataNum = MVZERO;
	}
	if (g_VLD_Params.m_sBoundingBoxList.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sBoundingBoxList.piBuffer);
		g_VLD_Params.m_sBoundingBoxList.piBuffer = MVNULL;
		g_VLD_Params.m_sBoundingBoxList.iDataNum = MVZERO;
	}
	if (g_VLD_Params.m_sAreaList.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sAreaList.piBuffer);
		g_VLD_Params.m_sAreaList.piBuffer = MVNULL;
		g_VLD_Params.m_sAreaList.iDataNum = MVZERO;
	}
	//////////////////////////////////////////////////

	if (g_VLD_Params.m_sValidBlotList.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sValidBlotList.piBuffer);
		g_VLD_Params.m_sValidBlotList.piBuffer = MVNULL;
		g_VLD_Params.m_sValidBlotList.iDataNum = MVZERO;
	}

	if (g_VLD_Params.m_sValidBlotList2.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sValidBlotList2.piBuffer);
		g_VLD_Params.m_sValidBlotList2.piBuffer = MVNULL;
		g_VLD_Params.m_sValidBlotList2.iDataNum = MVZERO;
	}

	if (g_VLD_Params.m_sValidBlotList3.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sValidBlotList3.piBuffer);
		g_VLD_Params.m_sValidBlotList3.piBuffer = MVNULL;
		g_VLD_Params.m_sValidBlotList3.iDataNum = MVZERO;
	}

	if (g_VLD_Params.m_sContainerCodeIndex.piBuffer != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_sContainerCodeIndex.piBuffer, 1);
		g_VLD_Params.m_sContainerCodeIndex.piBuffer = NULL;
		g_VLD_Params.m_sContainerCodeIndex.iDataNum = MVZERO;
		g_VLD_Params.m_sContainerCodeIndex.iRowNum = MVZERO;
		g_VLD_Params.m_sContainerCodeIndex.iColNum = MVZERO;
	}

	if (g_VLD_Params.m_sLevel_Value.piBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_sLevel_Value.piBuffer);
		g_VLD_Params.m_sLevel_Value.piBuffer = MVNULL;
		g_VLD_Params.m_sLevel_Value.iDataNum = MVZERO;
	}

	if (g_VLD_Params.m_pWorkBuffer != NULL)
	{
		MV_Free(g_VLD_Params.m_pWorkBuffer);
		g_VLD_Params.m_pWorkBuffer = NULL;
	}

	/*if(g_VLD_Params.m_psAGData != NULL)
	 {
	 MV_FreeMatrices(g_VLD_Params.m_psAGData, 1);
	 g_VLD_Params.m_psAGData = NULL;
	 }*/

	if (g_VLD_Params.m_piTransImage != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piTransImage, 1);
		g_VLD_Params.m_piTransImage = NULL;
	}

	if (g_VLD_Params.m_piROIProcessFrame != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piROIProcessFrame, 1);
		g_VLD_Params.m_piROIProcessFrame = NULL;
	}

	if (g_VLD_Params.m_piInterProcessFrame != NULL)
	{
		MV_FreeMatrices_S(g_VLD_Params.m_piInterProcessFrame, 1);
		g_VLD_Params.m_piInterProcessFrame = NULL;
	}

	if (g_VLD_Params.SDSBlock != NULL)
	{
		MV_Free(g_VLD_Params.SDSBlock);
		g_VLD_Params.SDSBlock = NULL;

	}

	if (g_VLD_Params.fSpeed_Cir_x != NULL)
	{
		MV_Free(g_VLD_Params.fSpeed_Cir_x);
		g_VLD_Params.fSpeed_Cir_x = NULL;

	}
	if (g_VLD_Params.fSpeed_Cir_y != NULL)
	{
		MV_Free(g_VLD_Params.fSpeed_Cir_y);
		g_VLD_Params.fSpeed_Cir_y = NULL;

	}

	if (g_VLD_Params.BlobT != NULL)
	{
		MV_Free(g_VLD_Params.BlobT);
		g_VLD_Params.BlobT = NULL;

	}

	if (g_VLD_Params.Cir != NULL)
	{
		MV_Free(g_VLD_Params.Cir);
		g_VLD_Params.Cir = NULL;

	}

	if (g_VLD_Params.CirFlag != NULL)
	{
		MV_Free(g_VLD_Params.Cir);
		g_VLD_Params.CirFlag = NULL;
	}

//	g_VLD_Params.m_sADAS_GP_Params = NULL;

	//Set control flag
	g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag = 0;

	g_VLD_Params.m_iRunState = ADAS_MODEL_STATE_FINALIZE;

#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_DestroyInstance && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("[ZCD] ZCD_DestroyInstance() End.\r\n");
#endif

}	//ZCD_DestroyInstance

/* Set initial params */
VOID VLD_SetInitialParams(CameraParams *psCameraP)
{
	//=========Data definition=========//
	LONG iSize = MVZERO;

	//==========Implementation==========//
#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_SetInitialParams && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("ZCD_SetInitialParams() Start.\r\n");
#endif
#ifdef _C66X_
	DSP_InitHistogram(&g_Hist_Params);
	memset(g_coord, 0, sizeof(UINT) * 921600);
#endif
	//Initial some params
//	g_VLD_Params.m_sADAS_GP_Params = psADAS_GP_Params;

	//#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG
	//	VLDDA_Initialization();
	//#endif

	//Evaluate the vehicle detection ROI
	VLD_EvaluateDetectionROI(psCameraP,
			&g_VLD_Params.m_sVehicleLicenseDetParams);

	VLD_InitialDetRoiChart(psCameraP, &g_VLD_Params.m_sVehicleLicenseDetParams,
			&g_VLD_Params.m_sDetRoiChart);

	//Initial preprocessed frame
	g_VLD_Params.m_piPreProcessFrame =
			MV_CreateMatrices_S(
					g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iHeight,
					g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iWidth,
					1, 8, 8);

	//Initial compressed frame
	g_VLD_Params.m_piCompressFrame = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, 1, 8, 8); //����װ�ú�ʹ�ñ���

	g_VLD_Params.m_piCompressFrame_R = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);
	g_VLD_Params.m_piCompressFrame_G = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);
	g_VLD_Params.m_piCompressFrame_B = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);
	//Initial compressed frame
	//g_VLD_Params.m_psV_ED = MV_CreateMatrices_S(g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);

	//g_VLD_Params.m_psSubV_ED = MV_CreateMatrices_S(g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);

	//g_VLD_Params.m_psSub_Gray = MV_CreateMatrices_S(g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);
	////////////////////////////////////////////////////////////////////
	g_VLD_Params.m_psLabelMap = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, 1, 8, 8);

	g_VLD_Params.m_piTransImage = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, 1, 8, 8);//����װ�ú�ʹ�ñ���

	g_VLD_Params.m_piROIProcessFrame = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, 1, 8, 8);//����Ȥ����

	g_VLD_Params.m_piInterProcessFrame = MV_CreateMatrices_S(
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth,
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, 1, 8, 8);//����Ȥ����

	g_VLD_Params.m_sQueueArr.iDataNum =
			g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight
					* g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth;
	g_VLD_Params.m_sQueueArr.piBuffer = (INT*) malloc(
			sizeof(INT) * g_VLD_Params.m_sQueueArr.iDataNum);

	g_VLD_Params.m_sBoundingBoxList.iDataNum = 200;
	g_VLD_Params.m_sBoundingBoxList.piBuffer = (AdasDetROI*) malloc(
			sizeof(AdasDetROI) * g_VLD_Params.m_sBoundingBoxList.iDataNum);

	g_VLD_Params.m_sAreaList.iDataNum = 5000;
	g_VLD_Params.m_sAreaList.piBuffer = (INT*) malloc(
			sizeof(INT) * g_VLD_Params.m_sAreaList.iDataNum);
	/////////////////////////////////////////////////////////////////////
	g_VLD_Params.m_sValidBlotList.iDataNum = 5000;
	g_VLD_Params.m_sValidBlotList.piBuffer = (AdasDetROI*) malloc(
			sizeof(AdasDetROI) * g_VLD_Params.m_sValidBlotList.iDataNum);
	g_VLD_Params.m_sValidBlotList2.iDataNum = 5000;
	g_VLD_Params.m_sValidBlotList2.piBuffer = (AdasDetROI*) malloc(
			sizeof(AdasDetROI) * g_VLD_Params.m_sValidBlotList2.iDataNum);
	g_VLD_Params.m_sValidBlotList3.iDataNum = 5000;
	g_VLD_Params.m_sValidBlotList3.piBuffer = (AdasDetROI*) malloc(
			sizeof(AdasDetROI) * g_VLD_Params.m_sValidBlotList3.iDataNum);

	g_VLD_Params.m_sContainerCodeIndex.piBuffer = MV_CreateMatrices_S(10, 50, 1,
			8, 8);
	g_VLD_Params.m_sContainerCodeIndex.iColNum = 50;
	g_VLD_Params.m_sContainerCodeIndex.iRowNum = 10;
	g_VLD_Params.m_sLevel_Value.iDataNum = 10;
	g_VLD_Params.m_sLevel_Value.piBuffer = (INT*) malloc(
			sizeof(INT) * g_VLD_Params.m_sLevel_Value.iDataNum);

	//Initial integral image
	//g_VLD_Params.m_psAGData = MV_CreateMatrices(g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iHeight, g_VLD_Params.m_sDetRoiChart.sCompressGridSize.iWidth, 1, 8, 8);

	//Initial work buffer
	iSize =
			sizeof(MV_FIXED)
					* g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iWidth
					* g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iHeight;
	g_VLD_Params.m_pWorkBuffer = (BYTE*) MV_Malloc(iSize);

	g_VLD_Params.BlobT = (AdasBlobT*) malloc(sizeof(AdasBlobT) * 100);

	g_VLD_Params.Cir = (AdasCircleT*) malloc(sizeof(AdasCircleT) * 100);

	g_VLD_Params.SDSBlock = (AdasCircleT*) malloc(sizeof(AdasCircleT) * 5);

	g_VLD_Params.fSpeed_Cir_x = (FLOAT*) malloc(sizeof(FLOAT) * 5);

	g_VLD_Params.fSpeed_Cir_y = (FLOAT*) malloc(sizeof(FLOAT) * 5);

	g_VLD_Params.CirFlag = (INT*) malloc(sizeof(INT) * 3);

	g_VLD_Params.IsZeroSpeed = 0;

	g_VLD_Params.b_SearchTotalImage = 1;

	g_VLD_Params.nAbideNum = 100;

	//Set control flag
	SETBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
			VLDCF_SYSTEM_INIT_STATE_FLAG);

	g_VLD_Params.m_iRunState = ADAS_MODEL_STATE_INITIALIZE;

//	SDS_dParams.ROI_startX = 0.18333333;
//	SDS_dParams.ROI_endX = 0.666667;
//	SDS_dParams.ROI_startY = 0.0500;
//	SDS_dParams.ROI_endY = 0.9500;
//	SDS_dParams.f_RDifSideCir = 30.000000;
//	SDS_dParams.fCirGood_1 = 1.300000;
//	SDS_dParams.fCirGood_2 = 1.400000;
//	SDS_dParams.fCirGood_3 = 1.500000;
//	SDS_dParams.fCirWarning_1 = 1.500000;
//	SDS_dParams.fCirWarning_2 = 1.800000;
//	SDS_dParams.fCirWarning_3 = 1.900000;
//	SDS_dParams.nBiggestRaduis = 100;
//	SDS_dParams.nSmallestRaduis = 4;
//	SDS_dParams.Blot_BiggestArea = 50000;
	g_VLD_Params.nSaveImageMaxNum = 2;
	g_VLD_Params.UsedCircleNum = 3;

#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG
	//VLDDA_Initialization(&(g_VLD_Params.m_sDetRoiChart.sCompressGridSize));
#endif

#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_SetInitialParams && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("ZCD_SetInitialParams() End.\r\n");
#endif
}	//ZCD_SetInitialParams

/* Evaluate Detection ROI */
VOID VLD_EvaluateDetectionROI(CameraParams *psCameraP,
		VLD_VehicleLicenseDetParams* psVehicleLicenseDetParams)
{
	g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iY = 0;
	g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iHeight =
			psCameraP->iROI.iHeight;
	g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iX = 0;
	g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI.iWidth =
			psCameraP->iROI.iWidth;

}	//ZCD_EvaluateDetectionROI

/* Initial Detection ROI Chart */
VOID VLD_InitialDetRoiChart(CameraParams *psCameraP,
		VLD_VehicleLicenseDetParams* psVehicleLicenseDetParams,
		VLD_DetRoiChart *psDetRoiChart)
{
	//=========Data definition=========//

	//==========Implementation==========//
	//Initial Detection ROI Chart params
	psDetRoiChart->iDetRoiChartXStep = 1;
	psDetRoiChart->iDetRoiChartYStep = 1;

	//Set compress grid size
	psDetRoiChart->sCompressGridSize.iX = 0;
	psDetRoiChart->sCompressGridSize.iY = 0;
	psDetRoiChart->sCompressGridSize.iWidth =
			psVehicleLicenseDetParams->sVehicleLicenseDetROI.iWidth;
	psDetRoiChart->sCompressGridSize.iHeight =
			psVehicleLicenseDetParams->sVehicleLicenseDetROI.iHeight;

}	//ZCD_InitialDetRoiChart

/* Vehicle detection primarily process */
VOID VLD_DetectionProcess(CameraParams *psCameraP)
{

//	BOOL SearchTotalImage;
//	INT ret;
//	INT threshold = MVZERO;
//	AdasImageROI ROISet;
//	LPMV_MATRIX_S ClipImage;
	INT CirNum = MVZERO;
	INT nErrorCode = MVZERO;
	nErrorCode = nErrorCode;
	static INT nIndex_BadImage = 0;

	INT ImWidth = g_VLD_Params.m_piTransImage->iCols;
	INT ImHeight = g_VLD_Params.m_piTransImage->iRows;

	//=========Data definition=========//
//IplImage *pSubImage = MVNULL;
//pSubImage = cvCreateImage(cvSize(5, 6), IPL_DEPTH_8U, 1);
//cvReleaseImage(&pSubImage);
	//==========Implementation==========//
	//ADAS_DATA_ANALYSIS
#if ADAS_LOG_MODEL_VLD && ADAS_OS == ADAS_WIN32
	ADAS_LOG_ProcessInfo("\r\n-------ZCD_FRAME %d---(FAR)----\r\n", g_VLD_Params.m_iFrameIndex);

#endif

	//Initial some params
	if (TESTBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
			VLDCF_SYSTEM_INIT_STATE_FLAG) == FALSE)
	{
		return;
	}

	CLEARBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
			VLDCF_RESET_INIT_STATE_FLAG);

	//ADAS_DATA_ANALYSIS
//#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG && ADAS_DEBUG_VLD_DrawROI
//	/*VLDDA_DrawROI(&g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI, 
//		psCameraP, &g_VLD_Params.m_sDetRoiChart);*/
//	UTI_Image_Show_Dstt(g_VLD_Params.m_piPreProcessFrame,g_VLD_Params.m_iFrameIndex);
//#endif
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part1");

//	//ͼƬ���з�ת�����ں���������
//	ret = MV_ImageTrans_D2(g_VLD_Params.m_piPreProcessFrame,
//			g_VLD_Params.m_piTransImage);

//	DSP_mat_trans(g_VLD_Params.m_piPreProcessFrame->pDataAlign,720,1280,g_VLD_Params.m_piTransImage->pDataAlign);

//	int t5,t6;
//	t5 = Utils_getCurTimeInMsec();
//	edma_copy3D(g_VLD_Params.m_piTransImage->pDataAlign,g_VLD_Params.m_piPreProcessFrame->pDataAlign,0,0,1);
//
//	t6 = Utils_getCurTimeInMsec();
//	System_printf("edma copy 3d [%d]\n",t6-t5);
//#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG && ADAS_DEBUG_VLD_DrawROI
//	/*VLDDA_DrawROI(&g_VLD_Params.m_sVehicleLicenseDetParams.sVehicleLicenseDetROI, 
//		psCameraP, &g_VLD_Params.m_sDetRoiChart);*/
//	UTI_Image_Show_Dstt(ClipImage,g_VLD_Params.m_iFrameIndex);
//#endif
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part1");

	if (g_VLD_Params.b_SearchTotalImage == 1)
	{
//		Log_write1(UIABenchmark_start, (xdc_IArg) "detect circle ROI");
		g_VLD_Params.SDSDetROI.iStartX = SDS_dParams.ROI_startX;
		g_VLD_Params.SDSDetROI.iEndX = SDS_dParams.ROI_endX;
		g_VLD_Params.SDSDetROI.iStartY = SDS_dParams.ROI_startY;
		g_VLD_Params.SDSDetROI.iEndY = SDS_dParams.ROI_endY;
		CirNum = SDS_DetectCircleROIArea(g_VLD_Params.m_piTransImage,
				g_VLD_Params.m_piInterProcessFrame,
				g_VLD_Params.m_piROIProcessFrame, g_VLD_Params.m_psLabelMap,
				g_VLD_Params.SDSDetROI, &g_VLD_Params.m_sValidBlotList,
				&g_VLD_Params.m_sQueueArr, &g_VLD_Params.m_sBoundingBoxList,
				&g_VLD_Params.m_sAreaList, g_VLD_Params.CirFlag,
				g_VLD_Params.m_pWorkBuffer);
//		Log_write1(UIABenchmark_stop, (xdc_IArg) "detect circle ROI");
		if (CirNum != 3)
		{

			//CString Speed;
			//Speed.Format("CirNum:%f\n",fSpeed_Cir_x[1]);
			//TRACE(Speed);
		}

		switch (CirNum)
		{
			case -2:			//˵���߶ȳ�����
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_OVERHEIGHT;//
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case -1:			//˵����Դû�п�
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LIGHTZERO;	//������
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 0:
				nErrorCode = CirNum;
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LIGHTZERO;	//û�е�
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 1:
				nErrorCode = CirNum;
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LIGHTONE;//��⵽һ����
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 2:
				nErrorCode = CirNum;
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LIGHTTWO;//��⵽������
				g_VLD_Params.IsZeroSpeed = TRUE;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				break;
			case 3:
				if (g_VLD_Params.IsZeroSpeed)
				{
					g_VLD_Params.IsZeroSpeed = FALSE;
					g_VLD_Params.b_SearchTotalImage = TRUE;
					g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_OK;
				} else
				{
					//				CString TraceSpeed;
					//				float x = fSpeed_Cir_x[0];
					//				fSpeed_Cir_x[0]=fSpeed_Cir_x[0]*5;
					//				fSpeed_Cir_x[1]=fSpeed_Cir_x[1]*5;
					//				fSpeed_Cir_x[2]=fSpeed_Cir_x[2]*5;
					//				TraceSpeed.Format("TraceSpeed0:%f\n",TraceSpeed);
					//				TRACE(TraceSpeed);
					g_VLD_Params.b_SearchTotalImage = FALSE;		//���ٿ�ʼ(����OK)
				}
				//			b_SearchTotalImage = FALSE;//���ٿ�ʼ(����OK)
				break;
			case 4:
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LIGHTOVERTHREE;//more than 3 lamps
				nErrorCode = 4;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 9:
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LEFTBAD;//Right lamp bad
				nErrorCode = 9;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 10:
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_CENTERBAD;	//Middle lamp bad
				nErrorCode = 10;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 11:
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_RIGHTBAD;//Left lamp bad
				nErrorCode = 11;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 12:
				g_VLD_Params.m_pResultData.nStatusCode =
						ALG_SDS_BADRELATIONSHIP;//Bad relationship between 3 lamps
				nErrorCode = 12;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			case 15:
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LABELERROR;//Bad relationship between 3 lamps
				nErrorCode = 12;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
				g_VLD_Params.IsZeroSpeed = TRUE;
			case 255:
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_LABELLINGSTATE;//�궨״̬
				nErrorCode = 255;
				g_VLD_Params.IsZeroSpeed = TRUE;
				break;
			default:
				nErrorCode = 150;
				g_VLD_Params.m_pResultData.nStatusCode = ALG_SDS_DEFAULTERROR;//��������;
				g_VLD_Params.IsZeroSpeed = TRUE;
				//SaveBadImageFile(dstt,nErrorCode,nWarningCode);
		}

	} else
	{
		INT CirlceNum;
		INT ROINum;

		memcpy(&g_VLD_Params.Cir[0], &g_VLD_Params.SDSBlock[0],
				sizeof(AdasCircleT));
		memcpy(&g_VLD_Params.Cir[1], &g_VLD_Params.SDSBlock[1],
				sizeof(AdasCircleT));
		memcpy(&g_VLD_Params.Cir[2], &g_VLD_Params.SDSBlock[2],
				sizeof(AdasCircleT));
//		Log_write1(UIABenchmark_start, (xdc_IArg) "part2");
		CirlceNum = GetThreeCirlcleFromBlock(g_VLD_Params.m_piTransImage,
				g_VLD_Params.m_piInterProcessFrame, g_VLD_Params.m_psLabelMap,
				g_VLD_Params.SDSDetROI, &g_VLD_Params.m_sValidBlotList,
				&g_VLD_Params.m_sQueueArr, &g_VLD_Params.m_sBoundingBoxList,
				&g_VLD_Params.m_sAreaList, g_VLD_Params.m_pWorkBuffer);
//		Log_write1(UIABenchmark_stop, (xdc_IArg) "part2");

		ROINum = JudgeSDSCircle(g_VLD_Params.m_piTransImage, CirlceNum,
				g_VLD_Params.CirFlag);

		if (ROINum == -2)
		{
			g_VLD_Params.m_pResultData.nStatusCode = 5;
			ResetDefaultSetting();
		} else if (ROINum == 3)
		{
			//����������ʾ
			g_VLD_Params.m_pResultData.nStatusCode = 0;
			//gCir1_x= float(Cir[0].Cg_x);//int��Ϊfloat ��(Cir[0].Cg_x * 1000)ȥ��1000 cjs20120911
			//gCir1_y= float(Cir[0].Cg_y);
			//gCir2_x= float(Cir[1].Cg_x);
			//gCir2_y= float(Cir[1].Cg_y);
			//gCir3_x= float(Cir[2].Cg_x);
			//gCir3_y= float(Cir[2].Cg_y);

			g_VLD_Params.m_pResultData.Cir1_PixelX =
					(FLOAT) (g_VLD_Params.Cir[0].Cg_x);
			;			//int(Cir[0].Cg_x*1000);
			g_VLD_Params.m_pResultData.Cir1_PixelY =
					(FLOAT) (g_VLD_Params.Cir[0].Cg_y);	//int(Cir[0].Cg_y*1000);
			g_VLD_Params.m_pResultData.Cir2_PixelX =
					(FLOAT) (g_VLD_Params.Cir[1].Cg_x);	//int(Cir[1].Cg_x*1000);
			g_VLD_Params.m_pResultData.Cir2_PixelY =
					(FLOAT) (g_VLD_Params.Cir[1].Cg_y);	//int(Cir[1].Cg_y*1000);
			g_VLD_Params.m_pResultData.Cir3_PixelX =
					(FLOAT) (g_VLD_Params.Cir[2].Cg_x);	//int(Cir[2].Cg_x*1000);
			g_VLD_Params.m_pResultData.Cir3_PixelY =
					(FLOAT) (g_VLD_Params.Cir[2].Cg_y);	//int(Cir[2].Cg_y*1000);

			nIndex_BadImage = 0;
			CirNum = 3;
		} else
		{
			INT tAbideNum;
			//CvScalar col;
			//col.val[0]=255;

			nIndex_BadImage++;			//ͨ���жϱ߽��������ӳ�ʱ�䣬
			//�õ��������ֵ��������10��������50��1s��

			tAbideNum = GetAbideTime(ImWidth, ImHeight);
			if (tAbideNum == 5)
			{
				g_VLD_Params.m_pResultData.nWarningCode = 7;			//С�����¼�
				g_VLD_Params.m_pResultData.nStatusCode = 7;
			}
			if (tAbideNum == g_VLD_Params.nAbideNum)//chengjs20121015��50�ĵ�70Ŀ����Ϊ���ӳ�ʱ�䣬�����70֡����ͼ�񻹻ز������򱨴�,̫���������Բ
			{
				//for(int i=0;i<3;i++)
				//{
				//	//��Բ��Χ�Ŀ���
				//	cvLine(dstt,cvPoint(DrawBT[i].left,DrawBT[i].up),cvPoint(DrawBT[i].right,DrawBT[i].up),col,1,8,0);
				//	cvLine(dstt,cvPoint(DrawBT[i].left,DrawBT[i].low),cvPoint(DrawBT[i].right,DrawBT[i].low),col,1,8,0);
				//	cvLine(dstt,cvPoint(DrawBT[i].left,DrawBT[i].up),cvPoint(DrawBT[i].left,DrawBT[i].low),col,1,8,0);
				//	cvLine(dstt,cvPoint(DrawBT[i].right,DrawBT[i].up),cvPoint(DrawBT[i].right,DrawBT[i].low),col,1,8,0);

				//}
				g_VLD_Params.m_pResultData.nWarningCode = 8;			//������Χ����
				g_VLD_Params.m_pResultData.nStatusCode = 8;
			}

			if (nIndex_BadImage < tAbideNum)
			{
				int ii = 0;
				g_VLD_Params.fSpeed_Cir_x[0] = 0;
				g_VLD_Params.fSpeed_Cir_x[1] = 0;
				g_VLD_Params.fSpeed_Cir_x[2] = 0;
				g_VLD_Params.fSpeed_Cir_y[0] = 0;
				g_VLD_Params.fSpeed_Cir_y[1] = 0;
				g_VLD_Params.fSpeed_Cir_y[2] = 0;
				g_VLD_Params.m_pResultData.nStatusCode = 0;

				g_VLD_Params.m_pResultData.Cir1_PixelX =
						(FLOAT) (g_VLD_Params.Cir[0].Cg_x);
				;				//int(Cir[0].Cg_x*1000);
				g_VLD_Params.m_pResultData.Cir1_PixelY =
						(FLOAT) (g_VLD_Params.Cir[0].Cg_y);	//int(Cir[0].Cg_y*1000);
				g_VLD_Params.m_pResultData.Cir2_PixelX =
						(FLOAT) (g_VLD_Params.Cir[1].Cg_x);	//int(Cir[1].Cg_x*1000);
				g_VLD_Params.m_pResultData.Cir2_PixelY =
						(FLOAT) (g_VLD_Params.Cir[1].Cg_y);	//int(Cir[1].Cg_y*1000);
				g_VLD_Params.m_pResultData.Cir3_PixelX =
						(FLOAT) (g_VLD_Params.Cir[2].Cg_x);	//int(Cir[2].Cg_x*1000);
				g_VLD_Params.m_pResultData.Cir3_PixelY =
						(FLOAT) (g_VLD_Params.Cir[2].Cg_y);	//int(Cir[2].Cg_y*1000);
				CirNum = 3;
				//SaveBadImageFile(dstt,3,m_pResultData->nWarningCode);
				for (ii = 0; ii < 3; ii++)
				{
					//				SDSBlock[ii].bt.left = SDSBlock[ii].bt.left - 1;
					//				SDSBlock[ii].bt.right = SDSBlock[ii].bt.right + 1;
					//				SDSBlock[ii].bt.up = SDSBlock[ii].bt.up - 1;
					//				SDSBlock[ii].bt.low = SDSBlock[ii].bt.low + 1;
					g_VLD_Params.SDSBlock[ii].bt.left =
							g_VLD_Params.SDSBlock[ii].bt.left - 1;
					g_VLD_Params.SDSBlock[ii].bt.right =
							g_VLD_Params.SDSBlock[ii].bt.right + 1;
					g_VLD_Params.SDSBlock[ii].bt.up =
							g_VLD_Params.SDSBlock[ii].bt.up;
					g_VLD_Params.SDSBlock[ii].bt.low =
							g_VLD_Params.SDSBlock[ii].bt.low;
					if (g_VLD_Params.SDSBlock[ii].bt.left < 0)
					{
						g_VLD_Params.SDSBlock[ii].bt.left = 0;
					}
					if (g_VLD_Params.SDSBlock[ii].bt.right >= ImWidth)
					{
						g_VLD_Params.SDSBlock[ii].bt.right = ImWidth - 1;
					}
					if (g_VLD_Params.SDSBlock[ii].bt.up < 0)
					{
						g_VLD_Params.SDSBlock[ii].bt.up = 0;
					}
					if (g_VLD_Params.SDSBlock[ii].bt.low >= ImHeight)
					{
						g_VLD_Params.SDSBlock[ii].bt.low = ImHeight - 1;
					}

				}
			}

			else
			{
				CirNum = 0;
				//SaveBadImageFile(dstt,m_pResultData->nStatusCode,m_pResultData->nWarningCode);
				//				SaveBadImageFile(dstt,nErrorCode,nWarningCode);

				ResetDefaultSetting();
			}

		}

	}

	//������(��Ϣ)
	if (CirNum == 3)
	{
		//m_pResultData->nWarningCode = 0;
		if (g_VLD_Params.CirFlag[0] == 1)				//1�Ǿ����࣬2�������࣬0��good
		{
			g_VLD_Params.m_pResultData.nWarningCode =
					g_VLD_Params.m_pResultData.nWarningCode + 1;
			CirNum = CirNum + 30;
		}
		if (g_VLD_Params.CirFlag[1] == 1)
		{
			g_VLD_Params.m_pResultData.nWarningCode =
					g_VLD_Params.m_pResultData.nWarningCode + 10;
			CirNum = CirNum + 300;
		}
		if (g_VLD_Params.CirFlag[2] == 1)
		{
			g_VLD_Params.m_pResultData.nWarningCode =
					g_VLD_Params.m_pResultData.nWarningCode + 100;
			CirNum = CirNum + 3000;
		}
	}

//	Log_write1(UIABenchmark_start, (xdc_IArg) "part3");
	g_VLD_Params.m_pResultData.Cir1_PixelX = g_VLD_Params.Cir[0].Cg_x;//int(Cir[0].Cg_x*1000);
	g_VLD_Params.m_pResultData.Cir1_PixelY = g_VLD_Params.Cir[0].Cg_y;//int(Cir[0].Cg_y*1000);
	g_VLD_Params.m_pResultData.Cir2_PixelX = g_VLD_Params.Cir[1].Cg_x;//; int(Cir[1].Cg_x*1000);
	g_VLD_Params.m_pResultData.Cir2_PixelY = g_VLD_Params.Cir[1].Cg_y;//int(Cir[1].Cg_y*1000);
	g_VLD_Params.m_pResultData.Cir3_PixelX = g_VLD_Params.Cir[2].Cg_x;//int(Cir[2].Cg_x*1000);
	g_VLD_Params.m_pResultData.Cir3_PixelY = g_VLD_Params.Cir[2].Cg_y;//int(Cir[2].Cg_y*1000);
	//g_VLD_Params.m_pResultData.Cir4_PixelX = g_VLD_Params.Cir[3].Cg_x;//chengjs20140928 add
	//g_VLD_Params.m_pResultData.Cir4_PixelY = g_VLD_Params.Cir[3].Cg_y;//chengjs20140928 add
	//g_VLD_Params.m_pResultData.Cir5_PixelX = g_VLD_Params.Cir[4].Cg_x;//chengjs20140928 add
	//g_VLD_Params.m_pResultData.Cir5_PixelY = g_VLD_Params.Cir[4].Cg_y;//chengjs20140928 add

	g_VLD_Params.m_pResultData.GCir1_PixelX =
			(FLOAT) (g_VLD_Params.Cir[0].Cg_x);
	g_VLD_Params.m_pResultData.GCir1_PixelY =
			(FLOAT) (g_VLD_Params.Cir[0].Cg_y);
	g_VLD_Params.m_pResultData.GCir2_PixelX =
			(FLOAT) (g_VLD_Params.Cir[1].Cg_x);
	g_VLD_Params.m_pResultData.GCir2_PixelY =
			(FLOAT) (g_VLD_Params.Cir[1].Cg_y);
	g_VLD_Params.m_pResultData.GCir3_PixelX =
			(FLOAT) (g_VLD_Params.Cir[2].Cg_x);
	g_VLD_Params.m_pResultData.GCir3_PixelY =
			(FLOAT) (g_VLD_Params.Cir[2].Cg_y);
	//g_VLD_Params.m_pResultData.GCir4_PixelX = (FLOAT)(g_VLD_Params.Cir[3].Cg_x);
	//g_VLD_Params.m_pResultData.GCir4_PixelY = (FLOAT)(g_VLD_Params.Cir[3].Cg_y);
	//g_VLD_Params.m_pResultData.GCir5_PixelX = (FLOAT)(g_VLD_Params.Cir[4].Cg_x);
	//g_VLD_Params.m_pResultData.GCir5_PixelY = (FLOAT)(g_VLD_Params.Cir[4].Cg_y);

	g_VLD_Params.m_pResultData.CCir1_PixelX =
			(FLOAT) (g_VLD_Params.Cir[0].Cir_x);
	g_VLD_Params.m_pResultData.CCir1_PixelY =
			(FLOAT) (g_VLD_Params.Cir[0].Cir_y);
	g_VLD_Params.m_pResultData.CCir2_PixelX =
			(FLOAT) (g_VLD_Params.Cir[1].Cir_x);
	g_VLD_Params.m_pResultData.CCir2_PixelY =
			(FLOAT) (g_VLD_Params.Cir[1].Cir_y);
	g_VLD_Params.m_pResultData.CCir3_PixelX =
			(FLOAT) (g_VLD_Params.Cir[2].Cir_x);
	g_VLD_Params.m_pResultData.CCir3_PixelY =
			(FLOAT) (g_VLD_Params.Cir[2].Cir_y);

	//g_VLD_Params.m_pResultData.CCir4_PixelX = (FLOAT)(g_VLD_Params.Cir[3].Cir_x);
	//g_VLD_Params.m_pResultData.CCir4_PixelY = (FLOAT)(g_VLD_Params.Cir[3].Cir_y);
	//g_VLD_Params.m_pResultData.CCir5_PixelX = (FLOAT)(g_VLD_Params.Cir[4].Cir_x);
	//g_VLD_Params.m_pResultData.CCir5_PixelY = (FLOAT)(g_VLD_Params.Cir[4].Cir_y);

	g_VLD_Params.m_pResultData.Cir1_r = (FLOAT) (g_VLD_Params.Cir[0].Cir_r);
	g_VLD_Params.m_pResultData.Cir2_r = (FLOAT) (g_VLD_Params.Cir[1].Cir_r);
	g_VLD_Params.m_pResultData.Cir3_r = (FLOAT) (g_VLD_Params.Cir[2].Cir_r);
	//g_VLD_Params.m_pResultData.Cir4_r = (FLOAT)(g_VLD_Params.Cir[3].Cir_r);
	//g_VLD_Params.m_pResultData.Cir5_r = (FLOAT)(g_VLD_Params.Cir[4].Cir_r);

	g_VLD_Params.m_pResultData.Cir1_f = (FLOAT) (g_VLD_Params.Cir[0].Cir_f);
	g_VLD_Params.m_pResultData.Cir2_f = (FLOAT) (g_VLD_Params.Cir[1].Cir_f);
	g_VLD_Params.m_pResultData.Cir3_f = (FLOAT) (g_VLD_Params.Cir[2].Cir_f);
	//g_VLD_Params.m_pResultData.Cir4_f = (FLOAT)(g_VLD_Params.Cir[3].Cir_f);
	//g_VLD_Params.m_pResultData.Cir5_f = (FLOAT)(g_VLD_Params.Cir[4].Cir_f);

	unsigned _RoiSize = sizeof(AdasBlobT);
	memcpy(&(g_VLD_Params.m_pResultData.Cir1_bt), &(g_VLD_Params.Cir[0].bt),
			_RoiSize);
	memcpy(&(g_VLD_Params.m_pResultData.Cir2_bt), &(g_VLD_Params.Cir[1].bt),
			_RoiSize);
	memcpy(&(g_VLD_Params.m_pResultData.Cir3_bt), &(g_VLD_Params.Cir[2].bt),
			_RoiSize);
	memcpy(&(g_VLD_Params.m_pResultData.Cir4_bt), &(g_VLD_Params.Cir[3].bt),
			_RoiSize);
	memcpy(&(g_VLD_Params.m_pResultData.Cir5_bt), &(g_VLD_Params.Cir[4].bt),
			_RoiSize);

	/*m_pResultData.Cir1_Total255 = g_VLD_Params.Cir[0].bt.Total255;
	 m_pResultData.Cir2_Total255 = g_VLD_Params.Cir[1].bt.Total255;
	 m_pResultData.Cir3_Total255 = g_VLD_Params.Cir[2].bt.Total255;
	 m_pResultData.Cir4_Total255 = g_VLD_Params.Cir[3].bt.Total255;
	 m_pResultData.Cir5_Total255 = g_VLD_Params.Cir[4].bt.Total255;*/

	//m_pResultData.ErrorIndex = nIndex;
	//return CirNum;
	g_VLD_Params.m_iRunState = ADAS_MODEL_STATE_ANALYZE;
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part3");

}	//ZCD_DetectionProcess

//��ͼ��ת��90�ȡ���������Բ��
int MV_ImageTrans_D(LPMV_MATRIX_S psV_EDS, LPMV_MATRIX_S psV_EDD)
{

	INT iRow = 0;
	INT iCol = 0;

	INT iWidth1 = MVZERO;
	INT iHight1 = MVZERO;

//	INT iWidth2 = MVZERO;
//	INT iHight2 = MVZERO;

	iHight1 = psV_EDS->iRows;
	iWidth1 = psV_EDS->iCols;

//	iHight2 = psV_EDD->iRows;
//	iWidth2 = psV_EDD->iCols;

	if (!psV_EDS || !psV_EDD)
	{
		return (-1);
	}

	if (psV_EDD->iRows != iWidth1 || psV_EDD->iCols != iHight1)
	{
		return (-1);
	}

	for (iRow = 0; iRow < iHight1; iRow++)
	{
		//DOUBLE *dtmp = &(pdA[iI*iN]);
		MV_FIXED_S *dtmp = ((MV_FIXED_S*) (((BYTE*) psV_EDS->pDataAlign)
				+ iRow * psV_EDS->iPitch) + iCol);

		INT iIndex = 0;
		for (iCol = 0; iCol < iWidth1; iCol++)
		{
			//pdB[iIndex]= (*dtmp);
			MV_FIXED_S *dtmpd;
			dtmpd = ((MV_FIXED_S*) (((BYTE*) psV_EDD->pDataAlign)
					+ iIndex * psV_EDD->iPitch) + iRow);
			*(dtmpd) = *((MV_FIXED_S*) (dtmp));
			iIndex += 1;
			dtmp++;
		}
	}

	return (0);

}

//����2.ת��90��.���Դ��ĺ���
int MV_ImageTrans_D2(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg)
{

	INT iRow = 0;
	INT iCol = 0;

	INT iWidth1 = MVZERO;
	INT iHight1 = MVZERO;

//	INT iWidth2 = MVZERO;

//	INT iHight2 = MVZERO;

	MV_FIXED_S *pSrcLineAddr;

	MV_FIXED_S *pDstLineAddr;

	iHight1 = SrcImg->iRows;
	iWidth1 = SrcImg->iCols;

//	iHight2 = DesImg->iRows;
//	iWidth2 = DesImg->iCols;

	if (!SrcImg || !DesImg)
	{
		return (-1);
	}

	if (DesImg->iRows != iWidth1 || DesImg->iCols != iHight1)
	{
		return (-1);
	}

	for (iRow = 0; iRow < iHight1; iRow++)
	{
#ifdef _C66X_
		pSrcLineAddr = ((MV_FIXED_S*) (((BYTE*) SrcImg->pDataAlign)
				+ iRow * SrcImg->iPitch) + 0);

#else
		pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, iRow, 0);
#endif
		for (iCol = 0; iCol < iWidth1; iCol++)
		{
#ifdef _C66X_
			pDstLineAddr = ((MV_FIXED_S*) (((BYTE*) DesImg->pDataAlign)
					+ iCol * DesImg->iPitch) + iRow);
#else
			pDstLineAddr = MV_MatrixGetLinePtr_S(DesImg, iCol, iRow);
#endif
			pDstLineAddr[0] = pSrcLineAddr[iCol];

		}
	}

	return (0);

}

//ͼ���¡
void CloneImage(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg)
{
	memcpy(DesImg->pData, SrcImg->pData, SrcImg->iLength);

}

//		 void  Copy(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,AdasDetROI ROI)
//{
//
//	int m = 0;
//	int n = 0;
//
//	MV_FIXED_S *pSrcLineAddr;
//	MV_FIXED_S *pDstLineAddr;
//
//	//ROI.iStartX=40;
//	//ROI.iEndX=60;
//	//ROI.iStartY=40;
//	//ROI.iEndY=60;
//
//
//	 DesImg->iRows =  ROI.iEndY- ROI.iStartY;
//	 DesImg->iCols = ROI.iEndX- ROI.iStartX;
//	 DesImg->iPitch = MV_ALIGN(DesImg->iCols * sizeof(MV_FIXED_S), DesImg->iLineAddrAlign);
//	 DesImg->iLength = DesImg->iPitch * DesImg->iRows;
//
//
//		//����ROI��Χ����ͼ��
//		for(n = ROI.iStartY; n < ROI.iEndY; n++)
//		{
//				pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, n, 0);
//				pDstLineAddr = MV_MatrixGetLinePtr_S(DesImg, (n-ROI.iStartY), 0);
//			for(m = ROI.iStartX; m < ROI.iEndX; m++)
//			{
//				pDstLineAddr[m-ROI.iStartX]=pSrcLineAddr[m];
//				//pDstImage->imageData[n* pSrcImage->roi->width + m] = pSrcImage->imageData[pSrcImage->roi->xOffset + (pSrcImage->roi->yOffset - n) * pSrcImage->width + m];
//				//pDstImage->imageDataOrigin[n* pSrcImage->roi->width + m] = pSrcImage->imageDataOrigin[pSrcImage->roi->xOffset + (pSrcImage->roi->yOffset - n) * pSrcImage->width + m];
//			}
//		}
//
//
//
//
//	//pDstImage->imageSize = pDstImage->width * pDstImage->height;
//
//	//pDstImage->widthStep = pDstImage->width;
//
//
//}

int SDS_DetectCircleROIArea(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,
		LPMV_MATRIX_S RoiImg, LPMV_MATRIX_S psLabelMap,
		AdasDetROI_Float DetectROI, BufferParams_DetROI* psValidBlotList,
		BufferParams_Int* psQueueArr, BufferParams_DetROI* psBoundingBox,
		BufferParams_Int* psArea, INT *CirFlag, BYTE * pWorkBuffer)
{

//	INT SearchLeft_X;
//	INT SearchRight_X;
//	INT CenterCircleTop_Y;
//	INT CenterCircleLow_Y;
	INT SearchTop_Y;
	INT SearchLow_Y;
	INT CenterCircleLeft_X;
	INT CenterCircleRight_X;
	AdasImageROI ROI;
	LPMV_MATRIX_S ClipImage;
	LPMV_MATRIX_S LabelImage;
	LPMV_MATRIX_S IntImage;

	INT Num = MVZERO;
	INT ImWidth = SrcImg->iCols;
	INT ImHeight = SrcImg->iRows;
	INT threshold;
	INT i = MVZERO;
	INT count = MVZERO;
	INT CirlceNum;
	INT UsedCircleNum;	//chengjs20140928 add
	INT ROINum;

	INT FilterNum;

	UsedCircleNum = g_VLD_Params.UsedCircleNum;

	SearchTop_Y = (INT) (ImHeight * DetectROI.iStartY);/*ImWidth/6;*///�������������4�ı���
	SearchLow_Y = (INT) (ImHeight * DetectROI.iEndY);/*9*ImWidth/10;*/	//��������
	CenterCircleLeft_X = (INT) (ImWidth * DetectROI.iStartX);/*ImHeight/3;*///����Բ�жϱ�׼
	CenterCircleRight_X = (INT) (ImWidth * DetectROI.iEndX);/*2*ImHeight/3;*///����Բ�жϱ�׼advised by mao at 7.6  448---360
	CenterCircleLeft_X = CenterCircleLeft_X;
	CenterCircleRight_X = CenterCircleRight_X;
	ROI.iX = 0;
	ROI.iY = SearchTop_Y;
	ROI.iHeight = ((SearchLow_Y - SearchTop_Y) / DATA_ALIGN) * DATA_ALIGN
			+ DATA_ALIGN;
	ROI.iWidth = ImWidth;

	//����ͼ��

	//CloneImage(psV_ED,copyImg);

	ClipImage = DesImg;

	ClipImage->iRows = ROI.iHeight;
	ClipImage->iCols = ROI.iWidth;
	ClipImage->iPitch = MV_ALIGN(ClipImage->iCols * sizeof(MV_FIXED_S),
			ClipImage->iLineAddrAlign);
	ClipImage->iLength = ClipImage->iPitch * ClipImage->iRows;

	LabelImage = psLabelMap;

	LabelImage->iRows = ROI.iHeight;
	LabelImage->iCols = ROI.iWidth;
	LabelImage->iPitch = MV_ALIGN(LabelImage->iCols * sizeof(MV_FIXED_S),
			LabelImage->iLineAddrAlign);
	LabelImage->iLength = LabelImage->iPitch * LabelImage->iRows;

	//��������Ȥ����ͼ��
	UTI_GetROIMatrix(SrcImg, &(ROI), 1, ClipImage);
//	System_printf("%d, %d\n",ClipImage->iRows,ClipImage->iCols);
	//�������������ֵ����Сֵ

	//UTI_Image_Show_Dstt(ClipImage,1);

	//��ȡ��ֵ
	threshold = GetOtsuThreshold(ClipImage);

	//��ֵ��
	Threshold0(ClipImage, threshold, g_coord);

	//���ǩ
//	Log_write1(UIABenchmark_start, (xdc_IArg) "first labeling");
	VLD_Sub_C_BWLabel_SDS0(ClipImage, LabelImage, LabelImage, psValidBlotList,
			psQueueArr, psBoundingBox, psArea, pWorkBuffer);
///	Log_write1(UIABenchmark_stop, (xdc_IArg) "first labeling");
	Num = psBoundingBox->iColNum + 1;	//ע��˴���Ҫ��1

	//ԭͼ����ʹ��

	IntImage = RoiImg;
	/*IntImage->iRows =  ROI.iHeight;
	 IntImage->iCols =  ROI.iWidth;
	 IntImage->iPitch = MV_ALIGN(IntImage->iCols * sizeof(MV_FIXED_S), IntImage->iLineAddrAlign);
	 IntImage->iLength = IntImage->iPitch * IntImage->iRows;*/
	if (Num == 0)
		return 15;
	if (Num == 1)
	{
		return 0;		//û�е�
	}
	if (Num > 100)
	{
		return 4;	//����������
	}
	for (i = 0; i < Num; i++)
	{
		//if (BT[i].area>19443)  //QC
		//if (BT[i].area>8800)
		if (g_VLD_Params.BlobT[i].area > SDS_dParams.Blot_BiggestArea)	//�����߹���
		{
			return -2;
		}
	}

#ifdef _C66X_
#pragma MUST_ITERATE(3,3)
#endif
	for (i = 0; i < Num - 1; i++)
	{
		//TRACE("nSearchLeft_X = %d\n",nSearchLeft_X);
		g_VLD_Params.BlobT[i].up = g_VLD_Params.BlobT[i].up + SearchTop_Y;
		g_VLD_Params.BlobT[i].low = g_VLD_Params.BlobT[i].low + SearchTop_Y;

	}
	count = RecogniseLamp(Num);		///������Բ��������Cir ����ͳ����

	if (count > 10)		//������һ���Բ
	{
		return 4;		//maybe there are no circle
	}

	//UTI_Image_Show_Dstt(ClipImage,1);

	//��ͼƬ�ٴ��ȥ
	// UTI_GetROIMatrix(SrcImg, &(ROI), 1, ClipImage);

	CirlceNum = GetCirlcleFromBlock(SrcImg, IntImage, LabelImage, DetectROI,
			psValidBlotList, psQueueArr, psBoundingBox, psArea, pWorkBuffer,
			count);

	FilterNum = FilterCir(count);	//����

	if (FilterNum < 3)
	{
		return FilterNum;
	}
	if (FilterNum == 7)
	{

		MaopaoSortFrom7();
		return 255;
	} else
	{
		SortLabel(FilterNum);
		FilterNum = JudgeSDSCircle(SrcImg, FilterNum, CirFlag);	//���� ͨ����ϵ�ж�,���Ҫ�ж��ĸ�Բ,���������Ӧ����
	}
	return FilterNum;

	// UTI_Image_Show_Dstt(ClipImage,1);

}

int GetOtsuThreshold(LPMV_MATRIX_S SrcImg)
{
//	Log_write1(UIABenchmark_start, (xdc_IArg) "get threshold part1");
	//chengjs20151118�ر�˵����GrayScale�����ǻҶȼ����ڻҶ�ͼ���и�ֵΪ256���ʱ�������256����ʾGrayScale

	INT width = SrcImg->iCols;
	INT height = SrcImg->iRows;
	unsigned short pixelCount[256];
	FLOAT pixelPro[256];

	INT i = MVZERO;
	INT j = MVZERO;
	INT pixelSum = width * height;
	INT threshold = MVZERO;
	unsigned char ret = 0;
	FLOAT w0 = MVZERO;
	FLOAT w1 = MVZERO;
	FLOAT u0tmp = MVZERO;
	FLOAT u1tmp = MVZERO;
	FLOAT u0 = MVZERO;
	FLOAT u1 = MVZERO;
	FLOAT u = MVZERO;

	FLOAT deltaTmp = MVZERO;
	FLOAT deltaMax = MVZERO;
	//BYTE* data = (uchar*)pInData->imageData;
	MV_FIXED_S *pSrcLineAddr;
#ifdef _C66X_
//	FLOAT w = MVZERO;
//	INT count0=MVZERO;
//	INT luma0=MVZERO;
//	INT count1=MVZERO;
//	INT luma1=MVZERO;
//	pixelSum*=pixelSum;
#pragma UNROLL(256)
#endif
	for (i = 0; i < 256; i++)
	{
		pixelCount[i] = 0;
		pixelPro[i] = 0;
	}

	//ͳ�ƻҶȼ���ÿ������������ͼ���еĸ���
#ifdef _C66X_
	pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, 0, 0);
	memset(g_Hist_Params.H1, 0, sizeof(short) * 256);
	memset(g_Hist_Params.H2, 0, sizeof(short) * 256);
	memset(g_Hist_Params.H3, 0, sizeof(short) * 256);
	VLIB_histogram_1D_U8(pSrcLineAddr, height * width, 256, 1,
			g_Hist_Params.histarray, g_Hist_Params.H1, g_Hist_Params.H2,
			g_Hist_Params.H3, pixelCount);

#else
	for(i = 0; i < height; i++)
	{
		pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, i, 0);
		for(j = 0;j < width;j++)
		{
			pixelCount[pSrcLineAddr[j]]++;
		}
	}
#endif
	//����ÿ������������ͼ���еı���
#ifdef _C66X_
	for (i = 0; i < 256; i++)
	{
		pixelPro[i] = divsp_c(pixelCount[i], pixelSum);
	}
#else
	for (i = 0; i < 256; i++)
	{

//#ifdef _C66X_
//		pixelPro[i] = divsp_c(pixelCount[i], pixelSum);
//#else
		pixelPro[i] = (float)pixelCount[i] / pixelSum;
//#endif
	}
#endif

//	Log_write1(UIABenchmark_stop, (xdc_IArg) "get threshold part1");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "get threshold part2");
	//�����Ҷȼ�[0,255]
#ifdef _C66X_
	for (i = 0; i < 256; i++)
	{
		w0 = 0.0f;
		w1 = 0.0f;
		u0tmp = 0.0f;
		u1tmp = 0.0f;
		u0 = 0.0f;
		u1 = 0.0f;
		u = 0.0f;
		deltaTmp = 0.0f;

		for (j = 0; j < 256; j++)
		{
			if (j <= i)	//��������
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			} else	//ǰ������
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}

		u0 = divsp_c(u0tmp, w0);
		u1 = divsp_c(u1tmp, w1);

		u = u0tmp + u1tmp;
//		deltaTmp =
//		w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
		deltaTmp = w0 * w1 * powsp(u0 - u1, 2);

		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}
//	for (i = 0; i < 256; i++)
//	{
//		w0 = 0.0f;
//		w1 = 0.0f;
//		count0=0;
//		count1=0;
//		luma0=0;
//		luma1=0;
//		u0 = 0.0f;
//		u1 = 0.0f;
//		deltaTmp = 0.0f;
//
//		for (j = 0; j < 256; j++)
//		{
//			if (j <= i)	//��������
//			{
//				count0 += pixelCount[j];
//				luma0 += j * pixelCount[j];
//			} else	//ǰ������
//			{
//				count1 += pixelCount[j];
//				luma1 += j * pixelCount[j];
//			}
//		}
//		w=divsp_c((float)(count0*count1),(float)pixelSum);
//		u0=divsp_c((float)luma0,(float)count0);
//		u1=divsp_c((float)luma1,(float)count1);
//		deltaTmp = w*powsp(u0-u1,2);
//		if (deltaTmp > deltaMax)
//		{
//			deltaMax = deltaTmp;
//			threshold = i;
//		}
//	}
#else
	for (i = 0; i < 256; i++)
	{
		w0 = 0.0f;
		w1 = 0.0f;
		u0tmp = 0.0f;
		u1tmp = 0.0f;
		u0 = 0.0f;
		u1 = 0.0f;
		u = 0.0f;
		deltaTmp = 0.0f;

		for (j = 0; j < 256; j++)
		{
			if (j <= i)	//��������
			{
				w0 += pixelPro[j];
				u0tmp += j * pixelPro[j];
			} else	//ǰ������
			{
				w1 += pixelPro[j];
				u1tmp += j * pixelPro[j];
			}
		}

		u0 = u0tmp / w0;
		u1 = u1tmp / w1;

		u = u0tmp + u1tmp;
		deltaTmp =
		w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);

		if (deltaTmp > deltaMax)
		{
			deltaMax = deltaTmp;
			threshold = i;
		}
	}
#endif

//	Log_write1(UIABenchmark_stop, (xdc_IArg) "get threshold part2");
	return threshold;
}

//Threshold
//src  ��Ҫ���ж�ֵ����ͼ��
//
void Threshold0(LPMV_MATRIX_S SrcImg, UINT threshold, UINT* coord)
{
//	Log_write1(UIABenchmark_start, (xdc_IArg) "threshold");
	INT iImgHeight = MVZERO;
	INT iImgWidth = MVZERO;
//	INT iIndex = MVZERO;
	INT iRow = MVZERO;
	INT iCol = MVZERO;
	MV_FIXED_S *pSrcLineAddr;
	//MV_FIXED_S *pDstLineAddr;

	//MV_FIXED_S pSRCLineAddr_S = MVZERO;
	//MV_FIXED_S pDSTineAddr_S = MVZERO;

	iImgHeight = SrcImg->iRows;
	iImgWidth = SrcImg->iCols;

	// ȡͼ���еĵڶ��ַ���
#ifdef _C66X_

	pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, 0, 0);
	for (iRow = 0; iRow < iImgHeight * iImgWidth; iRow++)
	{
		if (pSrcLineAddr[iRow] > threshold)
		{
			pSrcLineAddr[iRow] = 255;
		} else
			pSrcLineAddr[iRow] = 0;
	}
	int sum;
	DSP_boundary_8(pSrcLineAddr, iImgHeight, iImgWidth, coord, &sum);
	g_pixelcount = sum;
//	System_printf("the pixel num is %d\n",g_pixelcount);
#else
	for (iRow = 0; iRow < iImgHeight; iRow++)
	{
		pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, iRow, 0);
//		pDstLineAddr = MV_MatrixGetLinePtr_S(dst, iRow, 0);
		for (iCol = 0; iCol < iImgWidth; iCol++)
		{
			if (pSrcLineAddr[iCol]>threshold)
			pSrcLineAddr[iCol]=255;
			else
			pSrcLineAddr[iCol]=0;

		}
	}

#endif
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "threshold");
}
void Threshold(LPMV_MATRIX_S SrcImg, UINT threshold)
{
//	Log_write1(UIABenchmark_start, (xdc_IArg) "threshold");
	INT iImgHeight = MVZERO;
	INT iImgWidth = MVZERO;
//	INT iIndex = MVZERO;
	INT iRow = MVZERO;
	INT iCol = MVZERO;
	MV_FIXED_S *pSrcLineAddr;
	//MV_FIXED_S *pDstLineAddr;

	//MV_FIXED_S pSRCLineAddr_S = MVZERO;
	//MV_FIXED_S pDSTineAddr_S = MVZERO;

	iImgHeight = SrcImg->iRows;
	iImgWidth = SrcImg->iCols;

	// ȡͼ���еĵڶ��ַ���
#ifdef _C66X_

	pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, 0, 0);
	for (iRow = 0; iRow < iImgHeight * iImgWidth; iRow++)
	{
		if (pSrcLineAddr[iRow] > threshold)
		{
			pSrcLineAddr[iRow] = 255;
		} else
			pSrcLineAddr[iRow] = 0;
	}
//	int sum;
//	DSP_boundary_8(pSrcLineAddr,iImgHeight,iImgWidth,coord,&sum);
//	g_pixelcount=sum;
//	System_printf("the pixel num is %d\n",g_pixelcount);
#else
	for (iRow = 0; iRow < iImgHeight; iRow++)
	{
		pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, iRow, 0);
//		pDstLineAddr = MV_MatrixGetLinePtr_S(dst, iRow, 0);
		for (iCol = 0; iCol < iImgWidth; iCol++)
		{
			if (pSrcLineAddr[iCol]>=threshold)
			pSrcLineAddr[iCol]=255;
			else
			pSrcLineAddr[iCol]=0;

		}
	}

#endif
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "threshold");
}
//���ǩ
VOID VLD_Sub_C_BWLabel_SDS0(LPMV_MATRIX_S psV_ED, LPMV_MATRIX_S psLabelMap,
		LPMV_MATRIX_S pchAGData, BufferParams_DetROI* psValidBlotList,
		BufferParams_Int* psQueueArr, BufferParams_DetROI* psBoundingBox,
		BufferParams_Int* psArea, BYTE * pWorkBuffer)
{
//	Log_write1(UIABenchmark_start, (xdc_IArg) "connecting label");
	//=========Data definition=========//
	INT iImgHeight = MVZERO;
	INT iImgWidth = MVZERO;
	INT iIndex = MVZERO;
	INT iRow = MVZERO;
	INT iCol = MVZERO;
	MV_FIXED_S *pSrcLineAddr_S = MVNULL;
	MV_FIXED_S iValue_Source = MVZERO;
	MV_FIXED_S iValue_Labelmap = MVZERO;
	INT iLabelIndex = MVZERO;
	INT iQueue_Head = MVZERO;
	INT iQueue_Tail = MVZERO;
	INT iQueue = MVZERO;
	INT iLeft = MVZERO;
	INT iRight = MVZERO;
	INT iTop = MVZERO;
	INT iBottom = MVZERO;
	INT iRow2 = MVZERO;
	INT iCol2 = MVZERO;
	MV_FIXED_S *pVDLineAddr_S = MVNULL;
	MV_FIXED_S *pLMLineAddr_S = MVNULL;
	INT iDot = MVZERO;
	INT iDot_Flage = MVZERO;
	INT iDot_Value = MVZERO;
//	INT i_Mod = MVZERO;
//	INT i_Ceil = MVZERO;
//	INT iArea_Val = MVZERO;
	INT iValidNum = MVZERO;
	//rough selection
	INT iHight_Max = MVZERO;
	INT iHight_Min = MVZERO;
	INT iWidth_Max = MVZERO;
	INT iWidth_Min = MVZERO;
	INT iArea_Max = MVZERO;
	INT iArea_Min = MVZERO;
//	INT iWidth = MVZERO;
//	INT iHight = MVZERO;

	//==========Implementation==========//
	iImgWidth = psV_ED->iCols;
	iImgHeight = psV_ED->iRows;
	iValidNum = 0;
	//rough selection
	iHight_Max = 78;
	iHight_Min = 35;
	iWidth_Max = 60;
	iWidth_Min = 5;
	iArea_Max = 2700;
	iArea_Min = 55;

	iLabelIndex = 0;
	iQueue_Tail = -1;
	//initial the matrix
//	p3 = Utils_getCurTimeInUsec();
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part1 label");
#ifdef _C66X_
	pSrcLineAddr_S = MV_MatrixGetLinePtr_S(psLabelMap, 0, 0);
	memset(pSrcLineAddr_S, 0, (iImgHeight * iImgWidth) * sizeof(char));
//	for (iRow = 0; iRow < iImgHeight * iImgWidth; iRow++)
//	{
//		pSrcLineAddr_S[iRow] = 0;
//	}
#else
	for (iRow = 0; iRow < iImgHeight; iRow++)
	{
		pSrcLineAddr_S = MV_MatrixGetLinePtr_S(psLabelMap, iRow, 0);
		for (iCol = 0; iCol< iImgWidth; iCol++)
		{
			pSrcLineAddr_S[iCol] = 0;
		}
	}
#endif
	//	for (iIndex = 0; iIndex < psQueueArr->iDataNum; iIndex++)
	//	{
	//		psQueueArr->piBuffer[iIndex] = -1;
	//	}
	//
	//	for (iIndex = 0; iIndex < psBoundingBox->iDataNum; iIndex++)
	//	{
	//		psBoundingBox->piBuffer[iIndex].iStartX = -1;
	//		psBoundingBox->piBuffer[iIndex].iEndX = -1;
	//		psBoundingBox->piBuffer[iIndex].iStartY = -1;
	//		psBoundingBox->piBuffer[iIndex].iEndY = -1;
	//		psBoundingBox->piBuffer[iIndex].iValid = 0;
	//
	//		psValidBlotList->piBuffer[iIndex].iStartX = -1;
	//		psValidBlotList->piBuffer[iIndex].iEndX = -1;
	//		psValidBlotList->piBuffer[iIndex].iStartY = -1;
	//		psValidBlotList->piBuffer[iIndex].iEndY = -1;
	//		psValidBlotList->piBuffer[iIndex].iValid = 0;
	//
	//		psArea->piBuffer[iIndex] = 0;
	//	}
	/*2016.11.3��ǩ̫���ʵ�����*/
//	memset(psQueueArr->piBuffer, -1, 50 * sizeof(INT));
//	for (iIndex = 0; iIndex < 50; iIndex++)
//	{
//		psQueueArr->piBuffer[iIndex] = -1;
//	}
	for (iIndex = 0; iIndex < 50; iIndex++)
	{
		psBoundingBox->piBuffer[iIndex].iStartX = -1;
		psBoundingBox->piBuffer[iIndex].iEndX = -1;
		psBoundingBox->piBuffer[iIndex].iStartY = -1;
		psBoundingBox->piBuffer[iIndex].iEndY = -1;
		psBoundingBox->piBuffer[iIndex].iValid = 0;

//		psValidBlotList->piBuffer[iIndex].iStartX = -1;
//		psValidBlotList->piBuffer[iIndex].iEndX = -1;
//		psValidBlotList->piBuffer[iIndex].iStartY = -1;
//		psValidBlotList->piBuffer[iIndex].iEndY = -1;
//		psValidBlotList->piBuffer[iIndex].iValid = 0;

		psArea->piBuffer[iIndex] = 0;
	}
	int i;
	iValue_Source = *(MV_FIXED_S*) ((BYTE*) psV_ED->pDataAlign);
	iValue_Labelmap = *(MV_FIXED_S*) ((BYTE*) psLabelMap->pDataAlign);
	MV_FIXED_S *pSource = MVNULL;
	MV_FIXED_S *pLabelmap = MVNULL;
	pSource = (MV_FIXED_S*) ((BYTE*) psV_ED->pDataAlign);
	pLabelmap = (MV_FIXED_S*) ((BYTE*) psLabelMap->pDataAlign);
	for (i = 0; i < 400; i++)
	{
		psQueueArr->piBuffer[i] = 0;
	}
	INT* labellist;
	labellist = &psQueueArr->piBuffer[0];
	INT* pcirnum;
	pcirnum = &psQueueArr->piBuffer[200];

	int index;
	for (i = 0; i < g_pixelcount; i++)
	{
		index = g_coord[i];
		iTop = g_coord[i] - iImgWidth;
		iLeft = g_coord[i] - 1;
		if (iTop <= iImgWidth || iLeft % iImgWidth == (iImgWidth - 1))
			continue;
		if (pLabelmap[iTop] > 0 && pLabelmap[iLeft] > 0)
		{
			pLabelmap[index] =
					pLabelmap[iTop] > pLabelmap[iLeft] ?
							pLabelmap[iLeft] : pLabelmap[iTop];
			if (labellist[pLabelmap[iTop]] > labellist[pLabelmap[iLeft]])
				labellist[pLabelmap[iTop]] = labellist[pLabelmap[iLeft]];
			else
				labellist[pLabelmap[iLeft]] = labellist[pLabelmap[iTop]];
		} else if (pLabelmap[iTop] > 0 || pLabelmap[iLeft] > 0)
		{
			pLabelmap[index] =
					pLabelmap[iTop] > 0 ? pLabelmap[iTop] : pLabelmap[iLeft];
		} else
		{
			iLabelIndex++;
			pLabelmap[index] = iLabelIndex;
			labellist[iLabelIndex] = iLabelIndex;
		}
	}

	int cirnum = 0;
	for (i = 1; i < iLabelIndex + 1; i++)
	{
		if (i == labellist[i])
		{
			cirnum++;
			pcirnum[i] = cirnum;
		}
		labellist[i] = pcirnum[labellist[i]];
	}
	if (iLabelIndex >= psBoundingBox->iDataNum)
	{
		psBoundingBox->iColNum = -1;
		return;
	}
	//for (i = 1; i < iLabelIndex + 1; i++)
	//{
	//	printf("the %d label run is %d\n", i, labellist[i]);
	//}
//	for (i = 0; i < iImgHeight * iImgWidth; i++)
//	{
//		pLabelmap[i] = labellist[pLabelmap[i]];
//	}
	int areaID;
	for (i = 0; i < g_pixelcount; i++)
	{
		areaID = pLabelmap[g_coord[i]] = labellist[pLabelmap[g_coord[i]]];
		psArea->piBuffer[areaID - 1]++;
		iRow = g_coord[i] / iImgWidth;
		iCol = g_coord[i] % iImgWidth;
		if (psBoundingBox->piBuffer[areaID - 1].iStartY == -1)
		{
			psBoundingBox->piBuffer[areaID - 1].iStartX = iCol;
			psBoundingBox->piBuffer[areaID - 1].iEndX = iCol;
			psBoundingBox->piBuffer[areaID - 1].iStartY = iRow;
			psBoundingBox->piBuffer[areaID - 1].iEndY = iRow;
			psBoundingBox->piBuffer[areaID - 1].iValid = 1;
		}
		if (iCol < psBoundingBox->piBuffer[areaID - 1].iStartX)
			psBoundingBox->piBuffer[areaID - 1].iStartX = iCol;
		else if (iCol > psBoundingBox->piBuffer[areaID - 1].iEndX)
			psBoundingBox->piBuffer[areaID - 1].iEndX = iCol;
		if (iRow > psBoundingBox->piBuffer[areaID - 1].iEndY)
			psBoundingBox->piBuffer[areaID - 1].iEndY = iRow;

	}

	psBoundingBox->iColNum = cirnum;
//	for (iRow = 0; iRow < iImgHeight; iRow++)
//	{
//		for (iCol = 0; iCol < iImgWidth; iCol++)
//		{
//			iValue_Labelmap = *((MV_FIXED_S*) (((BYTE*) psLabelMap->pDataAlign)
//					+ iRow * psLabelMap->iPitch) + iCol);
//			if (iValue_Labelmap > 0)
//			{
//				psArea->piBuffer[iValue_Labelmap - 1] =
//						psArea->piBuffer[iValue_Labelmap - 1] + 1;
//				if (psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartY == -1)
//				{
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartX = iCol;
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartY = iRow;
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iValid = 1;
//				}
//				if (iCol < psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartX)
//				{
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartX = iCol;
//				}
//
//			}
//		}
//	}
//	/*2016.11.3 �޸�ɨ�跽ʽ������ɨ�裬��֤CACHE����*/
////#ifdef _C66X_
////#pragma MUST_ITERATE(4, ,4)
////#endif
//	for (iRow = iImgHeight-1; iRow >= 0; iRow--)
//	{
//		for (iCol = iImgWidth - 1; iCol >= 0; iCol--)
//		{
//			iValue_Labelmap = *((MV_FIXED_S*) (((BYTE*) psLabelMap->pDataAlign)
//					+ iRow * psLabelMap->iPitch) + iCol);
//			if (iValue_Labelmap > 0)
//			{
//				if (psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY == -1)
//				{
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX = iCol;
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY = iRow;
//				}
//				if (iCol > psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX)
//				{
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX = iCol;
//				}
//
//			}
//		}
//	}

	for (iIndex = 0; iIndex < psBoundingBox->iColNum; iIndex++)
	{
		g_VLD_Params.BlobT[iIndex].id = iIndex + 1;
		g_VLD_Params.BlobT[iIndex].left =
				psBoundingBox->piBuffer[iIndex].iStartX;
		g_VLD_Params.BlobT[iIndex].right =
				psBoundingBox->piBuffer[iIndex].iEndX;
		g_VLD_Params.BlobT[iIndex].up = psBoundingBox->piBuffer[iIndex].iStartY;
		g_VLD_Params.BlobT[iIndex].low = psBoundingBox->piBuffer[iIndex].iEndY;
		g_VLD_Params.BlobT[iIndex].area = psArea->piBuffer[iIndex];
//		System_printf("left:%d,right:%d,up:%d,low:%d\n",g_VLD_Params.BlobT[iIndex].left,g_VLD_Params.BlobT[iIndex].right,g_VLD_Params.BlobT[iIndex].up,g_VLD_Params.BlobT[iIndex].low);
	}
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part4 label");
//	p6 = Utils_getCurTimeInUsec();
//	System_printf("the Label part 1 cost is %dus\n",p4-p3);
//	System_printf("the Label part 2 cost is %dus\n",p5-p4);
//	System_printf("the Label part 3 cost is %dus\n",p6-p5);

	///////////////the stage of delate too small blot////////////////////
	/*for (iIndex = 0; iIndex < iLabelIndex; iIndex++)
	 {
	 iArea_Val = psArea->piBuffer[iIndex];
	 iWidth = psBoundingBox->piBuffer[iIndex].iEndX - psBoundingBox->piBuffer[iIndex].iStartX + 1;
	 iHight = psBoundingBox->piBuffer[iIndex].iEndY - psBoundingBox->piBuffer[iIndex].iStartY + 1;

	 if (iArea_Val < iArea_Min || iArea_Val > iArea_Max || iWidth > iWidth_Max || iWidth < iWidth_Min || iHight > iHight_Max || iHight < iHight_Min)
	 {
	 psBoundingBox->piBuffer[iIndex].iValid = 0;

	 for (iRow=psBoundingBox->piBuffer[iIndex].iStartY;iRow<=psBoundingBox->piBuffer[iIndex].iEndY;iRow++)
	 {
	 pSrcLineAddr_S=MV_MatrixGetLinePtr_S(psV_ED,iRow,0);
	 for (iCol=psBoundingBox->piBuffer[iIndex].iStartX;iCol<=psBoundingBox->piBuffer[iIndex].iEndX;iCol++)
	 {
	 iValue_Labelmap = *((MV_FIXED_S*)(((BYTE*)psLabelMap->pDataAlign) + iRow * psLabelMap->iPitch) + iCol);
	 if (iValue_Labelmap == (iIndex + 1))
	 {
	 pSrcLineAddr_S[iCol] = 0;
	 }
	 }
	 }
	 }
	 if (psBoundingBox->piBuffer[iIndex].iValid == 1)
	 {
	 psValidBlotList->piBuffer[iValidNum].iStartX=psBoundingBox->piBuffer[iIndex].iStartX;
	 psValidBlotList->piBuffer[iValidNum].iStartY=psBoundingBox->piBuffer[iIndex].iStartY;
	 psValidBlotList->piBuffer[iValidNum].iEndX=psBoundingBox->piBuffer[iIndex].iEndX;
	 psValidBlotList->piBuffer[iValidNum].iEndY=psBoundingBox->piBuffer[iIndex].iEndY;
	 psValidBlotList->piBuffer[iValidNum].iValid=1;

	 psBoundingBox->piBuffer[iValidNum].iStartX=psBoundingBox->piBuffer[iIndex].iStartX;
	 psBoundingBox->piBuffer[iValidNum].iStartY=psBoundingBox->piBuffer[iIndex].iStartY;
	 psBoundingBox->piBuffer[iValidNum].iEndX=psBoundingBox->piBuffer[iIndex].iEndX;
	 psBoundingBox->piBuffer[iValidNum].iEndY=psBoundingBox->piBuffer[iIndex].iEndY;
	 psBoundingBox->piBuffer[iValidNum].iValid=1;
	 iValidNum++;
	 }
	 }
	 psValidBlotList->iDataNum = iValidNum;
	 psValidBlotList->iColNum = iValidNum;
	 psBoundingBox->iColNum=iValidNum;*/
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "connecting label");
#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG && ADAS_OS == ADAS_WIN32 && ADAS_DEBUG_VLD_Save_C_LabelResult1
	//VLDDA_Save_C_LabelResult4(psValidBlotList, psV_ED, g_VLD_Params.m_iFrameIndex);
#endif

}	//VLD_Sub_C_BWLabel1
VOID VLD_Sub_C_BWLabel_SDS(LPMV_MATRIX_S psV_ED, LPMV_MATRIX_S psLabelMap,
		LPMV_MATRIX_S pchAGData, BufferParams_DetROI* psValidBlotList,
		BufferParams_Int* psQueueArr, BufferParams_DetROI* psBoundingBox,
		BufferParams_Int* psArea, BYTE * pWorkBuffer)
{
//	Log_write1(UIABenchmark_start, (xdc_IArg) "connecting label");
	//=========Data definition=========//
	INT iImgHeight = MVZERO;
	INT iImgWidth = MVZERO;
	INT iIndex = MVZERO;
	INT iRow = MVZERO;
	INT iCol = MVZERO;
	MV_FIXED_S *pSrcLineAddr_S = MVNULL;
	MV_FIXED_S iValue_Source = MVZERO;
	MV_FIXED_S iValue_Labelmap = MVZERO;
	INT iLabelIndex = MVZERO;
	INT iQueue_Head = MVZERO;
	INT iQueue_Tail = MVZERO;
	INT iQueue = MVZERO;
	INT iLeft = MVZERO;
	INT iRight = MVZERO;
	INT iTop = MVZERO;
	INT iBottom = MVZERO;
	INT iRow2 = MVZERO;
	INT iCol2 = MVZERO;
	MV_FIXED_S *pVDLineAddr_S = MVNULL;
	MV_FIXED_S *pLMLineAddr_S = MVNULL;
	INT iDot = MVZERO;
	INT iDot_Flage = MVZERO;
	INT iDot_Value = MVZERO;
//	INT i_Mod = MVZERO;
//	INT i_Ceil = MVZERO;
//	INT iArea_Val = MVZERO;
	INT iValidNum = MVZERO;
	//rough selection
	INT iHight_Max = MVZERO;
	INT iHight_Min = MVZERO;
	INT iWidth_Max = MVZERO;
	INT iWidth_Min = MVZERO;
	INT iArea_Max = MVZERO;
	INT iArea_Min = MVZERO;
//	INT iWidth = MVZERO;
//	INT iHight = MVZERO;

	//==========Implementation==========//
	iImgWidth = psV_ED->iCols;
	iImgHeight = psV_ED->iRows;
//	System_printf("the width is:%d, the height is :%d",iImgWidth,iImgHeight);
	iValidNum = 0;
	//rough selection
	iHight_Max = 78;
	iHight_Min = 35;
	iWidth_Max = 60;
	iWidth_Min = 5;
	iArea_Max = 2700;
	iArea_Min = 55;

	iLabelIndex = 0;
	iQueue_Tail = -1;
	//initial the matrix
//	p3 = Utils_getCurTimeInUsec();
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part0 label");
#ifdef _C66X_
	pSrcLineAddr_S = MV_MatrixGetLinePtr_S(psLabelMap, 0, 0);
	memset(pSrcLineAddr_S, 0, (iImgHeight * iImgWidth) * sizeof(char));
//	for (iRow = 0; iRow < iImgHeight * iImgWidth; iRow++)
//	{
//		pSrcLineAddr_S[iRow] = 0;
//	}
#else
	for (iRow = 0; iRow < iImgHeight; iRow++)
	{
		pSrcLineAddr_S = MV_MatrixGetLinePtr_S(psLabelMap, iRow, 0);
		for (iCol = 0; iCol< iImgWidth; iCol++)
		{
			pSrcLineAddr_S[iCol] = 0;
		}
	}
#endif
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part0 label");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part1 label");
	//	for (iIndex = 0; iIndex < psQueueArr->iDataNum; iIndex++)
	//	{
	//		psQueueArr->piBuffer[iIndex] = -1;
	//	}
	//
	//	for (iIndex = 0; iIndex < psBoundingBox->iDataNum; iIndex++)
	//	{
	//		psBoundingBox->piBuffer[iIndex].iStartX = -1;
	//		psBoundingBox->piBuffer[iIndex].iEndX = -1;
	//		psBoundingBox->piBuffer[iIndex].iStartY = -1;
	//		psBoundingBox->piBuffer[iIndex].iEndY = -1;
	//		psBoundingBox->piBuffer[iIndex].iValid = 0;
	//
	//		psValidBlotList->piBuffer[iIndex].iStartX = -1;
	//		psValidBlotList->piBuffer[iIndex].iEndX = -1;
	//		psValidBlotList->piBuffer[iIndex].iStartY = -1;
	//		psValidBlotList->piBuffer[iIndex].iEndY = -1;
	//		psValidBlotList->piBuffer[iIndex].iValid = 0;
	//
	//		psArea->piBuffer[iIndex] = 0;
	//	}
	/*2016.11.3��ǩ̫���ʵ�����*/
//	memset(psQueueArr->piBuffer, -1, 50 * sizeof(INT));
//	for (iIndex = 0; iIndex < 50; iIndex++)
//	{
//		psQueueArr->piBuffer[iIndex] = -1;
//	}
	for (iIndex = 0; iIndex < 50; iIndex++)
	{
		psBoundingBox->piBuffer[iIndex].iStartX = -1;
		psBoundingBox->piBuffer[iIndex].iEndX = -1;
		psBoundingBox->piBuffer[iIndex].iStartY = -1;
		psBoundingBox->piBuffer[iIndex].iEndY = -1;
		psBoundingBox->piBuffer[iIndex].iValid = 0;

//		psValidBlotList->piBuffer[iIndex].iStartX = -1;
//		psValidBlotList->piBuffer[iIndex].iEndX = -1;
//		psValidBlotList->piBuffer[iIndex].iStartY = -1;
//		psValidBlotList->piBuffer[iIndex].iEndY = -1;
//		psValidBlotList->piBuffer[iIndex].iValid = 0;

		psArea->piBuffer[iIndex] = 0;
	}
	int i;
	iValue_Source = *(MV_FIXED_S*) ((BYTE*) psV_ED->pDataAlign);
	iValue_Labelmap = *(MV_FIXED_S*) ((BYTE*) psLabelMap->pDataAlign);
	MV_FIXED_S *pSource = MVNULL;
	MV_FIXED_S *pLabelmap = MVNULL;
	pSource = (MV_FIXED_S*) ((BYTE*) psV_ED->pDataAlign);
	pLabelmap = (MV_FIXED_S*) ((BYTE*) psLabelMap->pDataAlign);
	for (i = 0; i < 400; i++)
	{
		psQueueArr->piBuffer[i] = 0;
	}
	INT* labellist;
	labellist = &psQueueArr->piBuffer[0];
	INT* pcirnum;
	pcirnum = &psQueueArr->piBuffer[200];

	int index;
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part1 label");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part2 label");
	if (pSource[0] == 255)
	{
		iLabelIndex++;
		pLabelmap[0] = iLabelIndex;
		labellist[iLabelIndex] = iLabelIndex;
	}
	for (i = 1; i < iImgWidth; i++)
	{
		if (pSource[i] == 255)
		{
			if (pSource[i - 1] == 255)
			{
				pLabelmap[i] = iLabelIndex;
			} else
			{
				iLabelIndex++;
				pLabelmap[i] = iLabelIndex;
				labellist[iLabelIndex] = iLabelIndex;
			}
		}
	}
	for (iRow = 1; iRow < iImgHeight; iRow++)
	{
		if (pSource[iRow * iImgWidth] == 255)
		{
			if (pSource[(iRow - 1) * iImgWidth] == 255)
				pLabelmap[iRow * iImgWidth] = pLabelmap[(iRow - 1) * iImgWidth];
			else
			{
				iLabelIndex++;
				pLabelmap[iRow * iImgWidth] = iLabelIndex;
				labellist[iLabelIndex] = iLabelIndex;

			}
		}
		for (iCol = 1; iCol < iImgWidth; iCol++)
		{
			index = iRow * iImgWidth + iCol;
			if (pSource[index] == 255)
			{

				iTop = pSource[index - iImgWidth];
				iLeft = pSource[index - 1];
				if (iTop == 255 && iLeft == 255)
				{
					if (labellist[pLabelmap[index - iImgWidth]]
							> labellist[pLabelmap[index - 1]])
					{
						pLabelmap[index] = pLabelmap[index - 1];
						labellist[pLabelmap[index - iImgWidth]] =
								labellist[pLabelmap[index - 1]];
					} else
					{
						pLabelmap[index] = pLabelmap[index - iImgWidth];
						labellist[pLabelmap[index - 1]] =
								labellist[pLabelmap[index - iImgWidth]];
					}

				} else if (iTop == 255 || iLeft == 255)
				{
					pLabelmap[index] =
							pLabelmap[index - iImgWidth] > 0 ?
									pLabelmap[index - iImgWidth] :
									pLabelmap[index - 1];
				} else
				{
					iLabelIndex++;
					pLabelmap[index] = iLabelIndex;
					labellist[iLabelIndex] = iLabelIndex;
				}
			}
		}
	}
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part2 label");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part3 label");
	int cirnum = 0;
	for (i = 1; i < iLabelIndex + 1; i++)
	{
		if (i == labellist[i])
		{
			cirnum++;
			pcirnum[i] = cirnum;
		}
		labellist[i] = pcirnum[labellist[i]];
	}
	if (iLabelIndex >= psBoundingBox->iDataNum)
	{
		psBoundingBox->iColNum = -1;
		return;
	}
	//for (i = 1; i < iLabelIndex + 1; i++)
	//{
	//	printf("the %d label run is %d\n", i, labellist[i]);
	//}
	psBoundingBox->iColNum = cirnum;
	int areaID;
	for (i = 0; i < iImgHeight * iImgWidth; i++)
	{
		areaID = pLabelmap[i] = labellist[pLabelmap[i]];
		psArea->piBuffer[areaID - 1]++;
	}

//	psBoundingBox->iColNum = iLabelIndex;

	//	for (iCol = 0; iCol < iImgWidth; iCol++)
	//	{
	//		for (iRow = 0; iRow < iImgHeight; iRow++)
	//		{
	//			iValue_Labelmap = *((MV_FIXED_S*) (((BYTE*) psLabelMap->pDataAlign)
	//					+ iRow * psLabelMap->iPitch) + iCol);
	//			if (iValue_Labelmap > 0)
	//			{
	//				psArea->piBuffer[iValue_Labelmap - 1] =
	//						psArea->piBuffer[iValue_Labelmap - 1] + 1;
	//				if (psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartX == -1)
	//				{
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartX = iCol;
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartY = iRow;
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iValid = 1;
	//				}
	//				if (iRow < psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartY)
	//				{
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iStartY = iRow;
	//				}
	//			}
	//		}
	//	}
	/*2016.11.3 �޸�ɨ�跽ʽ������ɨ�裬��֤CACHE����*/
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part3 label");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part4 label");
	//	for (iCol = iImgWidth - 1; iCol >= 0; iCol--)
	//	{
	//		for (iRow = iImgHeight - 1; iRow >= 0; iRow--)
	//		{
	//			iValue_Labelmap = *((MV_FIXED_S*) (((BYTE*) psLabelMap->pDataAlign)
	//					+ iRow * psLabelMap->iPitch) + iCol);
	//			if (iValue_Labelmap > 0)
	//			{
	//				if (psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX == -1)
	//				{
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX = iCol;
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY = iRow;
	//				}
	//				if (iRow > psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY)
	//				{
	//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY = iRow;
	//				}
	//			}
	//		}
	//	}
	/*2016.11.3 �޸�ɨ�跽ʽ������ɨ�裬��֤CACHE����*/
//#ifdef _C66X_
//#pragma MUST_ITERATE(4, ,4)
//#endif
//	for (iRow = iImgHeight - 1; iRow >= 0; iRow--)
//	{
//		for (iCol = iImgWidth - 1; iCol >= 0; iCol--)
//		{
//			iValue_Labelmap = *((MV_FIXED_S*) (((BYTE*) psLabelMap->pDataAlign)
//					+ iRow * psLabelMap->iPitch) + iCol);
//			if (iValue_Labelmap > 0)
//			{
//				if (psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY == -1)
//				{
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX = iCol;
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndY = iRow;
//				}
//				if (iCol > psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX)
//				{
//					psBoundingBox->piBuffer[iValue_Labelmap - 1].iEndX = iCol;
//				}
//
//			}
//		}
//	}
	for (iIndex = 0; iIndex < psBoundingBox->iColNum; iIndex++)
	{
		g_VLD_Params.BlobT[iIndex].id = iIndex + 1;
		g_VLD_Params.BlobT[iIndex].area = psArea->piBuffer[iIndex];
	}
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part4 label");
//	p6 = Utils_getCurTimeInUsec();
//	System_printf("the Label part 1 cost is %dus\n",p4-p3);
//	System_printf("the Label part 2 cost is %dus\n",p5-p4);
//	System_printf("the Label part 3 cost is %dus\n",p6-p5);

	///////////////the stage of delate too small blot////////////////////
	/*for (iIndex = 0; iIndex < iLabelIndex; iIndex++)
	 {
	 iArea_Val = psArea->piBuffer[iIndex];
	 iWidth = psBoundingBox->piBuffer[iIndex].iEndX - psBoundingBox->piBuffer[iIndex].iStartX + 1;
	 iHight = psBoundingBox->piBuffer[iIndex].iEndY - psBoundingBox->piBuffer[iIndex].iStartY + 1;

	 if (iArea_Val < iArea_Min || iArea_Val > iArea_Max || iWidth > iWidth_Max || iWidth < iWidth_Min || iHight > iHight_Max || iHight < iHight_Min)
	 {
	 psBoundingBox->piBuffer[iIndex].iValid = 0;

	 for (iRow=psBoundingBox->piBuffer[iIndex].iStartY;iRow<=psBoundingBox->piBuffer[iIndex].iEndY;iRow++)
	 {
	 pSrcLineAddr_S=MV_MatrixGetLinePtr_S(psV_ED,iRow,0);
	 for (iCol=psBoundingBox->piBuffer[iIndex].iStartX;iCol<=psBoundingBox->piBuffer[iIndex].iEndX;iCol++)
	 {
	 iValue_Labelmap = *((MV_FIXED_S*)(((BYTE*)psLabelMap->pDataAlign) + iRow * psLabelMap->iPitch) + iCol);
	 if (iValue_Labelmap == (iIndex + 1))
	 {
	 pSrcLineAddr_S[iCol] = 0;
	 }
	 }
	 }
	 }
	 if (psBoundingBox->piBuffer[iIndex].iValid == 1)
	 {
	 psValidBlotList->piBuffer[iValidNum].iStartX=psBoundingBox->piBuffer[iIndex].iStartX;
	 psValidBlotList->piBuffer[iValidNum].iStartY=psBoundingBox->piBuffer[iIndex].iStartY;
	 psValidBlotList->piBuffer[iValidNum].iEndX=psBoundingBox->piBuffer[iIndex].iEndX;
	 psValidBlotList->piBuffer[iValidNum].iEndY=psBoundingBox->piBuffer[iIndex].iEndY;
	 psValidBlotList->piBuffer[iValidNum].iValid=1;

	 psBoundingBox->piBuffer[iValidNum].iStartX=psBoundingBox->piBuffer[iIndex].iStartX;
	 psBoundingBox->piBuffer[iValidNum].iStartY=psBoundingBox->piBuffer[iIndex].iStartY;
	 psBoundingBox->piBuffer[iValidNum].iEndX=psBoundingBox->piBuffer[iIndex].iEndX;
	 psBoundingBox->piBuffer[iValidNum].iEndY=psBoundingBox->piBuffer[iIndex].iEndY;
	 psBoundingBox->piBuffer[iValidNum].iValid=1;
	 iValidNum++;
	 }
	 }
	 psValidBlotList->iDataNum = iValidNum;
	 psValidBlotList->iColNum = iValidNum;
	 psBoundingBox->iColNum=iValidNum;*/
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "connecting label");
#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG && ADAS_OS == ADAS_WIN32 && ADAS_DEBUG_VLD_Save_C_LabelResult1
	//VLDDA_Save_C_LabelResult4(psValidBlotList, psV_ED, g_VLD_Params.m_iFrameIndex);
#endif

}	//VLD_Sub_C_BWLabel1

//���ڵ�һ�ι��˵Ƶ���״��������������Cir�У���������ļ���;
/******************************
 1.FilterCir��XY�����ϵİ뾶���Ѿ�����5��������10��
 2.���ﱣ����ǿ������;
 *******************************/
int RecogniseLamp(int label)
{
	INT BlobNum = label - 1;
	INT count = 0;
	INT i;
	FLOAT radx;
	FLOAT rady;

	for (i = 0; i < BlobNum; i++)
	{
		radx = ((FLOAT) (g_VLD_Params.BlobT[i].right
				- g_VLD_Params.BlobT[i].left)) / 2;
		rady = ((FLOAT) (g_VLD_Params.BlobT[i].low - g_VLD_Params.BlobT[i].up))
				/ 2;
		//TRACE("(i = %d)radx = %f,rady = %f\n",i+1,radx,rady);
		if (g_VLD_Params.BlobT[i].area > SDS_dParams.Blot_SmallestArea)	//��ĸ�������Ϊ������;
			if (fabs(radx - rady) < SDS_dParams.Blot_xyOffset)
			{
				memcpy(&(g_VLD_Params.Cir[count].bt), &g_VLD_Params.BlobT[i],
						sizeof(AdasBlobT));
				count++;
				if (count > 30)
					break;
			}
	}
	return count;

}

int GetCirlcleFromBlock(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,
		LPMV_MATRIX_S psLabelMap, AdasDetROI_Float DetectROI,
		BufferParams_DetROI* psValidBlotList, BufferParams_Int* psQueueArr,
		BufferParams_DetROI* psBoundingBox, BufferParams_Int* psArea,
		BYTE * pWorkBuffer, INT CirNum)
{

	INT Height = SrcImg->iRows; ////vSize.height;
	INT Width = SrcImg->iCols; ////vSize.width;

	//INT Height1   = SrcImg->iRows; ////vSize.height;
	//INT Width1    = SrcImg->iCols ; ////vSize.width;
	INT Pitch = DesImg->iPitch;

	AdasImageROI Rt;
	INT nShowImageindex = MVZERO;
	INT cs = MVZERO;
	FLOAT radx = MVZERO;
	FLOAT rady = MVZERO;
	AdasBlobT DrawBT;
	INT ExpandPixle = 10;

	LPMV_MATRIX_S ClipImage = MVNULL;	//����������ͼ��
	LPMV_MATRIX_S ClipDesImg = MVNULL;	//���ڴ洢�����м����
	LPMV_MATRIX_S ClippLabelMap = MVNULL;	//�洢���ǩ

	LPMV_MATRIX_S ClipDesImg2 = MVNULL;	//���ڴ洢��Ե��ȡ�м��;

	INT TEST;

	INT *pCenterX;
	INT *pCenterX_Index;

	float C_xff = 0;
	float C_yff = 0;
	float C_rff = -1;
	float C_erff = 0;
	float cg_x = 0;
	float cg_y = 0;
	AdasBlobT tempB;

	Rt.iHeight = Height;
	Rt.iWidth = Width;
	Rt.iX = 0;
	Rt.iY = 0;

	//pCenterX = (INT*)(pWorkBuffer + 0);
	////iOff += sizeof(INT)*iNum;
	//pCenterX_Index = (INT*)(pWorkBuffer + 2);
	////iOff += sizeof(INT)*iNum;

	if (CirNum <= 0)
		return 0;

	//UTI_Image_Show_Dstt(SrcImg,1);

	//iOff=10;
	for (cs = 0; cs < CirNum; cs++)
	{

		radx = ((FLOAT) (g_VLD_Params.Cir[cs].bt.right
				- g_VLD_Params.Cir[cs].bt.left)) / 2;
		rady = ((FLOAT) (g_VLD_Params.Cir[cs].bt.low
				- g_VLD_Params.Cir[cs].bt.up)) / 2;

		DrawBT.left = g_VLD_Params.Cir[cs].bt.left - ExpandPixle;
		DrawBT.right = g_VLD_Params.Cir[cs].bt.right + ExpandPixle;
		DrawBT.up = g_VLD_Params.Cir[cs].bt.up - ExpandPixle;
		DrawBT.low = g_VLD_Params.Cir[cs].bt.low + ExpandPixle;

		Rt.iHeight = (INT) ((FLOAT) (DrawBT.low - DrawBT.up) / (DATA_ALIGN * 2))
				* (DATA_ALIGN * 2) + (DATA_ALIGN * 2);
		Rt.iWidth = (INT) ((FLOAT) (DrawBT.right - DrawBT.left)
				/ (DATA_ALIGN * 2)) * (DATA_ALIGN * 2) + (DATA_ALIGN * 2);
		Rt.iX = DrawBT.left;
		Rt.iY = DrawBT.up;

		if (Rt.iX < 0)
		{
			Rt.iX = 0;
			DrawBT.left = 0;
		}
		if (Rt.iY < 0)
		{
			Rt.iY = 0;
			DrawBT.up = 0;
		}
		if (Rt.iX + Rt.iWidth > Width)
		{
			Rt.iWidth = ((Width - Rt.iX) / (DATA_ALIGN * 2)) * (DATA_ALIGN * 2);
			DrawBT.right = DrawBT.left + Rt.iWidth;
		}
		if (Rt.iY + Rt.iHeight > Height)
		{
			Rt.iHeight = ((Height - Rt.iY) / (DATA_ALIGN * 2))
					* (DATA_ALIGN * 2);
			DrawBT.low = DrawBT.up + Rt.iHeight;
		}
		if (Rt.iWidth <= 0 || Rt.iHeight <= 0)
		{
			memset(&DrawBT, 0, sizeof(AdasBlobT));
			memset(&g_VLD_Params.Cir[cs], 0, sizeof(AdasCircleT));
			continue;	//
		}

		ClipImage = (LPMV_MATRIX_S) (DesImg);

		ClipImage->iRows = Rt.iHeight;
		ClipImage->iCols = Rt.iWidth;
		ClipImage->iPitch = MV_ALIGN(ClipImage->iCols * sizeof(MV_FIXED_S),
				ClipImage->iLineAddrAlign);
		ClipImage->iLength = ClipImage->iPitch * ClipImage->iRows;

		ClippLabelMap = psLabelMap;
		ClippLabelMap->pData = psLabelMap->pData;		//�洢���ǩ

		// ClippLabelMap->pData=(BYTE*)(psLabelMap->pData+2);

		ClippLabelMap->iRows = Rt.iHeight;
		ClippLabelMap->iCols = Rt.iWidth;
		ClippLabelMap->iPitch = MV_ALIGN(
				ClippLabelMap->iCols * sizeof(MV_FIXED_S),
				ClippLabelMap->iLineAddrAlign);
		ClippLabelMap->iLength = ClippLabelMap->iPitch * ClippLabelMap->iRows;
		// ClipDesImg2=psLabelMap;

		ClipDesImg2 = (LPMV_MATRIX_S) (pWorkBuffer);
		ClipDesImg2->pData = (BYTE*) (pWorkBuffer + sizeof(LPMV_MATRIX_S));	//���ڴ洢�����м����
		ClipDesImg2->iLineAddrAlign = psLabelMap->iLineAddrAlign;
		ClipDesImg2->iStartAddrAlign = psLabelMap->iStartAddrAlign;
		ClipDesImg2->pDataAlign = (MV_FIXED_S*) (ClipDesImg2->pData + 8);
		ClipDesImg2->iRows = Rt.iHeight;
		ClipDesImg2->iCols = Rt.iWidth;
		ClipDesImg2->iPitch = MV_ALIGN(ClipDesImg2->iCols * sizeof(MV_FIXED_S),
				ClipDesImg2->iLineAddrAlign);
		ClipDesImg2->iLength = ClipDesImg2->iPitch * ClipDesImg2->iRows;

		//��������Ȥ����ͼ��
		UTI_GetROIMatrix(SrcImg, &(Rt), 1, ClipImage);
		if (GetOneCenterFromBlock(ClipImage, ClipDesImg2, ClippLabelMap,
				psValidBlotList, psQueueArr, psBoundingBox, psArea, pWorkBuffer,
				&C_xff, &C_yff, &C_rff, &C_erff, &cg_x, &cg_y, &tempB, cs))
		{
			g_VLD_Params.Cir[cs].Cir_x = /* m_nImageLeft +*/C_xff + Rt.iX;
			g_VLD_Params.Cir[cs].Cir_y = /*m_nImageLeft +*/C_yff + Rt.iY;
			g_VLD_Params.Cir[cs].Cir_r = C_rff;
			g_VLD_Params.Cir[cs].Cir_f = C_erff;
			//if(C_erff >50)
			//{
			//	int test=0;
			//	//�������ͼ��
			//	SaveBadImageFile(dstt,-4,nWarningCode);//when OutofArea will appear
			//}

			g_VLD_Params.Cir[cs].Cg_x = cg_x + Rt.iX;
			g_VLD_Params.Cir[cs].Cg_y = cg_y + Rt.iY;

			g_VLD_Params.Cir[cs].bt.area = tempB.area;
			g_VLD_Params.Cir[cs].bt.id = tempB.id;
			g_VLD_Params.Cir[cs].bt.left = Rt.iX + tempB.left;
			g_VLD_Params.Cir[cs].bt.right = Rt.iX + tempB.right;
			g_VLD_Params.Cir[cs].bt.low = Rt.iY + tempB.low;
			g_VLD_Params.Cir[cs].bt.up = Rt.iY + tempB.up;
			g_VLD_Params.Cir[cs].bt.Total255 = tempB.Total255;

		} else
		{
			memset(&g_VLD_Params.Cir[cs], 0, sizeof(AdasCircleT));
		}

		nShowImageindex++;

	}

	return CirNum;
}

INT GetThreeCirlcleFromBlock(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,
		LPMV_MATRIX_S psLabelMap, AdasDetROI_Float DetectROI,
		BufferParams_DetROI* psValidBlotList, BufferParams_Int* psQueueArr,
		BufferParams_DetROI* psBoundingBox, BufferParams_Int* psArea,
		BYTE * pWorkBuffer)
{
	INT RetNum = 3;
	INT Height = SrcImg->iRows; ////vSize.height;
	INT Width = SrcImg->iCols; ////vSize.width;
	INT Pitch = DesImg->iPitch;

	AdasImageROI Rt;
	INT nShowImageindex = MVZERO;
	INT cs = MVZERO;
	//FLOAT radx=MVZERO;
	//FLOAT rady=MVZERO;
	AdasBlobT DrawBT;

	//int offsetSpeed_Left=MVZERO;
//	int offsetSpeed_Right=MVZERO;

	LPMV_MATRIX_S ClipImage;	//����������ͼ��
	LPMV_MATRIX_S ClipDesImg;	//���ڴ洢�����м����
	LPMV_MATRIX_S ClippLabelMap;	//�洢���ǩ

	LPMV_MATRIX_S ClipDesImg2;	//���ڱ�Ե��ȡ���м����

	float C_xff = 0;
	float C_yff = 0;
	float C_rff = -1;
	float C_erff = 0;
	float cg_x = 0;
	float cg_y = 0;

	AdasBlobT tempB;

	Rt.iHeight = Height;
	Rt.iWidth = Width;
	Rt.iX = 0;
	Rt.iY = 0;

	for (cs = 0; cs < 3; cs++)
	{
		//double radx=((double)(g_VLD_Params.Cir[cs].bt.right-g_VLD_Params.Cir[cs].bt.left))/2;
		//double rady=((double)(g_VLD_Params.Cir[cs].bt.low-g_VLD_Params.Cir[cs].bt.up))/2;

		//���ڻ������������������Very Important advise
		//int er=(int)((radx+rady)/2.5);
		//DrawBT[cs].left = Cir[cs].bt.left-er*3;//�˴�����Ϊ3��ԭ��Ϊ2
		//DrawBT[cs].right= Cir[cs].bt.right+er*3;
		//DrawBT[cs].up   = Cir[cs].bt.up-er;
		//DrawBT[cs].low  = Cir[cs].bt.low+er;

		INT ExpandPixle = SDS_dParams.Speed_ExpandPixel1;

		////////addd by mao 1128
		INT offsetSpeed_Left = 0;
		INT offsetSpeed_Right = 0;
		if (abs(g_VLD_Params.fSpeed_Cir_x[cs]) > 10)
			ExpandPixle = SDS_dParams.Speed_ExpandPixel2;

		if (g_VLD_Params.fSpeed_Cir_x[cs] > 1)
		{

			offsetSpeed_Right = g_VLD_Params.fSpeed_Cir_x[cs] * 3.5;
			if (abs(g_VLD_Params.fSpeed_Cir_x[cs]) > 10)
			{
				//				offsetSpeed_Right = offsetSpeed_Right + 40;
			}
			//			offsetSpeed_Right = offsetSpeed_Right + 30;
			offsetSpeed_Left = g_VLD_Params.fSpeed_Cir_x[cs] - 10;
		}

		if (g_VLD_Params.fSpeed_Cir_x[cs] < -1)
		{
			offsetSpeed_Right = g_VLD_Params.fSpeed_Cir_x[cs] + 10;
			offsetSpeed_Left = g_VLD_Params.fSpeed_Cir_x[cs] * 3.5;
			//			offsetSpeed_Left = offsetSpeed_Left - 30;
			if (abs(g_VLD_Params.fSpeed_Cir_x[cs]) > 10)
			{
				//				offsetSpeed_Left = offsetSpeed_Left - 40;
			}

		}
		offsetSpeed_Left = 0;		// 2016-05-16
		offsetSpeed_Right = 0;
		DrawBT.left = g_VLD_Params.Cir[cs].bt.left + offsetSpeed_Left
				- ExpandPixle;
		DrawBT.right = g_VLD_Params.Cir[cs].bt.right + offsetSpeed_Right
				+ ExpandPixle;
		//		DrawBT[cs].left = Cir[cs].bt.left  + static_cast<int>(fSpeed_Cir_x[cs]) - ExpandPixle;
		//		DrawBT[cs].right= Cir[cs].bt.right + static_cast<int>(fSpeed_Cir_x[cs]) + ExpandPixle;

		/////add by mao 1128 2014
		//DrawBT[cs].up   = g_VLD_Params.Cir[cs].bt.up    +(INT)(g_VLD_Params.fSpeed_Cir_y[cs])*2 - ExpandPixle;
		//DrawBT[cs].low  = g_VLD_Params.Cir[cs].bt.low   + (INT)(g_VLD_Params.fSpeed_Cir_y[cs])*2 + ExpandPixle;

		DrawBT.up = g_VLD_Params.Cir[cs].bt.up + (INT) (-ExpandPixle);
		DrawBT.low = g_VLD_Params.Cir[cs].bt.low + (INT) (ExpandPixle);

		Rt.iHeight =
				(INT) ((double) (DrawBT.low - DrawBT.up) / (DATA_ALIGN * 2))
						* (DATA_ALIGN * 2) + (DATA_ALIGN * 2);
		Rt.iWidth = (INT) ((double) (DrawBT.right - DrawBT.left)
				/ (DATA_ALIGN * 2)) * (DATA_ALIGN * 2) + (DATA_ALIGN * 2);
		Rt.iX = DrawBT.left;
		Rt.iY = DrawBT.up;

		if (Rt.iX < 0)
		{
			Rt.iX = 0;
			DrawBT.left = 0;
		}
		if (Rt.iY < 0)
		{
			Rt.iY = 0;
			DrawBT.up = 0;
		}
		if (Rt.iX + Rt.iWidth > Width)
		{
			Rt.iWidth = ((Width - Rt.iX) / (DATA_ALIGN * 2)) * (DATA_ALIGN * 2);
			DrawBT.right = DrawBT.left + Rt.iWidth;
		}
		if (Rt.iY + Rt.iHeight > Height)
		{
			Rt.iHeight = ((Height - Rt.iY) / (DATA_ALIGN * 2))
					* (DATA_ALIGN * 2);
			DrawBT.low = DrawBT.up + Rt.iHeight;
		}
		if (Rt.iWidth <= 0 || Rt.iHeight <= 0)
		{
			memset(&DrawBT, 0, sizeof(AdasBlobT));
			memset(&g_VLD_Params.Cir[cs], 0, sizeof(AdasCircleT));
			continue;		//
		}

		ClipImage = DesImg;

		ClipImage->iRows = Rt.iHeight;
		ClipImage->iCols = Rt.iWidth;
		ClipImage->iPitch = MV_ALIGN(ClipImage->iCols * sizeof(MV_FIXED_S),
				ClipImage->iLineAddrAlign);
		ClipImage->iLength = ClipImage->iPitch * ClipImage->iRows;

		ClippLabelMap = psLabelMap;		//�洢���ǩ

		ClippLabelMap->iRows = Rt.iHeight;
		ClippLabelMap->iCols = Rt.iWidth;
		ClippLabelMap->iPitch = MV_ALIGN(
				ClippLabelMap->iCols * sizeof(MV_FIXED_S),
				ClippLabelMap->iLineAddrAlign);
		ClippLabelMap->iLength = ClippLabelMap->iPitch * ClippLabelMap->iRows;

		// ClipDesImg2=psLabelMap;

		ClipDesImg2 = (LPMV_MATRIX_S) (pWorkBuffer);
		ClipDesImg2->pData = (BYTE*) (pWorkBuffer + sizeof(LPMV_MATRIX_S));	//���ڴ洢�����м����
		ClipDesImg2->iLineAddrAlign = psLabelMap->iLineAddrAlign;
		ClipDesImg2->iStartAddrAlign = psLabelMap->iStartAddrAlign;
		ClipDesImg2->pDataAlign = (MV_FIXED_S*) (ClipDesImg2->pData + 8);

		ClipDesImg2->iRows = Rt.iHeight;
		ClipDesImg2->iCols = Rt.iWidth;
		ClipDesImg2->iPitch = MV_ALIGN(ClipDesImg2->iCols * sizeof(MV_FIXED_S),
				ClipDesImg2->iLineAddrAlign);
		ClipDesImg2->iLength = ClipDesImg2->iPitch * ClipDesImg2->iRows;

//		Log_write1(UIABenchmark_start, (xdc_IArg) "copy ROI");
		//��������Ȥ����ͼ��
//		if(ClipImage->iCols*ClipImage->iRows<40000)
//		{
//
//		}
		UTI_GetROIMatrix(SrcImg, &(Rt), 1, ClipImage);
//		Log_write1(UIABenchmark_stop, (xdc_IArg) "copy ROI");

		if (GetOneCenterFromBlock(ClipImage, ClipDesImg2, ClippLabelMap,
				psValidBlotList, psQueueArr, psBoundingBox, psArea, pWorkBuffer,
				&C_xff, &C_yff, &C_rff, &C_erff, &cg_x, &cg_y, &tempB, cs))
		{
			g_VLD_Params.Cir[cs].Cir_x = /* m_nImageLeft +*/C_xff + Rt.iX;
			g_VLD_Params.Cir[cs].Cir_y = /*m_nImageLeft +*/C_yff + Rt.iY;
			g_VLD_Params.Cir[cs].Cir_r = C_rff;
			g_VLD_Params.Cir[cs].Cir_f = C_erff;
			if (C_erff > 50)
			{
				//int test=0;
				//�������ͼ��
				//SaveBadImageFile(dstt,-4,nWarningCode);//when OutofArea will appear
			}

			g_VLD_Params.Cir[cs].Cg_x = cg_x + Rt.iX;
			g_VLD_Params.Cir[cs].Cg_y = cg_y + Rt.iY;

			g_VLD_Params.Cir[cs].bt.area = tempB.area;
			g_VLD_Params.Cir[cs].bt.id = tempB.id;
			g_VLD_Params.Cir[cs].bt.left = Rt.iX + tempB.left;
			g_VLD_Params.Cir[cs].bt.right = Rt.iX + tempB.right;
			g_VLD_Params.Cir[cs].bt.low = Rt.iY + tempB.low;
			g_VLD_Params.Cir[cs].bt.up = Rt.iY + tempB.up;
			g_VLD_Params.Cir[cs].bt.Total255 = tempB.Total255;

		} else
		{
			memset(&g_VLD_Params.Cir[cs], 0, sizeof(AdasCircleT));
			RetNum--;
		}

		nShowImageindex++;

		//cvReleaseImage(&ClipImage);
		//cvResetImageROI(dstt);
	}

	return RetNum;

}

void DSP_SobelPart2(unsigned char *restrict inoutbuf,
		const unsigned char *restrict addbuf, short cols, short rows)
{
	_nassert(cols > 3);
	_nassert(rows >= 3);
	_nassert(cols % 2 == 0);
	int i, end;
	end = cols * rows;
#pragma UNROLL(2)
	for (i = 0; i < end; i += 8)
	{
		double ina, inb;
		int a1, a2, b1, b2, o1, o2;
		ina = _memd8_const((void*) &inoutbuf[i]);
		inb = _memd8_const((void*) &addbuf[i]);
		a1 = _hi(ina);
		a2 = _lo(ina);
		b1 = _hi(inb);
		b2 = _lo(inb);
		_abs2(a1);
		_abs2(a2);
		_abs2(b1);
		_abs2(b2);
		o1 = _add4(a1, b1);
		o2 = _add4(a2, b2);
		_memd8((void*) &inoutbuf[i]) = _itod(o1, o2);
	}
}
int GetOneCenterFromBlock(LPMV_MATRIX_S m_pBlockImage, LPMV_MATRIX_S DesImg2,
		LPMV_MATRIX_S psLabelMap, BufferParams_DetROI* psValidBlotList,
		BufferParams_Int* psQueueArr, BufferParams_DetROI* psBoundingBox,
		BufferParams_Int* psArea, BYTE * pWorkBuffer, FLOAT *xff, FLOAT *yff,
		FLOAT *rff, FLOAT *erff, FLOAT *cg_x, FLOAT *cg_y, AdasBlobT* OutBlob,
		int index)
{
//	Log_write1(UIABenchmark_start, (xdc_IArg) "detect one circle");
	INT Width = m_pBlockImage->iCols;
	INT Height = m_pBlockImage->iRows;
	INT threshold;
	//LPMV_MATRIX_S m_pSmoothImage;
	LPMV_MATRIX_S LabelImage;
	INT labelNum = MVZERO;

	INT ncountNum = MVZERO;
	double countX = MVZERO;
	double countY = MVZERO;
//	FLOAT Center_X = MVZERO;
	//FLOAT Center_Y = MVZERO;
	INT label = 1;
	INT i = MVZERO;
	INT j = MVZERO;
	INT nTotal255Value = MVZERO;

	INT iEnd1 = MVZERO;
	INT iEnd2 = MVZERO;

	INT iRow = MVZERO;
	INT iCol = MVZERO;

	INT iVar = MVZERO;
	INT iVar1 = MVZERO;
	INT iVar2 = MVZERO;

	//LPMV_MATRIX_S sFilter = MVNULL;
//	if (Width * Height < 40000)
//	{
//
//		L2_labelmap.iStartAddrAlign = psLabelMap->iStartAddrAlign;
//		L2_labelmap.iLineAddrAlign = psLabelMap->iLineAddrAlign;
//		L2_labelmap.iRows = psLabelMap->iRows;
//		L2_labelmap.iCols = psLabelMap->iRows;
//		L2_labelmap.iPitch = psLabelMap->iPitch;
//		L2_labelmap.iLength = psLabelMap->iLength;
//		L2_labelmap.pDataAlign = L2_labelmapdata;
//		LabelImage = &L2_labelmap;
//	} else
	LabelImage = psLabelMap;

	//�������ֵ����Сֵ

	//�ǲ�

	//UTI_Image_Show_Dstt(DesImg,1);

	//UTI_Image_Show_Dstt(DesImg,1);
//	p1 = Utils_getCurTimeInMsec();
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part1 label");
#ifdef _C66X_
//	memcpy(DesImg->pDataAlign, m_pBlockImage->pDataAlign,
//			(m_pBlockImage->iLength));
//	edma_copy2D(DesImg->pDataAlign, m_pBlockImage->pDataAlign,
//			m_pBlockImage->iCols , m_pBlockImage->iRows, 1);
#else
	memcpy(DesImg->pDataAlign, m_pBlockImage->pDataAlign,
			(m_pBlockImage->iCols * m_pBlockImage->iLength));
//	MedianSmooth(m_pBlockImage, DesImg,7,pWorkBuffer);//ͼ������
#endif

	//��ȡ��ֵ
	threshold = GetOtsuThreshold(m_pBlockImage);
	if (index < 5)
		g_VLD_Params.m_pResultData.threshold[index] = threshold;
	//��ֵ��
//	Log_write1(UIABenchmark_start, (xdc_IArg) "threshold");
	Threshold(m_pBlockImage, threshold);
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "threshold");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "label");
	//���ǩ
	VLD_Sub_C_BWLabel_SDS(m_pBlockImage, LabelImage, LabelImage,
			psValidBlotList, psQueueArr, psBoundingBox, psArea, pWorkBuffer);
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "label");
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part1 label");
//	Log_write1(UIABenchmark_start, (xdc_IArg) "part2 label");
	labelNum = psBoundingBox->iColNum;
	if (labelNum <= 0)
	{
		return 0;
	}

	//(*((MV_FIXED_S*)(((BYTE*)SrcImg->pDataAlign) + (m+i) * SrcImg->iPitch) + n+j));

	if (labelNum > 1)
	{
		label = GetLableIndex(labelNum);

		memcpy(OutBlob, &g_VLD_Params.BlobT[label - 1], sizeof(AdasBlobT));

		for (i = 0; i < Height; i++)
			for (j = 0; j < Width; j++)
			{
				//if((*(m_pLableImage->imageData+i*Width+j))==label)
				if ((*((MV_FIXED_S*) (((BYTE*) LabelImage->pDataAlign)
						+ (i) * LabelImage->iPitch) + j)) == label)
				{
					//(*(TwoValueImage->imageData+i*Width+j))=255;
					(*((MV_FIXED_S*) (((BYTE*) m_pBlockImage->pDataAlign)
							+ (i) * m_pBlockImage->iPitch) + j)) = 255;
					/*if((*((MV_FIXED_S*)(((BYTE*)m_pBlockImage->pDataAlign) + (i) * m_pBlockImage->iPitch) +j))>=250)
					 {
					 nTotal255Value++;
					 }*/
					ncountNum++;
					countX = countX + (FLOAT) j;
					countY = countY + (FLOAT) i;
				} else
					(*((MV_FIXED_S*) (((BYTE*) m_pBlockImage->pDataAlign)
							+ (i) * m_pBlockImage->iPitch) + j)) = 0;
			}

		OutBlob->Total255 = nTotal255Value;
	} else
	{
		memcpy(OutBlob, &g_VLD_Params.BlobT[0], sizeof(AdasBlobT));
		nTotal255Value = 0;
		for (i = 0; i < Height; i++)
			for (j = 0; j < Width; j++)
			{
				if ((*((MV_FIXED_S*) (((BYTE*) LabelImage->pDataAlign)
						+ (i) * LabelImage->iPitch) + j)) == label)
				{
					/*if((*((MV_FIXED_S*)(((BYTE*)m_pBlockImage->pDataAlign) + (i) * m_pBlockImage->iPitch) +j))>=250)
					 {
					 nTotal255Value++;
					 }*/
					ncountNum++;
					countX = countX + (double) j;
					countY = countY + (double) i;
				} else
					(*((MV_FIXED_S*) (((BYTE*) m_pBlockImage->pDataAlign)
							+ (i) * m_pBlockImage->iPitch) + j)) = 0;
			}
		OutBlob->Total255 = nTotal255Value;

	}

	//����������ȡ��Ե
	*cg_x = countX / ncountNum;
	*cg_y = countY / ncountNum;

//��Ե��ȡ
//	IMG_sobel_3x3_8(m_pBlockImage->pDataAlign,
//			DesImg2->pDataAlign + DesImg2->iCols, DesImg2->iCols,
//			DesImg2->iRows);

	int edgecount = 0;
	for (i = 0; i < DesImg2->iRows - 2; i++)
		edgecount += IMG_perimeter_8(
				m_pBlockImage->pDataAlign + i * DesImg2->iCols, DesImg2->iCols,
				DesImg2->pDataAlign + (1 + i) * DesImg2->iCols);
	if (edgecount > 1599)
		return 0;
//	pcvSobel(m_pBlockImage, LabelImage, PCV_DIRT_X, PCV_SOBEL_3X3);
//	pcvSobel(m_pBlockImage, DesImg2, PCV_DIRT_Y, PCV_SOBEL_3X3);
//	/*sFilter = g_VLD_Params.m_sADAS_GP_Params->m_sSobelFilter.psVerticalFilter3;
//	 MV_Convolution(DesImg, DesImg2, sFilter, 3, 0);
//	 sFilter =g_VLD_Params.m_sADAS_GP_Params->m_sSobelFilter.psHorizontalFilter3;
//	 MV_Convolution(DesImg,LabelImage,sFilter,3,0);*/
//
//	iEnd1 = m_pBlockImage->iRows;
//	iEnd2 = m_pBlockImage->iCols;
////	Log_write1(UIABenchmark_stop, (xdc_IArg) "part2 label");
//	///////////////////////////
////	Log_write1(UIABenchmark_start, (xdc_IArg) "part3");
//#ifdef _C66X_
//	DSP_SobelPart2(DesImg2->pDataAlign, LabelImage->pDataAlign, iEnd2, iEnd1);
//#else
//	for (iRow = 0; iRow < iEnd1; iRow++)
//	{
//		for (iCol = 0; iCol < iEnd2; iCol++)
//		{
//			iVar1 = *((MV_FIXED_S*) (((BYTE*) DesImg2->pDataAlign)
//							+ iRow * DesImg2->iPitch) + iCol);
//			iVar2 = *((MV_FIXED_S*) (((BYTE*) LabelImage->pDataAlign)
//							+ iRow * LabelImage->iPitch) + iCol);
////			iVar = iVar1 * iVar1 + iVar2 * iVar2;
////			iVar = sqrtsp(iVar);
//			iVar = abs(iVar1)+abs(iVar2);//c=|a|+|b| instead of c=sqrt(a^2+b^2)
////			iVar = sqrt(iVar);
//			if (iVar > (255))
//			{
//				iVar = 255;
//			}
//			*((MV_FIXED_S*) (((BYTE*) DesImg2->pDataAlign)
//							+ iRow * DesImg2->iPitch) + iCol) = iVar;
//		}    //iCol
//	}    //iRow
//#endif

	//�ٽ���һ�α�Ե��ȡ

	//pcvSobel(DesImg2, LabelImage, PCV_DIRT_X,PCV_SOBEL_3X3);

	//pcvSobel(DesImg2, DesImg2, PCV_DIRT_Y,PCV_SOBEL_3X3);

	//ϸ������
	//��С���˷����Բ��
//	Log_write1(UIABenchmark_start, (xdc_IArg) "process label");
	GetCirclePara(DesImg2, xff, yff, rff, erff, OutBlob, edgecount);
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "process label");
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "part3");
#if ADAS_DRAW_MODE == ADAS_DRAW_FOR_DEBUG && ADAS_DEBUG_VLD_DrawROI
	UTI_Image_Show_Dstt(DesImg2,1);
#endif
//	p6 = Utils_getCurTimeInMsec();
//	System_printf("the part1 cost is %dms\n",p2-p1);
//	System_printf("the part2 cost is %dms\n",p3-p2);
//	System_printf("the part3 cost is %dms\n",p4-p3);
//	System_printf("the part4 cost is %dms\n",p5-p4);
//	System_printf("the part5 cost is %dms\n",p6-p5);
//	Log_write1(UIABenchmark_stop, (xdc_IArg) "detect one circle");
	return 1;
}

INT GetLableIndex(INT BlobNum)
{

	//INT count=0;
	INT i = MVZERO;
	INT MaxSum = MVZERO;
	INT Lableindex = MVZERO;
	for (i = 0; i < BlobNum; i++)
	{
		if (g_VLD_Params.BlobT[i].area > MaxSum)
		{
			MaxSum = g_VLD_Params.BlobT[i].area;
			Lableindex = i + 1;
		}
	}
	return Lableindex;

}

//��ֵ�˲�
//===============================wxf added 2016-10-21=================================

int select_kth(char*data, int n, int k)
{
	int big, little;
	big = n - 1, little = 0;
	int v = data[k];	//ѡȡdata[k]����Ϊ����data�ǻ�������ģ������ܼ��ٲ���Ҫ�Ľ���
	int i = 0;
	while (i <= big)
	{
		if (data[i] < v)
		{
			if (i != little)
				data[little] = data[i];
			i++;
			little++;
		} else if (data[i] > v)
		{
			int temp;
			temp = data[i];
			data[i] = data[big];
			data[big] = temp;
			big--;
		} else
			i++;
	}
	//��ԭ���ݣ���Ϊ���ڻ��ٴ��õ�������
	for (i = little; i <= big; i++)
		data[little] = v;
	if (k < little)
		return select_kth(data, little, k);
	else if (k <= big)
		return data[k];
	else
		return select_kth(data + big + 1, n - big - 1, k - big - 1);
}

int getMedian(char* data, int n)
{
	int k, median;
	k = n / 2;
	if (n % 2)
		median = select_kth(data, n, k);
	else
		median = (select_kth(data, n, k) + select_kth(data, n, k - 1)) / 2;
	return median;
}
//===============================wxf added 2016-10-21=================================

INT MedianSmooth(LPMV_MATRIX_S SrcImg, LPMV_MATRIX_S DesImg,       //��ֵ�˲����ͼ��
		INT nTemplateType,         //��ֵ�˲�ģ������(3��ʾ3�ף�5��ʾ5�ף�7��ʾ7��)��ģ�����ͱ���������

		BYTE *pWorkBuffer)     //�����ռ�
{
	DesImg = SrcImg;

////	Log_write1(UIABenchmark_start, (xdc_IArg) "median smooth 1");
//	INT nWidth = SrcImg->iCols;             //ͼ���
//	INT nHeight = SrcImg->iRows;           //ͼ��߶�
//
//	INT iOff = MVZERO;
//	//MV_FIXED_S *pSrcLineAddr;
//	//MV_FIXED_S *pDstLineAddr;
//
//	BYTE *cTemplateBuffer;	//ģ�帲�Ƿ�Χ��ͼ�����صĻ�����
//	INT nTemplateBufferIndex = 0;	//ģ�帲�Ƿ�Χ��ͼ�����صĻ����������±�
//	INT i = MVZERO;	//forѭ���м����(ģ���е���)
//	INT j = MVZERO;	//forѭ���м����(ģ���е���)
//	INT m = MVZERO;	//forѭ���м����(x)
//	INT n = MVZERO;	//forѭ���м����(y)
//	INT k = MVZERO;	//forѭ���м����(ͼ���Եδģ�廯���л�����)
//	//BYTE SwapTemp = ' ';	//�����������ص��м����      //wxf 2016-10-21
//
//	cTemplateBuffer = (BYTE*) (pWorkBuffer + iOff);
//	iOff += sizeof(BYTE) * 277;
//
//	//���������飬����
//
//	//��ģ�徭�����������ֵ���뻺����������ֵ��Ϊģ���м�Ļ���ָ��λ�õ�����ֵ
//	for (m = (nTemplateType - 1) / 2; m < nHeight - (nTemplateType - 1) / 2;
//			m++)
//
//	{
//
////		pDstLineAddr = MV_MatrixGetLinePtr_S(dst, iRow, 0);
//		for (n = (nTemplateType - 1) / 2; n < nWidth - (nTemplateType - 1) / 2;
//				n++)
//		{
//			nTemplateBufferIndex = 0;
//			//�Ƚ�ģ����Ӧλ�õĸ�������ֵ����ڻ�������
//			for (i = -(nTemplateType - 1) / 2; i < (nTemplateType - 1) / 2 + 1;
//					i++)
//			{
//				//pSrcLineAddr = MV_MatrixGetLinePtr_S(SrcImg, (m+i), i);
//				for (j = -(nTemplateType - 1) / 2;
//						j < (nTemplateType - 1) / 2 + 1; j++)
//				{
//					cTemplateBuffer[nTemplateBufferIndex++] =
//							(*((MV_FIXED_S*) (((BYTE*) SrcImg->pDataAlign)
//									+ (m + i) * SrcImg->iPitch) + n + j)); //pSrcLineAddr[j];//pSrcImageData[(m + i) * nWidthStep + n + j];
//				}
//			}
//			//�Դ˻������ĸ������ؽ�������
//			/*for (i = 0; i < nTemplateType * nTemplateType - 1; i++)
//			{
//				for (j = 0; j < nTemplateType * nTemplateType - i - 1; j++)
//				{
//					if (cTemplateBuffer[j] > cTemplateBuffer[j + 1])
//					{
//						SwapTemp = cTemplateBuffer[j];
//						cTemplateBuffer[j] = cTemplateBuffer[j + 1];
//						cTemplateBuffer[j + 1] = SwapTemp;
//					}
//				}
//			}*/
//
//
//			//������������������м��һ������ֵ��Ϊģ�����м��λ������ֵ
//			//pMedianImage[m * nWidthStep + n] = cTemplateBuffer[(nTemplateType * nTemplateType - 1) / 2];
//
//
//			//==================WXF added 2016-10-21========================
//			cTemplateBuffer[(nTemplateType * nTemplateType - 1)/ 2]=getMedian(cTemplateBuffer, (nTemplateType * nTemplateType - 1));
//
//			//==================WXF added 2016-10-21========================
//
//			(*((MV_FIXED_S*) (((BYTE*) DesImg->pDataAlign) + m * DesImg->iPitch)
//					+ n)) = cTemplateBuffer[(nTemplateType * nTemplateType - 1)
//					/ 2];
//		}
//	}
//
//	//û�о���ģ���˲���������ԭ���ش���
//	for (k = 0; k < nTemplateType / 2; k++)
//	{
//		for (m = 0; m < nWidth; m++)
//		{
//			//ǰ�벿����
//			//pMedianImage[m + k * nWidthStep] = pSrcImageData[m + k * nWidthStep];
//			(*((MV_FIXED_S*) (((BYTE*) DesImg->pDataAlign) + k * DesImg->iPitch)
//					+ m)) = (*((MV_FIXED_S*) (((BYTE*) SrcImg->pDataAlign)
//					+ k * SrcImg->iPitch) + m));
//			//��벿����
//			//pMedianImage[m + (nHeight - 1 - k) * nWidthStep] = pSrcImageData[m + (nHeight - 1 - k) * nWidthStep];
//			(*((MV_FIXED_S*) (((BYTE*) DesImg->pDataAlign)
//					+ (nHeight - 1 - k) * DesImg->iPitch) + m)) =
//					(*((MV_FIXED_S*) (((BYTE*) SrcImg->pDataAlign)
//							+ (nHeight - 1 - k) * SrcImg->iPitch) + m));
//		}
//
//		for (m = 0; m < nHeight; m++)
//		{
//			//ǰ�벿����
//			//pMedianImage[m * nWidthStep + k] = pSrcImageData[m * nWidthStep + k];
//			(*((MV_FIXED_S*) (((BYTE*) DesImg->pDataAlign) + m * DesImg->iPitch)
//					+ k)) = (*((MV_FIXED_S*) (((BYTE*) SrcImg->pDataAlign)
//					+ m * SrcImg->iPitch) + k));
//
//			//��벿����
//			//pMedianImage[(m + 1) * nWidthStep - 1 - k] = pSrcImageData[(m + 1) * nWidthStep - 1 - k];
//			(*((MV_FIXED_S*) (((BYTE*) DesImg->pDataAlign)
//					+ (m + 1) * DesImg->iPitch) - 1 - k)) =
//					(*((MV_FIXED_S*) (((BYTE*) SrcImg->pDataAlign)
//							+ (m + 1) * SrcImg->iPitch) - 1 - k));
//		}
//	}
//	//Log_write1(UIABenchmark_stop, (xdc_IArg) "median smooth 1");
	return 1;
}

INT FilterCir5(INT nCirNum)
{
	INT count = 0;
	INT jjj = 0;

	double radx;
	double rady;

	INT i;
	for (i = 0; i < nCirNum; i++)
	{
		if (g_VLD_Params.Cir[i].Cir_r > SDS_dParams.nBiggestRaduis)
		{
			return -2;
		}
	}

	for (i = 0; i < nCirNum; i++)
	{
		radx = ((double) (g_VLD_Params.Cir[i].bt.right
				- g_VLD_Params.Cir[i].bt.left)) / 2;
		rady =
				((double) (g_VLD_Params.Cir[i].bt.low
						- g_VLD_Params.Cir[i].bt.up)) / 2;
		//		TRACE("Cir[%d].Cir_f = %f\n",i,Cir[i].Cir_f);
		if (g_VLD_Params.Cir[i].bt.area > 30)			//��ĸ�������Ϊ������
			if (fabs(radx - rady) < 10)
			{
				//׼��������ʾ
				//memcpy(&(CirShow[jjj]),&(Cir[i]),sizeof(CircleT));
				jjj++;

				if (g_VLD_Params.Cir[i].Cir_f < 3.0)//�����ſ�һ��advised by mao 20121031
				{
					memcpy(&(g_VLD_Params.Cir[count]), &(g_VLD_Params.Cir[i]),
							sizeof(AdasCircleT));
					count++;
					if (count > 30)
						break;
				}
			}

	}

	return count;
}

INT FilterCir(INT nCirNum)
{
	INT count = MVZERO;

	INT i = MVZERO;
	for (i = 0; i < nCirNum; i++)
	{
		if (g_VLD_Params.Cir[i].Cir_r > SDS_dParams.nBiggestRaduis)
		{
			return -2;
		}
	}

	for (i = 0; i < nCirNum; i++)
	{
		double radx = ((double) (g_VLD_Params.Cir[i].bt.right
				- g_VLD_Params.Cir[i].bt.left)) / 2;
		double rady = ((double) (g_VLD_Params.Cir[i].bt.low
				- g_VLD_Params.Cir[i].bt.up)) / 2;
		//		TRACE("Cir[%d].Cir_f = %f\n",i,Cir[i].Cir_f);
		if (g_VLD_Params.Cir[i].bt.area > SDS_dParams.Cir_SmallestArea)	//��ĸ�������Ϊ������
			if (fabs(radx - rady) < SDS_dParams.Cir_xyOffset)
				if (g_VLD_Params.Cir[i].Cir_f < SDS_dParams.Cir_errf)//�����ſ�һ��advised by mao 20121031
				{
					memcpy(&(g_VLD_Params.Cir[count]), &(g_VLD_Params.Cir[i]),
							sizeof(AdasCircleT));
					count++;
					if (count > 30)
						break;
				}

	}

	return count;
}

INT JudgeSDSCircle5(LPMV_MATRIX_S SrcImg, INT SDSCirNum)
{
	INT nCirNum = SDSCirNum;
	INT i = 0;
	BOOL BadCir[5];
	FLOAT valueGood;				//fCirGood=1.3
	FLOAT valueWaring;				//fCirWarning = 1.5
	FLOAT Cir2_fTemp = 0.0;
	FLOAT Cir3_fTemp = 0.0;
	INT Cir0Flag = MVZERO;
	INT Cir1Flag = MVZERO;
	INT Cir2Flag = MVZERO;
	INT Cir3Flag = MVZERO;				//chengjs20140928 add
	INT Cir4Flag = MVZERO;				//chengjs20140928 add

	//INT nMiddleIndex=MVZERO;
	//INT nRightIndex=MVZERO;
	//INT nLeftIndex=MVZERO;
	//�����������ڵ�����Բ������һ����ֱ���ҵ�����Բ���������������ҵ�һ�����������ͱ���

	INT nCenterNum = MVZERO;
	INT CenterList[30];

	FLOAT fSpeed_Cir_x[5];		//chengjs20140928 modify 3->5
	FLOAT fSpeed_Cir_y[5];

	double radx = 0.0;
	double rady = 0.0;

//	INT ImWidth=SrcImg->iCols;
	INT ImHeight = SrcImg->iRows;

	INT kkk = MVZERO;

	//INT nSearchLeft_X        = (INT)(ImWidth*SDS_dParams.ROI_startX);/*ImWidth/6;*///�������������4�ı���
//	INT nSearchRight_X       = (INT)(ImWidth*SDS_dParams.ROI_endX);/*9*ImWidth/10;*///��������
	INT nCenterCircleTop_Y = (INT) (ImHeight * SDS_dParams.ROI_startY);/*ImHeight/3;*///����Բ�жϱ�׼
	INT nCenterCircleLow_Y = (INT) (ImHeight * SDS_dParams.ROI_endY);/*2*ImHeight/3;*///����Բ�жϱ�׼advised by mao at 7.6  448---360

	for (i = 0; i < nCirNum; i++)
	{
		//chengjs20121017Ŀǰ���ֵΪ52���������ж�ֵΪ60������ֵ�ǲ��ǽ�һ����С
		//if(Cir[i].Cir_r>nBiggestRaduis)
		if (g_VLD_Params.Cir[i].Cir_r > SDS_dParams.nBiggestRaduis)
		{
			return -2;
		}
	}

	//���������Բ�ĵĸ���
	if (SDSCirNum == g_VLD_Params.UsedCircleNum)//UsedCircleNum)//chengjs20140928 modify 3->UsedCircleNum
	{
		//Բ�뾶����
		//BOOL BadCir[5];
		memset(BadCir, 0, 5 * sizeof(BOOL));
		valueGood = SDS_dParams.fCirGood_1;		//fCirGood=1.3
		valueWaring = SDS_dParams.fCirWarning_1;		//fCirWarning = 1.5

		//Ϊʲô���ұߵƶ������б�׼???��ʵ���Ҳһ��
		//�ұߵ�(1�ŵ�)
		if (g_VLD_Params.Cir[0].Cir_r > 20)
		{
			valueGood = SDS_dParams.fCirGood_2;		//fCirGood_R20 = 1.4
			valueWaring = SDS_dParams.fCirWarning_2;	//fCirWarning_R20 = 1.8
		}
		if (g_VLD_Params.Cir[0].Cir_r > 25)
		{
			valueGood = SDS_dParams.fCirGood_3;		//fCirGood_R25=1.5
			valueWaring = SDS_dParams.fCirWarning_3;	//fCirWarning_R25 = 1.9
		}
		//Բ�뾶����Ʒ��
		if (g_VLD_Params.Cir[0].Cir_f > valueGood
				&& g_VLD_Params.Cir[0].Cir_f < valueWaring)
		{
			Cir0Flag = 1;
		} else if (g_VLD_Params.Cir[0].Cir_f >= valueWaring)
		{
			Cir0Flag = 2;
			BadCir[0] = TRUE;
		} else
		{
			Cir0Flag = 0;
		}

		//chengjs20141014 add
		/*double Cir2_fTemp = 0.0;
		 double Cir3_fTemp = 0.0;*/
		if (g_VLD_Params.Cir[1].Cir_x > g_VLD_Params.Cir[2].Cir_x)
		{
			Cir2_fTemp = g_VLD_Params.Cir[2].Cir_f;
			Cir3_fTemp = g_VLD_Params.Cir[1].Cir_f;
		} else
		{
			Cir2_fTemp = g_VLD_Params.Cir[1].Cir_f;
			Cir3_fTemp = g_VLD_Params.Cir[2].Cir_f;
		}
		//�м��(2�ŵ�)
		if (Cir2_fTemp > (valueGood + 0.2) && Cir2_fTemp < (valueWaring + 0.4))
		{
			Cir1Flag = 1;
		} else if (Cir2_fTemp >= valueWaring + 0.4)
		{
			Cir1Flag = 2;
			BadCir[1] = TRUE;
		} else
		{
			Cir1Flag = 0;
		}
		//��ߵ�(���ŵ�)
		if (Cir3_fTemp > valueGood && Cir3_fTemp < valueWaring)
		{
			Cir2Flag = 1;
		} else if (Cir3_fTemp >= valueWaring)
		{
			Cir2Flag = 2;
			BadCir[2] = TRUE;
		} else
		{

			Cir2Flag = 0;
		}

		//�ĺŵ�
		if (g_VLD_Params.Cir[3].Cir_f > valueGood
				&& g_VLD_Params.Cir[3].Cir_f < valueWaring)
		{
			Cir3Flag = 1;
		} else if (g_VLD_Params.Cir[3].Cir_f >= valueWaring)
		{
			Cir3Flag = 2;
			BadCir[3] = TRUE;
		} else
		{
			Cir3Flag = 0;
		}
		//��ŵ�
		if (g_VLD_Params.Cir[4].Cir_f > valueGood
				&& g_VLD_Params.Cir[4].Cir_f < valueWaring)
		{
			Cir4Flag = 1;
		} else if (g_VLD_Params.Cir[4].Cir_f >= valueWaring)
		{
			Cir4Flag = 2;
			BadCir[4] = TRUE;
		} else
		{
			Cir4Flag = 0;
		}

		//for(int i=0;i<UsedCircleNum;i++)//chengjs20140928 modify 3->UsedCircleNum
		for (i = 0; i < g_VLD_Params.UsedCircleNum; i++)
		{
			radx = ((double) (g_VLD_Params.Cir[i].bt.right
					- g_VLD_Params.Cir[i].bt.left)) / 2;
			rady = ((double) (g_VLD_Params.Cir[i].bt.low
					- g_VLD_Params.Cir[i].bt.up)) / 2;
			if ((g_VLD_Params.Cir[i].bt.area < SDS_dParams.nSmallestRaduis)
					|| g_VLD_Params.Cir[i].Cir_r < SDS_dParams.nSmallestRaduis
					|| (fabs(radx - rady) > 5)
					|| g_VLD_Params.Cir[i].Cir_r > SDS_dParams.nBiggestRaduis)
			{
				BadCir[i] = TRUE;
			}

		}
		//chengjs20140928add
		if (4 == g_VLD_Params.UsedCircleNum)
		{
			Cir4Flag = 0;
			BadCir[4] = FALSE;
		} else if (3 == g_VLD_Params.UsedCircleNum)
		{
			Cir3Flag = 0;
			Cir4Flag = 0;
			BadCir[3] = FALSE;
			BadCir[4] = FALSE;
		}

		if (BadCir[0])		//�ұߵ�(1�ŵ�)
		{
			return 9;
		}
		if (BadCir[1])		//�м��(2�ŵ�)
		{
			return 10;
		}
		if (BadCir[2])		//��ߵ�(3�ŵ�)
		{
			return 11;
		}
		//chengjs20140928add
		if (BadCir[3])		//��ߵ�(4�ŵ�)
		{
			return 7;
		}
		if (BadCir[4])		//��ߵ�(5�ŵ�)
		{
			return 8;
		}

		//����Բ��ֵ���� ,ͨ���ĸ�Բ֮��İ뾶��ϵ
		if (g_VLD_Params.Cir[0].Cg_y > g_VLD_Params.Cir[1].Cg_y
				|| g_VLD_Params.Cir[1].Cg_y > g_VLD_Params.Cir[3].Cg_y
				|| g_VLD_Params.Cir[0].Cg_y > g_VLD_Params.Cir[2].Cg_y
				|| g_VLD_Params.Cir[2].Cg_y > g_VLD_Params.Cir[3].Cg_y
				/*|| Cir[1].Cg_x > Cir[0].Cg_x || Cir[0].Cg_x > Cir[2].Cg_x
				 || Cir[1].Cg_x > Cir[3].Cg_x || Cir[3].Cg_x > Cir[2].Cg_x*/
				|| g_VLD_Params.Cir[1].Cir_r > g_VLD_Params.Cir[0].Cir_r
				|| g_VLD_Params.Cir[1].Cir_r > g_VLD_Params.Cir[3].Cir_r
				|| g_VLD_Params.Cir[2].Cir_r > g_VLD_Params.Cir[0].Cir_r
				|| g_VLD_Params.Cir[2].Cir_r > g_VLD_Params.Cir[3].Cir_r
				|| g_VLD_Params.Cir[1].Cg_y < nCenterCircleTop_Y
				|| g_VLD_Params.Cir[1].Cg_y > nCenterCircleLow_Y
				|| g_VLD_Params.Cir[2].Cg_y < nCenterCircleTop_Y
				|| g_VLD_Params.Cir[2].Cg_y > nCenterCircleLow_Y
				|| abs(g_VLD_Params.Cir[0].Cir_r - g_VLD_Params.Cir[3].Cir_r)
						> SDS_dParams.f_RDifSideCir
				|| abs(g_VLD_Params.Cir[1].Cir_r - g_VLD_Params.Cir[2].Cir_r)
						> SDS_dParams.f_RDifSideCir)
		{
			return 12;
		}

		if (nCirNum == g_VLD_Params.UsedCircleNum)//chengjs20140928 modify 3->UsedCircleNum
		{
			for (kkk = 0; kkk < g_VLD_Params.UsedCircleNum; kkk++)
			{
				fSpeed_Cir_x[kkk] = (FLOAT) (g_VLD_Params.Cir[kkk].Cg_x
						- g_VLD_Params.SDSBlock[kkk].Cg_x);
				fSpeed_Cir_y[kkk] = (FLOAT) (g_VLD_Params.Cir[kkk].Cg_y
						- g_VLD_Params.SDSBlock[kkk].Cg_y);
				memcpy(&g_VLD_Params.SDSBlock[kkk], &g_VLD_Params.Cir[kkk],
						sizeof(AdasCircleT));
			}
			return 3;		//�������е�����Բ������ض�������
		}
		//return nCirNum;//�������е�����Բ������ض�������chengjs20141014 ����
	}

	//chengjs20141014 ��>�ұߵ�4��ΪUsedCircleNum,���ֹ�����ʱ������ָ���ƵĴ���
	if (nCirNum > g_VLD_Params.UsedCircleNum)		//����3���������������ĵ�ͱ���
	{
		//return SDSCirNum;
		return 4;
	}
	//there are many problems need to solve
	//so we must avoid this step to improve the accuracy
	//����Ƶĸ���������������(����ֻ���Ǵ���3�����)//chengjs20140928 ����3������������ʱ����
	if (SDSCirNum > g_VLD_Params.UsedCircleNum)	//chengjs20141014 modify 3->UsedCircleNum
	{

		//�м�Ƶĸ�������3��ֱ�ӱ���
		//��������Բ�ĸ���
		for (i = 0; i < SDSCirNum; i++)
		{
			if (g_VLD_Params.Cir[i].Cg_y > nCenterCircleTop_Y
					&& g_VLD_Params.Cir[i].Cg_y < nCenterCircleLow_Y)
			{
				CenterList[nCenterNum] = i;	//if 3 circle all enter the center how to do
				nCenterNum++;
			}
		}

	}

	return -1;		//SDSCirNum;

}

INT JudgeSDSCircle(LPMV_MATRIX_S SrcImg, INT SDSCirNum, INT *CirFlag)
{
	INT nCirNum = SDSCirNum;
	INT i = MVZERO;
	FLOAT valueGood;		//fCirGood=1.3
	FLOAT valueWaring;		//fCirWarning = 1.5
	BOOL BadCir[3];

	//INT Cir0Flag=MVZERO;
	//INT Cir1Flag=MVZERO;
	//INT Cir2Flag=MVZERO;
//	INT Cir3Flag=MVZERO;//chengjs20140928 add
//	INT Cir4Flag=MVZERO;//chengjs20140928 add

	INT ImWidth = SrcImg->iCols;
	INT ImHeight = SrcImg->iRows;

//	INT kkk=MVZERO;

	double radx;
	double rady;

	FLOAT fSpeed_Cir_x[5];
	FLOAT fSpeed_Cir_y[5];

	double Dif1And0_Y;
	double Dif2And1_Y;

	double Dif1And0_X;
	double Dif2And1_X;

	//INT nSearchLeft_X        = (INT)(ImWidth*SDS_dParams.ROI_startX);/*ImWidth/6;*///�������������4�ı���
//	INT nSearchRight_X       = (INT)(ImWidth*SDS_dParams.ROI_endX);/*9*ImWidth/10;*///��������
	INT nCenterCircleLeft_X = (INT) (ImWidth * SDS_dParams.ROI_startX);/*ImHeight/3;*///����Բ�жϱ�׼
	INT nCenterCircleRight_X = (INT) (ImWidth * SDS_dParams.ROI_endX);/*2*ImHeight/3;*/	//����Բ�жϱ�׼advised by mao at 7.6  448---360

	//INT nMiddleIndex=MVZERO;
	//INT nRightIndex=MVZERO;
	//INT nLeftIndex=MVZERO;
	//�����������ڵ�����Բ������һ����ֱ���ҵ�����Բ���������������ҵ�һ�����������ͱ���

	INT nCenterNum = MVZERO;
	INT CenterList[30];

	INT nRightCenterCir = MVZERO;
	INT j = MVZERO;

//	int nTempMiddle =MVZERO;
	//int nTempRight  = MVZERO;
	//int nTempLeft   = MVZERO;

	double difLM_X = 0.0;
	double difRM_X = 0.0;
	double difRL_X = 0.0;

	double difRM_Y = 0.0;
	double difLM_Y = 0.0;

	double difLR_R = 0.0;

	for (i = 0; i < nCirNum; i++)
	{
		//chengjs20121017Ŀǰ���ֵΪ52���������ж�ֵΪ60������ֵ�ǲ��ǽ�һ����С
		if (g_VLD_Params.Cir[i].Cir_r > SDS_dParams.nBiggestRaduis)
		{
			return -2;
		}
	}
	//���������Բ�ĵĸ���
	if (SDSCirNum == 3)
	{
		//Բ�뾶����
		//BOOL BadCir[3];
		memset(BadCir, 0, 3 * sizeof(BOOL));
		valueGood = SDS_dParams.fCirGood_1;		//fCirGood=1.3
		valueWaring = SDS_dParams.fCirWarning_1;		//fCirWarning = 1.5

		//Ϊʲô���ұߵƶ������б�׼???��ʵ���Ҳһ��
		//�ұߵ�
		if (g_VLD_Params.Cir[0].Cir_r > SDS_dParams.fCirRadius_1)
		{
			valueGood = SDS_dParams.fCirGood_2;		//fCirGood_R20 = 1.4
			valueWaring = SDS_dParams.fCirWarning_2;	//fCirWarning_R20 = 1.8
		}
		if (g_VLD_Params.Cir[0].Cir_r > SDS_dParams.fCirRadius_2)
		{
			valueGood = SDS_dParams.fCirGood_3;		//fCirGood_R25=1.5
			valueWaring = SDS_dParams.fCirWarning_3;	//fCirWarning_R25 = 1.9
		}
		//Բ�뾶����Ʒ��
		if (g_VLD_Params.Cir[0].Cir_f > valueGood
				&& g_VLD_Params.Cir[0].Cir_f < valueWaring)
		{
			CirFlag[0] = 1;
		} else if (g_VLD_Params.Cir[0].Cir_f >= valueWaring)
		{
			CirFlag[0] = 2;
			BadCir[0] = TRUE;
		} else
		{
			CirFlag[0] = 0;
		}
		//�м��
		if (g_VLD_Params.Cir[1].Cir_f
				> (valueGood + SDS_dParams.fCenterCirGoodOffset)
				&& g_VLD_Params.Cir[1].Cir_f
						< (valueWaring + SDS_dParams.fCenterCirWarningOffset))
		{
			CirFlag[1] = 1;
		} else if (g_VLD_Params.Cir[1].Cir_f
				>= valueWaring + SDS_dParams.fCenterCirWarningOffset)
		{
			CirFlag[1] = 2;
			BadCir[1] = TRUE;
		} else
		{
			CirFlag[1] = 0;
		}
		//��ߵ�
		if (g_VLD_Params.Cir[2].Cir_f > valueGood
				&& g_VLD_Params.Cir[2].Cir_f < valueWaring)
		{
			CirFlag[2] = 1;
		} else if (g_VLD_Params.Cir[2].Cir_f >= valueWaring)
		{
			CirFlag[2] = 2;
			BadCir[2] = TRUE;
		} else
		{
			CirFlag[2] = 0;
		}
		for (i = 0; i < 3; i++)
		{
			radx = ((double) (g_VLD_Params.Cir[i].bt.right
					- g_VLD_Params.Cir[i].bt.left)) / 2;
			rady = ((double) (g_VLD_Params.Cir[i].bt.low
					- g_VLD_Params.Cir[i].bt.up)) / 2;
			if ((g_VLD_Params.Cir[i].bt.area < SDS_dParams.nSmallestRaduis)
					|| g_VLD_Params.Cir[i].Cir_r < SDS_dParams.nSmallestRaduis
					|| (fabs(radx - rady) > SDS_dParams.Judge_xyOffset)
					|| g_VLD_Params.Cir[i].Cir_r > SDS_dParams.nBiggestRaduis)
			{
				BadCir[i] = TRUE;
			}

		}
		if (BadCir[0])		//�ұߵ�
		{
			return 9;
		}
		if (BadCir[1])		//�м��
		{
			return 10;
		}
		if (BadCir[2])		//��ߵ�
		{
			return 11;
		}

		//����Բ�Ĺ�ϵ

		//����Բ��ֵ���� ,ͨ������Բ֮��İ뾶��ϵ
		Dif1And0_Y = g_VLD_Params.Cir[1].Cg_y - g_VLD_Params.Cir[0].Cg_y;
		Dif2And1_Y = g_VLD_Params.Cir[2].Cg_y - g_VLD_Params.Cir[1].Cg_y;

		Dif1And0_X = g_VLD_Params.Cir[1].Cg_x - g_VLD_Params.Cir[0].Cg_x;
		Dif2And1_X = g_VLD_Params.Cir[2].Cg_x - g_VLD_Params.Cir[1].Cg_x;
		if (g_VLD_Params.Cir[1].Cg_x < nCenterCircleLeft_X
				|| g_VLD_Params.Cir[1].Cg_x > nCenterCircleRight_X
				|| abs(g_VLD_Params.Cir[0].Cir_r - g_VLD_Params.Cir[2].Cir_r)
						> SDS_dParams.f_RDifSideCir ||	//f_RDifSideCirΪ0.7

				Dif1And0_Y > Dif1And0_X || Dif2And1_Y > Dif2And1_X)

		{
			return 12;
		}

		if (nCirNum == 3)
		{

			if (g_VLD_Params.IsZeroSpeed == FALSE)
			{
				/*		if(Temp == FALSE)
				 {
				 CString SpeedInfo;
				 SpeedInfo.Format("Cir[2]:%f,SDSBlock[2]:%f,speed:%f\n",Cir[1].Cg_x,SDSBlock[1].Cg_x,(Cir[1].Cg_x-SDSBlock[1].Cg_x));
				 TRACE(SpeedInfo);
				 SpeedInfo.Format("LastSpeed:%f\n",fSpeed_Cir_x[1]);
				 TRACE(SpeedInfo);

				 Temp = TRUE;
				 }*/
				fSpeed_Cir_x[0] = (float) (g_VLD_Params.Cir[0].Cg_x
						- g_VLD_Params.SDSBlock[0].Cg_x);
				fSpeed_Cir_y[0] = (float) (g_VLD_Params.Cir[0].Cg_y
						- g_VLD_Params.SDSBlock[0].Cg_y);
				fSpeed_Cir_x[1] = (float) (g_VLD_Params.Cir[1].Cg_x
						- g_VLD_Params.SDSBlock[1].Cg_x);
				fSpeed_Cir_y[1] = (float) (g_VLD_Params.Cir[1].Cg_y
						- g_VLD_Params.SDSBlock[1].Cg_y);
				fSpeed_Cir_x[2] = (float) (g_VLD_Params.Cir[2].Cg_x
						- g_VLD_Params.SDSBlock[2].Cg_x);
				fSpeed_Cir_y[2] = (float) (g_VLD_Params.Cir[2].Cg_y
						- g_VLD_Params.SDSBlock[2].Cg_y);
				//				if(b_SearchTotalImage)
				//				{
				//					CString SpeedInfo;
				//					SpeedInfo.Format("Cir[1].Cg_x:%f,SDSBlock[1].Cg_x:%f\n",Cir[1].Cg_x,SDSBlock[1].Cg_x);
				////					TRACE(SpeedInfo);
				//					OutputDebugString(SpeedInfo);
				//					SpeedInfo.Format("fSpeed_Cir_x[1]:%f\n",fSpeed_Cir_x[1]);
				//
				//					OutputDebugString(SpeedInfo);
				//
				//					Temp = FALSE;
				//			
				//				}

			}

			//Temp = TRUE;
			//CString SpeedInfo;
			//SpeedInfo.Format("SecondSpeed:%f\n",fSpeed_Cir_x[1]);
			//TRACE(SpeedInfo);
			//OutputDebugString(SpeedInfo);
			memcpy(&g_VLD_Params.SDSBlock[0], &g_VLD_Params.Cir[0],
					sizeof(AdasCircleT));
			memcpy(&g_VLD_Params.SDSBlock[1], &g_VLD_Params.Cir[1],
					sizeof(AdasCircleT));
			memcpy(&g_VLD_Params.SDSBlock[2], &g_VLD_Params.Cir[2],
					sizeof(AdasCircleT));

		}

		return nCirNum;			//�������е�����Բ������ض�������
	}
	//there are many problems need to solve
	//so we must avoid this step to improve the accuracy
	//����Ƶĸ���������������(����ֻ���Ǵ���3�����)
	if (SDSCirNum > 3)
	{
		INT nMiddleIndex = MVZERO;
		INT nRightIndex = MVZERO;
		INT nLeftIndex = MVZERO;
		//�����������ڵ�����Բ������һ����ֱ���ҵ�����Բ���������������ҵ�һ�����������ͱ���

		nCenterNum = 0;
//		CenterList[30];

		//�м�Ƶĸ�������3��ֱ�ӱ���
		//��������Բ�ĸ���
		for (i = 0; i < SDSCirNum; i++)
		{
			if (g_VLD_Params.Cir[i].Cg_x > nCenterCircleLeft_X
					&& g_VLD_Params.Cir[i].Cg_x < nCenterCircleRight_X)
			{
				CenterList[nCenterNum] = i;	//if 3 circle all enter the center how to do
				nCenterNum++;
			}
		}

		if (nCenterNum >= 4)		//����3���������������ĵ�ͱ���
		{
			return 4; //return SDSCirNum;�޸�by lmt 15��2��28
		}
		//���ֻ��һ����������������Ϊ������ĵ�����¶��еƣ������ǣ�����
		if (nCenterNum == 1)
		{
			nMiddleIndex = CenterList[0];
			nRightIndex = nMiddleIndex - 1;
			nLeftIndex = nMiddleIndex + 1;
			if ((nLeftIndex < SDSCirNum) && (nRightIndex >= 0))
			{
				memcpy(&g_VLD_Params.SDSBlock[0],
						&g_VLD_Params.Cir[nRightIndex].bt, sizeof(AdasBlobT));
				memcpy(&g_VLD_Params.SDSBlock[1],
						&g_VLD_Params.Cir[nMiddleIndex].bt, sizeof(AdasBlobT));
				memcpy(&g_VLD_Params.SDSBlock[2],
						&g_VLD_Params.Cir[nLeftIndex].bt, sizeof(AdasBlobT));
				return 3;
			}
			return SDSCirNum;
		}

		//chengjs20121017���ﻹ�д���ϸ����
		//����������ι��ˣ���ר��д��������

		for (j = 0; j < nCenterNum; j++)
		{
			int nTempMiddle = CenterList[j];
			int nTempRight = nTempMiddle - 1;
			int nTempLeft = nTempMiddle + 1;
			if (nTempRight >= 0 && nTempLeft < SDSCirNum)
			{
				difLM_X = abs(
						g_VLD_Params.Cir[nTempLeft].Cg_x
								- g_VLD_Params.Cir[nTempMiddle].Cg_x);
				difRM_X = abs(
						g_VLD_Params.Cir[nTempRight].Cg_x
								- g_VLD_Params.Cir[nTempMiddle].Cg_x);
				difRL_X = abs(
						g_VLD_Params.Cir[nTempRight].Cg_x
								- g_VLD_Params.Cir[nTempLeft].Cg_x);

				difRM_Y = abs(
						g_VLD_Params.Cir[nTempRight].Cg_y
								- g_VLD_Params.Cir[nTempMiddle].Cg_y);
				difLM_Y = abs(
						g_VLD_Params.Cir[nTempLeft].Cg_y
								- g_VLD_Params.Cir[nTempMiddle].Cg_y);

				difLR_R = abs(
						g_VLD_Params.Cir[nTempLeft].Cir_r
								- g_VLD_Params.Cir[nTempRight].Cir_r);

				if (difLR_R < SDS_dParams.f_RDifSideCir
						&& abs(difRM_Y - difLM_Y) < 5 && difLM_X < 5
						&& difRM_X < 5 && difRL_X < 5)

				{
					nMiddleIndex = nTempMiddle;
					nRightIndex = nTempRight;
					nLeftIndex = nTempLeft;
					nRightCenterCir++;
				}
			}
		}
		if (nRightCenterCir == 1)
		{
			memcpy(&g_VLD_Params.SDSBlock[0], &g_VLD_Params.Cir[nRightIndex].bt,
					sizeof(AdasBlobT));
			memcpy(&g_VLD_Params.SDSBlock[1],
					&g_VLD_Params.Cir[nMiddleIndex].bt, sizeof(AdasBlobT));
			memcpy(&g_VLD_Params.SDSBlock[2], &g_VLD_Params.Cir[nLeftIndex].bt,
					sizeof(AdasBlobT));
			return 3;
		}
	}
	return SDSCirNum;

}

VOID MaopaoSortFrom7()
{
	float Sort_source[7];
	float Position[7] =
	{ 0, 1, 2, 3, 4, 5, 6 };
	int i, j;
	int SortLength = 7;
	int Temp, Temp2;
	AdasCircleT Tempcir;
	Sort_source[0] = g_VLD_Params.Cir[0].Cg_x;
	Sort_source[1] = g_VLD_Params.Cir[1].Cg_x;
	Sort_source[2] = g_VLD_Params.Cir[2].Cg_x;
	Sort_source[3] = g_VLD_Params.Cir[3].Cg_x;
	Sort_source[4] = g_VLD_Params.Cir[4].Cg_x;
	Sort_source[5] = g_VLD_Params.Cir[5].Cg_x;
	Sort_source[6] = g_VLD_Params.Cir[6].Cg_x;

	for (j = 0; j < SortLength; j++)
	{
		for (i = 0; i < SortLength - j - 1; i++)
		{
			if (Sort_source[i] > Sort_source[i + 1])
			{
				Temp = Sort_source[i];
				Sort_source[i] = Sort_source[i + 1];
				Sort_source[i + 1] = Temp;

				Temp2 = Position[i];
				Position[i] = Position[i + 1];
				Position[i + 1] = Temp2;

				Tempcir = g_VLD_Params.Cir[i];
				g_VLD_Params.Cir[i] = g_VLD_Params.Cir[i + 1];
				g_VLD_Params.Cir[i + 1] = Tempcir;
			}
		}

	}
	if (g_VLD_Params.Cir[0].Cir_y > g_VLD_Params.Cir[1].Cir_y)
	{
		Tempcir = g_VLD_Params.Cir[0];
		g_VLD_Params.Cir[0] = g_VLD_Params.Cir[1];
		g_VLD_Params.Cir[1] = Tempcir;
	}
	if (g_VLD_Params.Cir[5].Cir_y > g_VLD_Params.Cir[6].Cir_y)
	{
		Tempcir = g_VLD_Params.Cir[5];
		g_VLD_Params.Cir[5] = g_VLD_Params.Cir[6];
		g_VLD_Params.Cir[6] = Tempcir;
	}
	g_VLD_Params.Cir[2] = g_VLD_Params.Cir[5];
	g_VLD_Params.Cir[3] = g_VLD_Params.Cir[6];
}

//��С���˷����Բ��
BOOL GetCirclePara(LPMV_MATRIX_S EdgeImage, float *xff, float *yff, float *rff,
		float *valff, AdasBlobT*OutBlob, int cnt)
{

	static unsigned short coord_x[1600];
	static unsigned short coord_y[1600];
	int edgecount = cnt;
	INT i = 0;
	INT j = 0;
	INT yyy = 0;
	DSP_boundary_8_xy(EdgeImage->pDataAlign + EdgeImage->iCols,
			EdgeImage->iRows, EdgeImage->iCols, coord_x, coord_y);
	int edgecount_align;
	if (edgecount % 4 == 0)
		edgecount_align = edgecount;
	else
		edgecount_align = (edgecount / 4) * 4 + 4;
	for (i = edgecount; i < edgecount_align; i++)
	{
		coord_x[i] = 0;
		coord_y[i] = 0;
	}
	OutBlob->right = DSP_Max(coord_x, edgecount_align);
	OutBlob->low = DSP_Max(coord_y, edgecount_align);
	for (i = edgecount; i < edgecount_align; i++)
	{
		coord_x[i] = 65535;
		coord_y[i] = 65535;
	}
	OutBlob->left = DSP_Min(coord_x, edgecount_align);
	OutBlob->up = DSP_Min(coord_y, edgecount_align);
	//INT Row=EdgeImage->iRows;
	//INT Col=EdgeImage->iCols;

	float x2 = 0;
	float x1 = 0;
	float y2 = 0;
	float y1 = 0;
	float x1y1 = 0;
	float x1y2 = 0;
	float x3 = 0;
	float y3 = 0;
	float x2y1 = 0;

	int N;
	float p;
	float q;
	float s;
	float t;
	float u;

	float v;

	float valfferro;

	float rr;

//	System_printf("the edgecount is %d\n",edgecount);
	for (i = 0; i < edgecount; i++)
	{
		x1 = x1 + coord_x[i];
		x2 = x2 + coord_x[i] * coord_x[i];
		y1 = y1 + coord_y[i];
		y2 = y2 + coord_y[i] * coord_y[i];
		x1y1 = x1y1 + coord_x[i] * coord_y[i];
		x1y2 = x1y2 + coord_x[i] * coord_y[i] * coord_y[i];
		x2y1 = x2y1 + coord_x[i] * coord_x[i] * coord_y[i];
		x3 = x3 + coord_x[i] * coord_x[i] * coord_x[i];
		y3 = y3 + coord_y[i] * coord_y[i] * coord_y[i];
	}
	N = (double) edgecount;
	p = N * x2 - x1 * x1;
	q = N * y2 - y1 * y1;
	s = N * x3 + N * x1y2 - (x2 + y2) * x1;
	t = N * y3 + N * x2y1 - (x2 + y2) * y1;
	u = N * x1y1 - x1 * y1;

	(*xff) = (s * q - t * u) / 2 / (p * q - u * u);
	(*yff) = (t * p - s * u) / 2 / (p * q - u * u);

	v = 1 / (double) N * (2 * (*xff) * x1 + 2 * (*yff) * y1 - (x2 + y2));
	(*rff) = sqrtsp((*xff) * (*xff) + (*yff) * (*yff) - v);

	valfferro = 0;
	for (i = 0; i < edgecount; i++)
	{
		rr = (coord_x[i] - (*xff)) * (coord_x[i] - (*xff))
				+ (coord_y[i] - (*yff)) * (coord_y[i] - (*yff));
		rr = sqrtsp(rr);
		valfferro = valfferro + (rr - (*rff)) * (rr - (*rff));
//		valfferro = valfferro + (rr - (*rff) * (*rff)) * (rr - (*rff) * (*rff));
	}
	(*valff) = valfferro / (double) N;			/// (*rff) / (*rff);
//	memset(coord_x,0,sizeof(unsigned short)*edgecount_align);
//	memset(coord_y,0,sizeof(unsigned short)*edgecount_align);

	return 1;
}

VOID ResetDefaultSetting()
{
	g_VLD_Params.fSpeed_Cir_x[0] = 0;
	g_VLD_Params.fSpeed_Cir_x[1] = 0;
	g_VLD_Params.fSpeed_Cir_x[2] = 0;
	g_VLD_Params.fSpeed_Cir_x[3] = 0;
	g_VLD_Params.fSpeed_Cir_x[4] = 0;
	g_VLD_Params.fSpeed_Cir_y[0] = 0;
	g_VLD_Params.fSpeed_Cir_y[1] = 0;
	g_VLD_Params.fSpeed_Cir_y[2] = 0;
	g_VLD_Params.fSpeed_Cir_y[3] = 0;
	g_VLD_Params.fSpeed_Cir_y[4] = 0;
	memset(g_VLD_Params.Cir, 0, sizeof(AdasCircleT) * 100);
	memset(g_VLD_Params.SDSBlock, 0, sizeof(AdasCircleT) * 5);

	g_VLD_Params.IsZeroSpeed = TRUE;
	g_VLD_Params.b_SearchTotalImage = TRUE;				//��Ҫ���½�������
}

//Ҳ�����е��κ�һ���ֱ�����������ͼ���Ե10
INT GetAbideTime(INT ImWidth, INT ImHeight)
{
	INT abideTime = 5;
	INT kkk;
	//INT ImWidth=SrcImg->iCols;
	//INT ImHeight=SrcImg->iRows;

	for (kkk = 0; kkk < g_VLD_Params.UsedCircleNum; kkk++)
	{
		if ((g_VLD_Params.SDSBlock[kkk].bt.left - 10) < 0
				|| (ImWidth - g_VLD_Params.SDSBlock[kkk].bt.right) < 10
				|| (g_VLD_Params.SDSBlock[kkk].bt.up - 10) < 0
				|| (ImHeight - g_VLD_Params.SDSBlock[kkk].bt.low) < 10)
		{
			abideTime = g_VLD_Params.nAbideNum;	///�Ķ���Ϊ��ֹԲ�ص�
			break;
		}
	}
	return abideTime;
}

/* Reset Initial Params */
VOID VLD_Restart(CameraParams *pCameraP, VehicleParams *pVehicleP)
{
	//=========Data definition=========//

	//==========Implementation==========//
#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_Restart && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("[ZCD] ZCD_Restart() Start.\r\n");
#endif

	if (TESTBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
			VLDCF_SYSTEM_INIT_STATE_FLAG) == FALSE
			|| TESTBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
					VLDCF_RESET_INIT_STATE_FLAG) != FALSE)
	{
		return;
	}

	//Set flag
	SETBIT(g_VLD_Params.m_sVehicleLicenseDetParams.dVLDControlFlag,
			VLDCF_RESET_INIT_STATE_FLAG);

	g_VLD_Params.m_iRunState = ADAS_MODEL_STATE_RESTART;

#if ADAS_LOG_MODEL_VLD && ADAS_LOG_FUNC_VLD_Restart && ADAS_LOG_LEVEL_COMMON_VLD
	ADAS_LOG_ProcessInfo("[ZCD] ZCD_Restart() End.\r\n");
#endif

}	//ZCD_Restart()

ADAS_ModelRunState VLD_GetRunState()
{
	return g_VLD_Params.m_iRunState;
}	//GB_ZCD_GetRunState

VOID VLD_SetRunState(ADAS_ModelRunState iRunState)
{
	g_VLD_Params.m_iRunState = iRunState;
}	//GB_ZCD_SetRunState

VOID VLD_Pause()
{
	g_VLD_Params.m_iRunState = ADAS_MODEL_STATE_PAUSE;
}	//ZCD_Pause

#ifdef _C66X_
VOID DSP_InitHistogram(DSP_HistParams* histparams)
{
	int i;
	for (i = 0; i < 256; i++)
		histparams->binedge[i] = i;
	memset(histparams->histarray, 0, sizeof(unsigned char) * 257);
	memset(histparams->H1, 0, sizeof(unsigned short) * 256);
	memset(histparams->H2, 0, sizeof(unsigned short) * 256);
	memset(histparams->H3, 0, sizeof(unsigned short) * 256);
	VLIB_histogram_1D_Init_U8(histparams->binedge, 256, histparams->histarray);
}

VOID DSP_thr_min2max(const unsigned char *in_data,
		unsigned char*restrict out_data, short cols, short rows,
		unsigned char threshold)
{
	int i, pixels = rows * cols;
	unsigned thththth;

	_nassert((int) in_data % 8 == 0);
	_nassert((int) out_data % 8 == 0);
	_nassert(pixels % 16 == 0);
	_nassert(pixels >= 16);

	thththth = _pack2(threshold, threshold);
	thththth = _packl4(thththth, thththth);

	/* -------------------------------------------------------------------- */
	/*  Step through input image copying pixels to the output.  If the      */
	/*  pixels are above our threshold, set them to 255.                    */
	/* -------------------------------------------------------------------- */
#pragma MUST_ITERATE(4,,4);
#pragma UNROLL(4);

	for (i = 0; i < pixels; i += 4)
	{
		unsigned p3p2p1p0, x3x2x1x0;

		p3p2p1p0 = _amem4_const(&in_data[i]);
		x3x2x1x0 = _xpnd4(_cmpgtu4(p3p2p1p0, thththth));

		_amem4(&out_data[i]) = x3x2x1x0;
	}
}

VOID DSP_boundary_8(const unsigned char*restrict i_data, int rows, int cols,
		unsigned int*restrict o_coord, int* sum)
{
	*sum = 0;
	int i, p, p0, p1, p2, p3;
	_nassert(cols % 4 == 0);
	_nassert(cols >= 16);
	_nassert(cols <= 32764);
	_nassert(rows <= 32764);
	_nassert((int) i_data % 8 == 0);
	for (i = 0; i < rows * cols; i += 4)
	{
		p = _amem4_const(&i_data[i]);
		p0 = (p) & 0xFF;
		p1 = (p >> 8) & 0xFF;
		p2 = (p >> 16) & 0xFF;
		p3 = (p >> 24) & 0xFF;
		if (p0)
		{
			*o_coord++ = i;
			*sum += 1;
		}
		if (p1)
		{
			*o_coord++ = i + 1;
			*sum += 1;
		}
		if (p2)
		{
			*o_coord++ = i + 2;
			*sum += 1;
		}
		if (p3)
		{
			*o_coord++ = i + 3;
			*sum += 1;
		}
	}
}

VOID DSP_boundary_8_xy(const unsigned char*restrict i_data, int rows, int cols,
		unsigned short*restrict coord_x, unsigned short* restrict coord_y)
{
	int i, x, p, p0, p1, p2, p3;
	int c0, c1, c2, c3, r_step;

	_nassert(cols % 4 == 0);
	_nassert(cols >= 16);
	_nassert(cols <= 32764);
	_nassert(rows <= 32764);
	_nassert((int) i_data % 8 == 0);
	_nassert((int) coord_x % 8 != (int) coord_y % 8);

	r_step = 0x10000 - cols;
#   pragma MUST_ITERATE(1, 32764/4*32764, 1);
	for (i = x = c0 = 0; i < rows * cols; i += 4, x += 4)
	{
		if (x == cols)
		{
			x = 0;
			c0 += r_step;
		}

		p = _amem4_const(&i_data[i]);

		p0 = (p) & 0xFF;
		p1 = (p >> 8) & 0xFF;
		p2 = (p >> 16) & 0xFF;
		p3 = (p >> 24) & 0xFF;

		c1 = c0 + 1;
		c2 = c0 + 2;
		c3 = c0 + 3;

		if (p0)
		{
			*coord_x++ = c0 & 0xFFFF;
			*coord_y++ = (c0 >> 16) & 0xFFFF;
		}
		if (p1)
		{
			*coord_x++ = c1 & 0xFFFF;
			*coord_y++ = (c1 >> 16) & 0xFFFF;
		}
		if (p2)
		{
			*coord_x++ = c2 & 0xFFFF;
			*coord_y++ = (c2 >> 16) & 0xFFFF;
		}
		if (p3)
		{
			*coord_x++ = c3 & 0xFFFF;
			*coord_y++ = (c3 >> 16) & 0xFFFF;
		}

		c0 += 4;
	}
}

unsigned short DSP_Max(const unsigned short*x, int nx)
{
	int i;
	long long x0123;
	unsigned int x01, x23;
	unsigned short x0, x1, x2, x3, max0, max1, max2, max3;
	max0 = max1 = max2 = max3 = 0;
	for (i = 0; i < nx; i += 4)
	{
		x0123 = _amem8_const((void*) &x[i]);
		x01 = _hill(x0123);
		x23 = _loll(x0123);
		x0 = (x01 >> 16) & 0xFFFF;
		x1 = x01 & 0xFFFF;
		x2 = (x23 >> 16) & 0xFFFF;
		x3 = x23 & 0xFFFF;
		max0 = x0 > max0 ? x0 : max0;
		max1 = x1 > max1 ? x1 : max1;
		max2 = x2 > max2 ? x2 : max2;
		max3 = x3 > max3 ? x3 : max3;
	}
	max1 = max0 > max1 ? max0 : max1;
	max3 = max2 > max3 ? max2 : max3;
	if (max1 > max3)
		return max1;
	else
		return max3;
}

unsigned short DSP_Min(const unsigned short*x, int nx)
{
	int i;
	long long x0123;
	unsigned int x01, x23;
	unsigned short x0, x1, x2, x3, min0, min1, min2, min3;
	min0 = min1 = min2 = min3 = 65535;
	for (i = 0; i < nx; i += 4)
	{
		x0123 = _amem8_const((void*) &x[i]);
		x01 = _hill(x0123);
		x23 = _loll(x0123);
		x0 = (x01 >> 16) & 0xFFFF;
		x1 = x01 & 0xFFFF;
		x2 = (x23 >> 16) & 0xFFFF;
		x3 = x23 & 0xFFFF;
		min0 = x0 < min0 ? x0 : min0;
		min1 = x1 < min1 ? x1 : min1;
		min2 = x2 < min2 ? x2 : min2;
		min3 = x3 < min3 ? x3 : min3;
	}
	min1 = min0 < min1 ? min0 : min1;
	min3 = min2 < min3 ? min2 : min3;
	if (min1 < min3)
		return min1;
	else
		return min3;
}
VOID SortLabel(int num)
{
	if (num < 3 || num > 10)
		return;
	int i, j;
	for (i = 0; i < num; i++)
	{
		for (j = 0; j < i; j++)
		{
			int a, b;
			a = g_VLD_Params.Cir[i].Cir_x;
			b = g_VLD_Params.Cir[j].Cir_x;
			if (a < b)
			{
				SwapLabel(&g_VLD_Params.Cir[i], &g_VLD_Params.Cir[j]);
			}
		}
	}
}

VOID SwapLabel(AdasCircleT* A, AdasCircleT*B)
{
	AdasCircleT temp;
	memcpy(&temp, A, sizeof(AdasCircleT));
	memcpy(A, B, sizeof(AdasCircleT));
	memcpy(B, &temp, sizeof(AdasCircleT));
}
#endif
