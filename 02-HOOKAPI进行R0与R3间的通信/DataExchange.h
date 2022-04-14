#pragma once
#include <ntifs.h>




typedef NTSTATUS(*FuncCallBack)(HANDLE hFile, PVOID  pv1, PVOID  pv2, PVOID pv3);
typedef struct _DataPake{
	//回调函数中确定是谁调用的ID
	ULONG64 ulInfoId;
	//实现那种功能的ID
	ULONG64 ulFunctionID;
	//数据
	ULONG64 ulData;
	//尺寸
	ULONG64 ulSize;
}DataPake, * PDataPake;

typedef struct _RegisterAttrCallback {
	FuncCallBack QueryAttributeInformation;
	FuncCallBack SetAttributeInformation;
}RegisterAttrCallback,*PRegisterAttrCallback;

typedef NTSTATUS (*ExRegisterAttributeInformationCallbackFunc)(PRegisterAttrCallback prcCallBack);
typedef NTSTATUS(*whCallBackFunc)(PDataPake ExchangeData);

//通信注册
NTSTATUS RegisterCommunication(whCallBackFunc whFunc); \
//卸载通信
VOID UnRegisterCommunication();
