#include<ntifs.h>
#include"GetModuleBase.h"
#include"DataExchange.h"
#include"DataExchangeStruct.h"
#define GET_MODULE_BASE 0x1
VOID DriverUnload(PDRIVER_OBJECT pDriverObject) {
	UnRegisterCommunication();
}
NTSTATUS DisPatch(PDataPake ExchangeData) {
	PVOID pvBuffer = ExchangeData->ulData;
	NTSTATUS ntStatus = STATUS_SUCCESS;
	switch (ExchangeData->ulFunctionID)
	{
		case GET_MODULE_BASE:
		{
			PGetModuleInfo ModuleInfo = (PGetModuleInfo)pvBuffer;
			if (ModuleInfo)
			{
				ModuleInfo->ulModuleBase = GetModuleBaseAddr(ModuleInfo->ulPid, ModuleInfo->ulModuleName);
				ntStatus = STATUS_SUCCESS;
			}
			break;
		}
	}
	return ntStatus;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) {
	pDriverObject->DriverUnload = DriverUnload;

	DbgPrint("helloworld\r\n");
	//ULONG_PTR ulpBase64 = GetModuleBaseAddr(2928,"kernel32.dll");
	//ULONG_PTR ulpBase32 = GetModuleBaseAddr(3060, "kernel32.dll");
	//DbgPrint("x86 kernel32 Module Base = %llx\r\n", ulpBase32);
	//DbgPrint("x64 kernel32 Module Base = %llx\r\n", ulpBase64);
	RegisterCommunication(DisPatch);
	return STATUS_SUCCESS;
	
}
