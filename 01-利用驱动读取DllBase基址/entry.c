#include<ntifs.h>
#include"GetModuleBase.h"
VOID DriverUnload(PDRIVER_OBJECT pDriverObject) {



}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) {
	pDriverObject->DriverUnload = DriverUnload;

	DbgPrint("helloworld\r\n");
	ULONG_PTR ulpBase64 = GetModuleBaseAddr(2928,"kernel32.dll");
	ULONG_PTR ulpBase32 = GetModuleBaseAddr(3060, "kernel32.dll");
	DbgPrint("x86 kernel32 Module Base = %llx\r\n", ulpBase32);
	DbgPrint("x64 kernel32 Module Base = %llx\r\n", ulpBase64);
	return STATUS_SUCCESS;
	
}