#include"FarCall.h"

ULONG PsSuspendThreadCode[8] = { 0x24548948 ,0xc894810 , 0x56530824 ,0x41544157 ,0xec834855 ,0xea8b4c40 ,0x33f18b48 ,0x247c89ff };
ULONG PsResumeThreadCode[8] = { 0x8348f3ff ,0x8b4820ec ,0x7f1ae8da ,0x8548ffc6,0x890274db ,0x48c03303 ,0x5b20c483 ,0x909090c3 };
 
PDRIVER_OBJECT g_DriverObject = NULL;
extern POBJECT_TYPE* IoDriverObjectType;
NTSTATUS
ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectName,
	__in ULONG Attributes,
	__in_opt PACCESS_STATE AccessState,
	__in_opt ACCESS_MASK DesiredAccess,
	__in POBJECT_TYPE ObjectType,
	__in KPROCESSOR_MODE AccessMode,
	__inout_opt PVOID ParseContext,
	__out PVOID* Object
);


NTSTATUS InitGetDriverObject() {
	UNICODE_STRING usDriverName;
	RtlInitUnicodeString(&usDriverName, L"\\Driver\\Ndis");
	NTSTATUS status = ObReferenceObjectByName(&usDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, *IoDriverObjectType, KernelMode, NULL, &g_DriverObject);
	return status;
}

NTSTATUS NTAPI NtGetNextThread(
	HANDLE ProcessHandle,
	HANDLE ThreadHandle,
	ACCESS_MASK DesireAccess,
	ULONG HandleAttributes,
	ULONG Flags,
	PHANDLE NewThreadHandle
) {

	typedef NTSTATUS(NTAPI* ZwGetNextThreadProc)(
		HANDLE ProcessHandle,
		HANDLE ThreadHandle,
		ACCESS_MASK DesireAccess,
		ULONG HandleAttributes,
		ULONG Flags,
		PHANDLE NewThreadHandle
		);

	ZwGetNextThreadProc ZwGetNextThreadProcFunc = NULL;
	if (!ZwGetNextThreadProcFunc)
	{
		UNICODE_STRING usFuncName = { 0 };
		RtlInitUnicodeString(&usFuncName, "ZwGetNextThread");
		//Win10可以直接u获取
		ZwGetNextThreadProcFunc = (ZwGetNextThreadProc)MmGetSystemRoutineAddress(&usFuncName);
	}
	//没找到
	if (!ZwGetNextThreadProcFunc) {
		UNICODE_STRING usFuncName1 = { 0 };
		RtlInitUnicodeString(&usFuncName1, "ZwGetNotificationResourceManager");
		//在ZwGetNextThread下面
		PUCHAR punTempAddr = (PUCHAR)MmGetSystemRoutineAddress(&usFuncName1);
		punTempAddr -= 50;
		//特征码  48 8B C4
		for (size_t i = 0; i < 0x30;i++)
		{
			if (punTempAddr[i] == 0x48 && punTempAddr[i + 1] == 0x8B && punTempAddr[i + 2] == 0xC4) {
				ZwGetNextThreadProcFunc = punTempAddr + i;
			}
		}
	}
	if (ZwGetNextThreadProcFunc)
	{
		return ZwGetNextThreadProcFunc(ProcessHandle, ThreadHandle, DesireAccess, HandleAttributes, Flags, NewThreadHandle);
	}

};

NTSTATUS PsSuspendThread(IN PETHREAD Thread, OUT PULONG PreviousSuspendCount OPTIONAL) {
	typedef NTSTATUS(NTAPI* PsSuspendThreadProc)(PETHREAD Thread, PULONG PreviousSuspendCount OPTIONAL);
	static PsSuspendThreadProc PsSuspendThreadFunc = NULL;
	if (!PsSuspendThreadFunc)
	{
		ULONG64 ulRetAddr;
		//特征获取
		GetAddr(PsSuspendThreadCode, &ulRetAddr);
		PsSuspendThreadFunc = (PsSuspendThreadProc)ulRetAddr;

	}
	if (PsSuspendThreadFunc)
	{
		return PsSuspendThreadFunc(Thread, PreviousSuspendCount);
	}
	return STATUS_NOT_IMPLEMENTED;

}

