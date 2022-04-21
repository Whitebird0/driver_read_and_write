#include"GetModuleBase.h"
#include<ntifs.h>

//32位模块获取
ULONG_PTR x86GetModuleBaseAddr(PEPROCESS pEprocess,PPEB32 pPeb32,PUNICODE_STRING ulModuleName) {
	ULONG_PTR ulpModuleAddr = 0;
	ULONG ulRet = 0;
	MmCopyVirtualMemory(pEprocess, pPeb32, pEprocess, pPeb32, 0x1, UserMode, &ulRet);
	PPEB_LDR_DATA32 pldLdr = ULongToPtr(pPeb32->Ldr);
	PLIST_ENTRY32 pModuleList = (PLIST_ENTRY32)&(pldLdr->InLoadOrderModuleList);
	PLDR_DATA_TABLE_ENTRY32 pLdrData = (PLDR_DATA_TABLE_ENTRY32)ULongToPtr(pModuleList->Flink);
	while (pLdrData != pModuleList)
	{
		PWCH pwchDllName=pLdrData->BaseDllName.Buffer;
		UNICODE_STRING usDllName;	
		RtlInitUnicodeString(&usDllName, pwchDllName);
		
		if (RtlCompareUnicodeString(&usDllName,ulModuleName,TRUE)==0)
		{
			ulpModuleAddr = pLdrData->DllBase;
			break;
		}
		else
		{
			pLdrData = (PLDR_DATA_TABLE_ENTRY32)ULongToPtr(pLdrData->InLoadOrderLinks.Flink);
		}
		
	}
	return ulpModuleAddr;
}
//64位模块获取
ULONG_PTR x64GetModuleBaseAddr(PEPROCESS pEprocess, PPEB pPeb64,PUNICODE_STRING ulModuleName) {
	ULONG_PTR  ulpModuleAddr = 0;
	ULONG ulRet = 0;
	MmCopyVirtualMemory(pEprocess, pPeb64, pEprocess, pPeb64, 0x1, UserMode, &ulRet);
	PPEB_LDR_DATA pldLdr = pPeb64->Ldr;
	PLIST_ENTRY pModuleList = (PLIST_ENTRY)&(pldLdr->InLoadOrderModuleList);
	PLDR_DATA_TABLE_ENTRY pLdrData = (PLDR_DATA_TABLE_ENTRY)(pModuleList->Flink);

	while (pLdrData != pModuleList)
	{
		
		if (RtlCompareUnicodeString(ulModuleName, &pLdrData->BaseDllName, TRUE) == 0)
		{
			ulpModuleAddr = pLdrData->DllBase;
			break;
		}
		else {
			pLdrData = (PLDR_DATA_TABLE_ENTRY)pLdrData->InLoadOrderLinks.Flink;
		}
	}
		return ulpModuleAddr;
	

}
//参数：1进程ID 2模块名称
ULONG_PTR GetModuleBaseAddr(HANDLE hProcessId,char* szModuleName) {

	if (!szModuleName) {//模块名称是否拿到
		return 0;
	}
	//进程结构
	PEPROCESS pEprocess = NULL;
	//APC结构
	KAPC_STATE kApcState = { 0 };
	//模块地址
	ULONG_PTR ulpModuleAddr = 0;
	//状态
	NTSTATUS ntStatus = STATUS_SUCCESS;

	//获取进程结构
	ntStatus = PsLookupProcessByProcessId(hProcessId, &pEprocess);
	if (!NT_SUCCESS(ntStatus))
	{
		return 0;
	}
	STRING strModuleName = { 0 };
	RtlInitAnsiString(&strModuleName, szModuleName);
	UNICODE_STRING ulModuleName = { 0 };
	RtlAnsiStringToUnicodeString(&ulModuleName, &strModuleName, TRUE);
	//挂靠进程
	KeStackAttachProcess(pEprocess, &kApcState);
	//获取PEB 32位，64位
	PPEB pPeb64 = PsGetProcessPeb(pEprocess);
	PPEB32 pPeb32 = PsGetProcessWow64Process(pEprocess);
	if (pPeb32)
	{
		//32PEB
		ulpModuleAddr=x86GetModuleBaseAddr(pEprocess, pPeb32,&ulModuleName);

	}
	else {
		//64PEB
		ulpModuleAddr=x64GetModuleBaseAddr(pEprocess, pPeb64, &ulModuleName);

	}
	//解除挂靠
	KeUnstackDetachProcess(&kApcState);
	return ulpModuleAddr;


}

NTSTATUS MyQueryMemoryInfo(HANDLE hProcessId, ULONG64 ulBaseAddress, PRMEMORY_BASIC_INFORMATION pbiMemoryInfo)
{
	if (pbiMemoryInfo == NULL)
	{
		return STATUS_UNSUCCESSFUL;
	}


	PEPROCESS pEprocess = NULL;
	KAPC_STATE kApc = { 0 };
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	ntStatus = PsLookupProcessByProcessId(hProcessId, &pEprocess);
	if (!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}
	PMEMORY_BASIC_INFORMATION pbiMemoryInformation = (PMEMORY_BASIC_INFORMATION)ExAllocatePool(NonPagedPool, sizeof(MEMORY_BASIC_INFORMATION));
	RtlZeroMemory(pbiMemoryInformation, sizeof(MEMORY_BASIC_INFORMATION));
	KeStackAttachProcess(pEprocess, &kApc);
	SIZE_T szRetLength = 0;
	ntStatus = ZwQueryVirtualMemory(NtCurrentProcess(), ulBaseAddress, MemoryBasicInformation, pbiMemoryInformation, sizeof(MEMORY_BASIC_INFORMATION), &szRetLength);
	KeUnstackDetachProcess(&kApc);
	if (NT_SUCCESS(ntStatus))
	{
		pbiMemoryInfo->AllocationBase = pbiMemoryInformation->AllocationBase;
		pbiMemoryInfo->AllocationProtect = pbiMemoryInformation->AllocationProtect;
		pbiMemoryInfo->BaseAddress = pbiMemoryInformation->BaseAddress;
		pbiMemoryInfo->Protect = pbiMemoryInformation->Protect;
		pbiMemoryInfo->RegionSize = pbiMemoryInformation->RegionSize;
		pbiMemoryInfo->State = pbiMemoryInformation->State;
		pbiMemoryInfo->Type = pbiMemoryInformation->Type;
	}
	ObDereferenceObject(pEprocess);
	ExFreePool(pbiMemoryInformation);
	return ntStatus;


}
