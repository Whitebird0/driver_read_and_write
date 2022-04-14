#pragma once
#include <ntifs.h>




typedef NTSTATUS(*FuncCallBack)(HANDLE hFile, PVOID  pv1, PVOID  pv2, PVOID pv3);
typedef struct _DataPake{
	//�ص�������ȷ����˭���õ�ID
	ULONG64 ulInfoId;
	//ʵ�����ֹ��ܵ�ID
	ULONG64 ulFunctionID;
	//����
	ULONG64 ulData;
	//�ߴ�
	ULONG64 ulSize;
}DataPake, * PDataPake;

typedef struct _RegisterAttrCallback {
	FuncCallBack QueryAttributeInformation;
	FuncCallBack SetAttributeInformation;
}RegisterAttrCallback,*PRegisterAttrCallback;

typedef NTSTATUS (*ExRegisterAttributeInformationCallbackFunc)(PRegisterAttrCallback prcCallBack);
typedef NTSTATUS(*whCallBackFunc)(PDataPake ExchangeData);

//ͨ��ע��
NTSTATUS RegisterCommunication(whCallBackFunc whFunc); \
//ж��ͨ��
VOID UnRegisterCommunication();
