#pragma once
#include<ntifs.h>

NTSTATUS GetNtoskernBaseAddr(PDRIVER_OBJECT pDriverObject, ULONG64* retNtosAddr);

NTSTATUS GetFunctionAddress(ULONG64 ulStartAddress, ULONG64 ulEndAddress, ULONG64* reFunAddr, ULONG* ulCode);

NTSTATUS GetAddr(ULONG* ulCode, ULONG64* reFunAddr);
