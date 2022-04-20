#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<stdlib.h>
#include<iostream>
#include"LoadDriver.h"
#include<stdio.h>

#define GET_MODULE_BASE 0x1
#define GWH_READ_MEMORY 0x2
#define	GWH_WRITE_MEMORY 0x3

typedef struct	_GetModuleInfo {
	ULONG64 ulPid;
	ULONG64 ulModuleName;
	ULONG64 ulModuleBase;
}GetModuleInfo, *PGetModuleInfo;

typedef struct _ReadWriteInfo {
	ULONG64 ulPid;
	ULONG64 ulBaseAddress;
	ULONG64 ulBuffer;
	ULONG64 ulSize;	

}ReadWriteInfo,*PReadWriteInfo;

typedef struct _DataPake {
	//�ص�������ȷ����˭���õ�ID
	ULONG64 ulInfoId;
	//ʵ�����ֹ��ܵ�ID
	ULONG64 ulFunctionID;
	//����
	ULONG64 ulData;
	//�ߴ�
	ULONG64 ulSize;
	//״̬
	
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
NtQueryInformationFileProc NtQueryInformationFile = NULL;
HANDLE hFile = NULL;


BOOLEAN DriverExchange(ULONG64 ulFunctionID, PVOID pBuffer, SIZE_T szSize) {
	char buffer[0xE0] = { 0 };
	PDataPake pDataPake = (PDataPake)buffer;
	pDataPake->ulInfoId = 0x12138111;
	pDataPake->ulFunctionID = ulFunctionID;
	pDataPake->ulData = (ULONG64)pBuffer;
	pDataPake->ulSize = szSize;
	IO_STATUS_BLOCK isb = { 0 };
	//��ʱNtQueryInformationFile��0�������Ѿ���hook�ˣ��ͻ�ʵ�������Լ�����Ĳ���
	NtQueryInformationFile(hFile, &isb, buffer, sizeof(buffer), 0x34);
	return 0;
}


VOID LoadDriver() {//��װ��������
	char DriverPath[50] = "c:\\";
	char End[5] = ".sys";
	char DriverName[50] = { 0 };
	strcpy((char*)DriverName, RandomString());
	strcat(DriverPath, DriverName);
	strcat(DriverPath, End);
	ReleaseDriver(DriverPath);
	InstallDriver(DriverName, DriverName, DriverPath);
	StartDriver();
	DeleteFile((LPWSTR)DriverPath);
}

VOID UnLoadDriver() {//ֹͣж������
	StopDriver();
	UnInstallDriver();
}

VOID InitDataExchange() {
	char strFilePath [50]= "c:\\";
	char End[5] = ".sys";
	char Filename[50] = { 0 };
	strcpy((char*)Filename, RandomString());
	strcat(strFilePath, Filename);
	strcat(strFilePath, End);
	NtQueryInformationFile = (NtQueryInformationFileProc)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationFile");
	hFile = CreateFileA("C:\\test.txt", FILE_ALL_ACCESS, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

BOOLEAN GwhReadMemory(DWORD dwPID,ULONG64 ulBase,PVOID pBuffer,ULONG ulSize) {
	ReadWriteInfo info = { 0 };
	info.ulPid = dwPID;
	info.ulBaseAddress = ulBase;
	info.ulBuffer = (ULONG64)pBuffer;
	info.ulSize = ulSize;
	BOOLEAN bRet = DriverExchange(GWH_READ_MEMORY, &info, sizeof(ReadWriteInfo));
	return bRet;
}

BOOLEAN GwhWriteMemory(DWORD dwPID, ULONG64 ulBase, PVOID pBuffer, ULONG ulSize) {
	ReadWriteInfo info = { 0 };
	info.ulPid = dwPID;
	info.ulBaseAddress = ulBase;
	info.ulBuffer = (ULONG64)pBuffer;
	info.ulSize = ulSize;
	BOOLEAN bRet = DriverExchange(GWH_WRITE_MEMORY, &info, sizeof(ReadWriteInfo));
	return bRet;

}

BOOLEAN GetModuleBase(DWORD dwPID, const char* szModuleName,ULONG64 *ulRetAddress) {
	GetModuleInfo ModuleInfo = { 0 };
	ModuleInfo.ulModuleName = (ULONG64) szModuleName;
	ModuleInfo.ulPid = dwPID;
	BOOLEAN bRet = DriverExchange(GET_MODULE_BASE, &ModuleInfo, sizeof(GetModuleInfo));
	*ulRetAddress = ModuleInfo.ulModuleBase;
	return bRet;
}
int main() {
	LoadDriver();
	InitDataExchange();
	system("pause");
	HWND hWnd = NULL;
	do {
		hWnd = FindWindowA("���³�����ʿ", "���³�����ʿ");
	} while ((!hWnd));
	DWORD nPid = 0;
	GetWindowThreadProcessId(hWnd, &nPid);

	printf("PID:%d\n", nPid);
	printf("������PID:\n");
	int pid;
	scanf("%d", &pid);
	char dllname[0x256] = { 0 };
	//DNF.exe  140000000
	printf("������ģ������:\n");
	scanf("%s", dllname);
	ULONG64 ulModuleBase = 0;
	GetModuleBase(pid, dllname, &ulModuleBase);
	printf("ModuleName : %s ModuleBase = %llX\n", dllname, ulModuleBase);
	ULONG ulReadBase = 0;
	//ULONG ulWriteBase=0;
	ULONG ulTemp;
	//printf("������Ҫд��ĵ�ַ:\n");
	//char buffer[6] = { '1','2','3','4','5','6'};
	//scanf("%x", &ulWriteBase);
	//GwhWriteMemory(pid, ulWriteBase, buffer, sizeof(buffer));
	//0x14A428630
	printf("������Ҫ��ȡ�ĵ�ַ:\n");
	scanf("%x", &ulReadBase);
	GwhReadMemory(pid, ulReadBase, &ulTemp, sizeof(ulTemp));
	printf("Player Obejct��0x%llX\n", ulTemp);
	system("pause");
	UnLoadDriver();
	
	system("pause");
	return 0;
}