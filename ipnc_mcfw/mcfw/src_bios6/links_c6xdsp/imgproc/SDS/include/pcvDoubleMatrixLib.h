
/*
* @(#)$Id: pcvDoubleMatrixLib.h 1007 2012-12-19 03:13:32Z LvGangXiao $ 
* @(#) Declaration file of  Double Matrix Lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/

#ifndef PCVDOUBLEMATRIXLIB_H
#define PCVDOUBLEMATRIXLIB_H


#define PCV_DM_EPSILON (0.0000001f)
#define PCV_ITERATION_NUMBER 60

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcvDoubleVectorLib.h"

typedef struct _MV_MATRIX_D_
{
	INT iRows;
	INT iCols;
	INT iLength;
	DOUBLE* pdData;
}MV_MATRIX_D,*LPMV_MATRIX_D;

typedef enum _MATRIX_DIM
{
	MATRIX_DIM_COLUMN = 1,
	MATRIX_DIM_ROW,
} MATRIX_DIM;

#ifdef __cplusplus
extern "C" {
#endif

//�������
INT MV_MatrixSum_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//���������ֵ
INT MV_MatrixMax_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//��������Сֵ
INT MV_MatrixMin_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//��������
INT MV_MatrixSort_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//����ת
INT MV_MatrixFliplr_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD);

//������ƽ��ֵ
INT MV_MatrixMean_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//��������λ��
INT MV_MatrixMedian_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD,MATRIX_DIM iDim);

//����ı�׼��
DOUBLE MV_MatrixSTD_D(LPMV_MATRIX_D psMat,INT iflag);

//�������
INT MV_MatrixSub_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//�������
INT MV_MatrixAdd_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//������
INT MV_MatrixDiv_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//������
INT MV_MatrixDotMul_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//����˷�
INT MV_MatrixMul_D(LPMV_MATRIX_D psMatA, LPMV_MATRIX_D psMatB,LPMV_MATRIX_D psMatC);

//����ת��
INT MV_MatrixTrans_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D psMatD);

//������ƽ����
INT MV_MatrixSqrt_D(LPMV_MATRIX_D psMatS,LPMV_MATRIX_D pMatDr,LPMV_MATRIX_D pMatDi);

//��ȡ����ĳһ�л�һ����Ϊһ������
INT MV_GetMatricesRoworCol_D(LPMV_MATRIX_D psMat,LPMV_VECTOR_D psOutput,INT iIndex,MATRIX_DIM iDim);

//����ĳ�л���תΪvector
INT MV_MatrixToVector_D(LPMV_MATRIX_D psMat,INT iIndex,MATRIX_DIM iDim,LPMV_VECTOR_D psOutput);

//����תΪ����
INT MV_VectorToMatrix_D(LPMV_VECTOR_D psVec,MATRIX_DIM iDim,LPMV_MATRIX_D psOutput);

//�����л�������
DOUBLE MV_MatrixNorm2_D(LPMV_MATRIX_D psMat,INT iIndex,MATRIX_DIM iDim);

//��ʵ�Գƾ��������ֵ����������
//input:psMat
//output:psMat:�Խ���Ϊ����ֵ;psEigenVector:��������
INT MV_MatrixEigen_D(LPMV_MATRIX_D psMat,LPMV_MATRIX_D psEigenVector);

//��������
DOUBLE MV_MatrixNorm_D(LPMV_MATRIX_D psMat);

//�������
INT MV_MatrixInv_D(LPMV_MATRIX_D psMat);

//����ֵ�ֽ�
//input��
//   LPMV_MATRIX_D pmat(mxn)
//output��
//   LPMV_MATRIX_D pmat(mxn):�Խ��߸�������ֵ������Ԫ��Ϊ0
//   LPMV_MATRIX_D psMatL(mxm):�������������
//   LPMV_MATRIX_D psMatR(nxn):�������������
INT MV_MatrixSVD_D(LPMV_MATRIX_D psMat,LPMV_MATRIX_D psMatL,LPMV_MATRIX_D psMatR);

//���������
INT MV_MatrixGeneralInv_D(LPMV_MATRIX_D psMatI,LPMV_MATRIX_D psMatO);


//���������ʽ��ֵ
DOUBLE MV_Matrixdet_D(LPMV_MATRIX_D psMat);

//���������ֵȫΪ0��
LPMV_MATRIX_D MV_MatricesZeros_D(INT iRows,INT iCols,INT iNum);

//���ɵ�λ����ֵȫΪ1��
LPMV_MATRIX_D MV_MatricesOnes_D(INT iRows,INT iCols,INT iNum);

//�ͷž���
void MV_FreeMatrices_D(LPMV_MATRIX_D psMatrics,INT iNum);

DOUBLE MV_MatrixGet_D(const LPMV_MATRIX_D psMatrix, INT iRow, INT iCol);

void MV_MatrixSet_D(LPMV_MATRIX_D psMatrix, INT iRow, INT iCol, DOUBLE dData);

DOUBLE* MV_MatrixGetLinePtr_D(LPMV_MATRIX_D psMatrix, INT iRow);

LPMV_MATRIX_D MV_MatrixCopy_D(LPMV_MATRIX_D psSrc);

void MV_MatrixCopy2_D(LPMV_MATRIX_D psSrc, LPMV_MATRIX_D psDst);

void MV_PrintMatrix_D(LPMV_MATRIX_D psMat);

INT lsqnonlin(void (*func)(DOUBLE *pdP, DOUBLE *pdF, INT iM, INT iN, DOUBLE *pdXdata1, DOUBLE *pdXdata2),
			  LPMV_MATRIX_D psMatH, LPMV_VECTOR_D psVecydata,LPMV_MATRIX_D psMatx, LPMV_MATRIX_D psMatxp);

#ifdef __cplusplus
}
#endif


#endif /* PCVDOUBLEMATRIXLIB_H*/
