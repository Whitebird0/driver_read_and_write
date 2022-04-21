#include <ntifs.h>
#include"DataExchangeStruct.h"
EXTERN_C PVOID NTAPI PsGetProcessPeb(PEPROCESS pEProcess);
EXTERN_C PVOID NTAPI PsGetProcessWow64Process(PEPROCESS  pEprocess);
NTSTATUS MmCopyVirtualMemory(IN PEPROCESS FromProcess, IN CONST VOID* FromAddress, IN PEPROCESS ToProcess, OUT PVOID ToAddress, IN SIZE_T BufferSize, IN KPROCESSOR_MODE PreviousMode, OUT PSIZE_T NumberOfBytesCopied);



typedef struct _PEB_LDR_DATA32
{
	ULONG Length;
	UCHAR Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG EntryInProgress;
	UCHAR ShutdownInProgress;
	ULONG ShutdownThreadId;
}PEB_LDR_DATA32, * PPEB_LDR_DATA32;




typedef struct _PEB32
{
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	union
	{
		UCHAR BitField;
		struct
		{
			UCHAR ImageUsesLargePages : 1;
			UCHAR IsProtectedProcess : 1;
			UCHAR IsLegacyProcess : 1;
			UCHAR IsImageDynamicallyRelocated : 1;
			UCHAR SkipPatchingUser32Forwarders : 1;
			UCHAR SpareBits : 3;
		};
	};
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG Ldr;

}PEB32, * PPEB32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderLinks;
	LIST_ENTRY32 InInitializationOrderLinks;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY32 HashLinks;
		struct
		{
			ULONG SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		ULONG LoadedImports;
	};
	ULONG EntryPointActivationContext;
	ULONG PatchInformation;
	LIST_ENTRY32 ForwarderLinks;
	LIST_ENTRY32 ServiceTagLinks;
	LIST_ENTRY32 StaticLinks;
	ULONG ContextInformation;
	ULONG OriginalBase;
	LARGE_INTEGER LoadTime;
}LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

#pragma pack(0)

typedef struct _PEB_LDR_DATA
{

	ULONG Length;
	UCHAR Initialized;
	VOID* SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	VOID* EntryInProgress;
	UCHAR ShutdownInProgress;
	VOID* ShutdownThreadId;
}PEB_LDR_DATA, * PPEB_LDR_DATA;

typedef struct _PEB
{
	ULONG64 x;
	VOID* Mutant;
	VOID* ImageBaseAddress;
	PEB_LDR_DATA* Ldr;

}PEB, * PPEB;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	VOID* DllBase;
	VOID* EntryPoint;
	ULONG64 SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		struct
		{
			VOID* SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		VOID* LoadedImports;
	};
	struct _ACTIVATION_CONTEXT* EntryPointActivationContext;
	VOID* PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
	VOID* ContextInformation;
	ULONGLONG OriginalBase;
	LARGE_INTEGER LoadTime;
}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

ULONG_PTR GetModuleBaseAddr(HANDLE hProcessId, char* szModuleName);



NTSTATUS MyQueryMemoryInfo(HANDLE hProcessId, ULONG64 ulBaseAddress, PRMEMORY_BASIC_INFORMATION pbiMemoryInfo);