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
	//�ص�������ȷ����˭���õ�ID
	ULONG64 ulInfoId;
	//ʵ�����ֹ��ܵ�ID
	ULONG64 ulFunctionID;
	//����
	ULONG64 ulData;
	//�ߴ�
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
	//��ȡNtQueryInformationFile������ַ
	NtQueryInformationFileProc NtQueryInformationFile = (NtQueryInformationFileProc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationFile");
	//�����ļ�
	HANDLE hFile = CreateFileA("C:\\test.txt", FILE_ALL_ACCESS, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	//����һ��������������Ŵ���0�������ݣ���С������0XE0
	char buffer[0xE0] = { 0 };
	//����һ���ṹ�屣��һЩ�������Ϣ,RҲҪ��
	GetModuleInfo ModuleInfo = { 0 };
	printf("������ģ����:\n");
	char szModuleName[0x256] = { 0 };
	scanf("%s", szModuleName);

	printf("���������ID:\n");
	scanf("%d", &ModuleInfo.ulPid);
	ModuleInfo.ulModuleName = (ULONG64)szModuleName;
	//buffer�Ľṹ����ǿת
	PDataPake pdData = (PDataPake)buffer;
	//��������R0������Ҫ�õ����ݽ��и�ֵ
	pdData->ulInfoId = 0x12138111;
	pdData->ulFunctionID = 0x1;
	pdData->ulData = (ULONG64)&ModuleInfo;
	pdData->ulSize = sizeof(GetModuleInfo);
	IO_STATUS_BLOCK isb = { 0 };
	//��ʱNtQueryInformationFile��0�������Ѿ���hook�ˣ��ͻ�ʵ�������Լ�����Ĳ���
	NtQueryInformationFile(hFile, &isb, buffer, sizeof(buffer), 0x34);
	CloseHandle(hFile);
	printf("0x%x", ModuleInfo.ulModuleBase);
	
	StopDriver();
	UnInstallDriver();
	system("pause");
	return 0;
}