NTSTATUS PsResumeThread(IN PETHREAD Thread, OUT PULONG PreviousSuspendCount OPTIONAL) {
	typedef NTSTATUS(NTAPI* PsResumeThreadProc)(PETHREAD Thread, PULONG PreviousSuspendCount OPTIONAL);
	static PsResumeThreadProc PsResumeThreadFunc = NULL;
	if (!PsResumeThreadFunc)
	{
		ULONG64 ulRetAddr;
		//特征获取
		GetAddr(PsResumeThreadCode, &ulRetAddr);
		PsResumeThreadFunc = (PsResumeThreadProc)ulRetAddr;
	}
	if (PsResumeThreadFunc)
	{
		return PsResumeThreadFunc(Thread, PreviousSuspendCount);
	}
	return STATUS_NOT_IMPLEMENTED;

}
NTSTATUS GetNtoskernBaseAddr(PDRIVER_OBJECT pDriverObject,ULONG64*retNtosAddr) {
	PLIST_ENTRY HeaderNode = NULL;
	PLIST_ENTRY NextNode = NULL;
	UNICODE_STRING usKernelFileName;
	PUNICODE_STRING pusTempKernelFileName;
	RtlInitUnicodeString(&usKernelFileName, L"ntoskrnl.exe");
	HeaderNode = (PLIST_ENTRY)(pDriverObject->DriverSection);
	NextNode = HeaderNode->Flink;
	while (HeaderNode!= NextNode)
	{
		pusTempKernelFileName = (PUNICODE_STRING)((ULONG64)NextNode + 0X58);
		if (RtlCompareUnicodeString(pusTempKernelFileName,&usKernelFileName,TRUE)==0)
		{
			*retNtosAddr = (ULONG64)NextNode;
			return STATUS_SUCCESS;
		}
		NextNode = NextNode->Flink;
	}
	return STATUS_UNSUCCESSFUL; 
}
NTSTATUS GetFunctionAddress(ULONG64 ulStartAddress, ULONG64 ulEndAddress, ULONG64* reFunAddr, ULONG* ulCode) {
	for (size_t i = ulStartAddress; i < ulEndAddress; i++)
	{
		if (((*(ULONG*)i) == ulCode[0]) &&
			*((ULONG*)i + 4) == ulCode[1] &&
			*((ULONG*)i + 8) == ulCode[2] &&
			*((ULONG*)i + 12) == ulCode[3] &&
			(*((ULONG*)(i + 16)) == ulCode[4]) &&
			(*((ULONG*)(i + 20)) == ulCode[5]) &&
			(*((ULONG*)(i + 24)) == ulCode[6]) &&
			(*((ULONG*)(i + 28)) == ulCode[7])
			) {
			*reFunAddr = (PVOID)i;
			return STATUS_SUCCESS;
		}
		
	}
}
NTSTATUS GetAddr(ULONG* ulCode, ULONG64* reFunAddr) {
	ULONG64 ulNtosAddr;
	ULONG64 usStartAddr;
	ULONG64 ulEndAddr;
	ULONG64 size;
	ULONG64 ulFuncAddr;
	GetNtoskernBaseAddr(g_DriverObject,&ulNtosAddr);
	usStartAddr = *(ULONG64*)(ulNtosAddr + 0X38);
	size = *(ULONG64*)(ulNtosAddr + 0x40);
	ulEndAddr = usStartAddr + size;
	GetFunctionAddress(usStartAddr, ulEndAddr, &ulFuncAddr, ulCode);
	*reFunAddr = ulFuncAddr;
	return STATUS_SUCCESS;
}
PETHREAD NtGetProcessMainThread(PEPROCESS pEprocess) {
	PETHREAD pThread = NULL;
	KAPC_STATE kApc;
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	KeStackAttachProcess(pEprocess, &kApc);
	HANDLE hThread = NULL;
	ntStatus = NtGetNextThread(NtCurrentProcess(), NULL, THREAD_ALL_ACCESS, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, 0, &hThread);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ObReferenceObjectByHandle(hThread, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &pThread, NULL);
		NtClose(pThread);
		if (!NT_SUCCESS(ntStatus)) {
			pThread = NULL;
		}
	}
	KeUnstackDetachProcess(&kApc);
	return pThread;
}
