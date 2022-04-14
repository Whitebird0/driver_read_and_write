#pragma once
#include<ntifs.h>
typedef struct	_GetModuleInfo {
	ULONG64 ulPid;
	ULONG64 ulModuleName;
	ULONG64 ulModuleBase;
}GetModuleInfo,*PGetModuleInfo;