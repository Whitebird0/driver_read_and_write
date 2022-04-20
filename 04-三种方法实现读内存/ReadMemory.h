#pragma once
#include<ntifs.h>
#include<intrin.h>
//ReadProcessMemory
NTSTATUS
MmCopyVirtualMemory(
	IN  PEPROCESS FromProcess,
	IN  CONST VOID* FromAddress,
	IN  PEPROCESS ToProcess,
	OUT PVOID ToAddress,
	IN  SIZE_T BufferSize,
	IN  KPROCESSOR_MODE PreviousMode,
	OUT PSIZE_T NumberOfBytesCopied
);
NTSTATUS ReadDirect(HANDLE hProcessID, PVOID pBaseAddress, PVOID pBuffer, SIZE_T szBufferSize);

NTSTATUS ReadVirtualMemory(HANDLE hProcessID, PVOID pBaseAddress, PVOID pBuffer, SIZE_T szBufferSize);
