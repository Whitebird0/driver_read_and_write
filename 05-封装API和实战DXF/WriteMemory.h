#pragma once
#include<ntifs.h>
#include<intrin.h>
NTSTATUS MmCopyVirtualMemory(
	IN PEPROCESS FromProcess, 
	IN CONST VOID* FromAddress,
	IN PEPROCESS ToProcess, 
	OUT PVOID ToAddress,
	IN SIZE_T BufferSize,
	IN KPROCESSOR_MODE PreviousMode, 
	OUT PSIZE_T NumberOfBytesCopied);


NTSTATUS NTAPI NtProtectVirtualMemory(

	IN HANDLE               ProcessHandle,
	IN OUT PVOID* BaseAddress,
	IN OUT PULONG           NumberOfBytesToProtect,
	IN ULONG                NewAccessProtection,
	OUT PULONG              OldAccessProtection);

NTSTATUS WriteMemory(HANDLE hProcessID, PVOID pBaseAddress, PVOID pBuffer, SIZE_T szBufferSize);
