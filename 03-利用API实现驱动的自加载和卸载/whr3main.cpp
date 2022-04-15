#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<stdlib.h>
#include<iostream>
#include"LoadDriver.h"
#include<stdio.h>
#define GET_MODULE_BASE 0x1

typedef struct	_GetModuleInfo {
	ULONG64 ulPid;
	ULONG64 ulModuleName;
	ULONG64 ulModuleBase;
}GetModuleInfo, * PGetModuleInfo;

typedef struct _DataPake {
	//回调函数中确定是谁调用的ID
	ULONG64 ulInfoId;
	//实现那种功能的ID
	ULONG64 ulFunctionID;
	//数据
	ULONG64 ulData;
	//尺寸
	ULONG64 ulSize;
}DataPake, * PDataPake;

typedef struct _IO_STATUS_BLOCK {
	union {
		ULONG Status;
		PVOID Pointer;
	} ;

	ULONG_PTR Information;
} IO_STATUS_BLOCK, * PIO_STATUS_BLOCK;

typedef ULONG(WINAPI* NtQueryInformationFileProc)(
	HANDLE                 FileHandle,
	PIO_STATUS_BLOCK       IoStatusBlock,
	PVOID                  FileInformation,
	ULONG                  Length,
	ULONG FileInformationClass
	);

int main() {
	char DriverPath[50] = "c:\\";
	char End[5] = ".sys";
	char DriverName[50] = { 0 };
	strcpy((char*)DriverName, RandomString());
	strcat(DriverPath, DriverName);
	strcat(DriverPath, End);
	printf("%s", DriverPath);
	ReleaseDriver(DriverPath);
	InstallDriver(DriverName, DriverName, DriverPath);
	StartDriver();
	//获取NtQueryInformationFile函数地址
	NtQueryInformationFileProc NtQueryInformationFile = (NtQueryInformationFileProc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationFile");
	//创建文件
	HANDLE hFile = CreateFileA("C:\\test.txt", FILE_ALL_ACCESS, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//申请一个缓冲区用来存放传入0环的数据，大小必须是0XE0
	char buffer[0xE0] = { 0 };
	//利用一个结构体保存一些输入的信息,R也要用
	GetModuleInfo ModuleInfo = { 0 };
	printf("请输入模块名:\n");
	char szModuleName[0x256] = { 0 };
	scanf("%s", szModuleName);

	printf("请输入进程ID:\n");
	scanf("%d", &ModuleInfo.ulPid);
	ModuleInfo.ulModuleName = (ULONG64)szModuleName;
	//buffer的结构进行强转
	PDataPake pdData = (PDataPake)buffer;
	//即将进入R0，对需要用的数据进行赋值
	pdData->ulInfoId = 0x12138111;
	pdData->ulFunctionID = 0x1;
	pdData->ulData = (ULONG64)&ModuleInfo;
	pdData->ulSize = sizeof(GetModuleInfo);
	IO_STATUS_BLOCK isb = { 0 };
	//此时NtQueryInformationFile的0环代码已经被hook了，就会实现我们自己定义的操作
	NtQueryInformationFile(hFile, &isb, buffer, sizeof(buffer), 0x34);
	CloseHandle(hFile);
	printf("0x%x", ModuleInfo.ulModuleBase);
	
	StopDriver();
	UnInstallDriver();
	system("pause");
	return 0;
}