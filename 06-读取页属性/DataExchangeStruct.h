#pragma once
#include<ntifs.h>

typedef struct	_GetModuleInfo {
	ULONG64 ulPid;
	ULONG64 ulModuleName;
	ULONG64 ulModuleBase;
}GetModuleInfo,*PGetModuleInfo;

typedef struct _RMEMORY_BASIC_INFORMATION {
	ULONG64  BaseAddress;
	ULONG64  AllocationBase;
	ULONG64  AllocationProtect;
	ULONG64 RegionSize;
	ULONG64  State;
	ULONG64  Protect;
	ULONG64  Type;
} RMEMORY_BASIC_INFORMATION, * PRMEMORY_BASIC_INFORMATION;

typedef struct _ReadWriteInfo {
	ULONG64 ulPid;
	ULONG64 ulBaseAddress;
	ULONG64 ulBuffer;
	ULONG64 ulSize;

}ReadWriteInfo, *PReadWriteInfo;
typedef struct _QueryMemoryInfo {
	ULONG64 ulPid;
	ULONG64 ulBaseAddress;
	RMEMORY_BASIC_INFORMATION pbiMemoryInfo;
}QueryMemoryInfo, * PQueryMemoryInfo;