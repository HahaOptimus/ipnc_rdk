
/*
* @(#)$Id: pcvDoubleVectorLib.h 1007 2012-12-19 03:13:32Z LvGangXiao $ 
* @(#) Declaration file of  Double Vector Lib
*
* (c) EZ CORPORATION 2015
* All Rights Reserved.
*/
#ifndef PCVDOUBLEVECTORLIB_H
#define PCVDOUBLEVECTORLIB_H

#include "pcvPlatformdef.h"

#define PCV_DM_EPSILON (0.0000001f)
#define PCV_ITERATION_NUMBER 60

typedef struct _MV_VECTOR_D_
{
	INT     iLength;
	DOUBLE  *pdData;
} MV_VECTOR_D, *LPMV_VECTOR_D;

//-----------------------------------------------------------------/
// �궨����
//-----------------------------------------------------------------/
/// <summary>
/// �����ṹ��
/// </summary>
/// <param name="Real">ʵ��</param>
/// <param name="Imag">�鲿</param>
typedef struct _ComplexD
{
	DOUBLE Real;
	DOUBLE Imag;
}ComplexD;


#ifdef __cplusplus
extern "C" {
#endif

//�������
DOUBLE MV_VectorSum_D(LPMV_VECTOR_D psVec);

//���������ֵ
DOUBLE MV_VectorMax_D(LPMV_VECTOR_D psVec,INT *iIndex);

//��������Сֵ
DOUBLE MV_VectorMin_D(LPMV_VECTOR_D psVec,INT *piIndex);

//��������
INT MV_VectorSort_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecD);

//������ת
INT MV_VectorFliplr_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecD);

//������ƽ��ֵ
DOUBLE MV_VectorMean_D(LPMV_VECTOR_D psVec);

//��������λ��
DOUBLE MV_VectorMedian_D(LPMV_VECTOR_D psVec);

//������׼��
DOUBLE MV_VectorSTD_D(LPMV_VECTOR_D psVec,INT iFlag);

//�������
INT MV_VectorSub_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//�������
INT MV_VectorAdd_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//�������
INT MV_VectorDiv_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//�������
INT MV_VectorDotMul_D(LPMV_VECTOR_D psVecA, LPMV_VECTOR_D psVecB,LPMV_VECTOR_D psVecC);

//������ƽ����
INT MV_VectorSqrt_D(LPMV_VECTOR_D psVecS,LPMV_VECTOR_D psVecDr,LPMV_VECTOR_D psVecDi);

//QR���̸�
INT MV_VectorQRRoots_D(LPMV_VECTOR_D psVector,LPMV_VECTOR_D psRRoots,LPMV_VECTOR_D psIRoots);

////QR���̸�2
INT MV_VectorQRRoots2_D(DOUBLE* pdData, INT iNVarl, ComplexD* roots, INT* piRn);

//����ʽ��������
INT MV_VectorPolyval_D(LPMV_VECTOR_D psLPVector,LPMV_VECTOR_D psLPInput,LPMV_VECTOR_D psLPOutput);

//����ʽ��������2
INT MV_VectorPolyval2_D(DOUBLE *pdLPVector,DOUBLE *pdLPInput,DOUBLE *pdLPOutput,INT iLength);

//����2����
DOUBLE MV_VectorNorm_D(LPMV_VECTOR_D psLPVectorD);

//������������ֵȫΪ0��
LPMV_VECTOR_D MV_VectorsZeros_D(INT iLength,INT iNum);

//���ɵ�λ������ֵȫΪ1��
LPMV_VECTOR_D MV_VectorsOnes_D(INT iLength,INT iNum);

//�ͷ�����
void MV_FreeVectors_D(LPMV_VECTOR_D psLPVectorD,INT iNum);

DOUBLE MV_VectorGet_D(const LPMV_VECTOR_D pVector, INT iIndex);

void MV_VectorSet_D(LPMV_VECTOR_D psVector, INT iIndex, DOUBLE dData);

LPMV_VECTOR_D MV_VectorCopy_D(LPMV_VECTOR_D psSrc);

void MV_VectorCopy2_D(LPMV_VECTOR_D psSrc, LPMV_VECTOR_D psDst);

void MV_PrintVector_D(LPMV_VECTOR_D psVec);

#ifdef __cplusplus
}
#endif


#endif /* PCVDOUBLEMATRIXLIB_H*/
