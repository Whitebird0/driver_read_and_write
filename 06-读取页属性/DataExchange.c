#include "DataExchange.h"

FuncCallBack ExpDisQueryAttrbuteInformationOld = NULL;
FuncCallBack ExpDisSetAttrbuteInformationOld = NULL;
whCallBackFunc ExchangeCallBack = NULL;
PULONG gQueryCallBack = NULL;
NTSTATUS whQueryAttributeInformation(HANDLE hFile, PVOID  pv1, PVOID  pv2, PVOID pv3)
{
	if (MmIsAddressValid(pv1))
	{
		PDataPake m_Pack = (PDataPake)pv1;
		if (m_Pack->ulInfoId==0x12138111)
		{
		
			ExchangeCallBack(m_Pack);
		}
		else {
			if (ExpDisQueryAttrbuteInformationOld)
			{
				return ExpDisQueryAttrbuteInformationOld( hFile,  pv1,  pv2,  pv3);
			}
		}
	}
	return STATUS_SUCCESS;
}
NTSTATUS whSetAttributeInformation(HANDLE hFile, PVOID  pv1, PVOID  pv2, PVOID pv3) {
	if (MmIsAddressValid(pv1))
	{
		PDataPake m_Pack = (PDataPake)pv1;
		if (m_Pack->ulInfoId == 0x12138111)
		{
			//调用通信分发
			ExchangeCallBack(m_Pack);
		}
		else {
			if (ExpDisSetAttrbuteInformationOld)
			{
				return ExpDisSetAttrbuteInformationOld(hFile, pv1, pv2, pv3);
			}
		}
	}
	return STATUS_SUCCESS;
}



NTSTATUS RegisterCommunication(whCallBackFunc whFunc)
{
	UNICODE_STRING usFunctionName = { 0 };
	RtlInitUnicodeString(&usFunctionName, L"ExRegisterAttributeInformationCallback");
	//获取ExRegisterAttributeInformationCallback的地址
	PUCHAR pucFunctionAddress = MmGetSystemRoutineAddress(&usFunctionName);
	//获取第一个回调
	ULONG64 ulCallBackAddress = *(PULONG)(pucFunctionAddress + 0Xd + 3);
	PULONG64 ExpDisQueryAttrbuteInformation = (PULONG64)((pucFunctionAddress + 0Xd + 7)+ ulCallBackAddress);
	ExpDisQueryAttrbuteInformationOld = (FuncCallBack)ExpDisQueryAttrbuteInformation[0];
	ExpDisSetAttrbuteInformationOld = (FuncCallBack)ExpDisQueryAttrbuteInformation[1];
	ExpDisQueryAttrbuteInformation[0] = 0;
	ExpDisQueryAttrbuteInformation[1] = 0;

	RegisterAttrCallback whCall = { 0 };
	whCall.QueryAttributeInformation = whQueryAttributeInformation;
	whCall.SetAttributeInformation = whSetAttributeInformation;
	
	ExRegisterAttributeInformationCallbackFunc pExRegisterAttInforCallbackFunc = (ExRegisterAttributeInformationCallbackFunc)pucFunctionAddress;
	NTSTATUS ntStatus = pExRegisterAttInforCallbackFunc(&whCall);
	if (NT_SUCCESS(ntStatus)) {
		ExchangeCallBack = whFunc;
		gQueryCallBack = ExpDisQueryAttrbuteInformation;
	}
	return ntStatus;
}

VOID UnRegisterCommunication()
{
	if (gQueryCallBack)
	{
		gQueryCallBack[0] = ExpDisQueryAttrbuteInformationOld;
		gQueryCallBack[1] = ExpDisSetAttrbuteInformationOld;
	}
}
