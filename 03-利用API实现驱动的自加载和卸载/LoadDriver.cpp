#include"LoadDriver.h"
SC_HANDLE hSCM = NULL;
SC_HANDLE hSer = NULL;
#define Random(x)(rand()%x)

LPCSTR RandomString()
{
	srand((int)time(NULL));
	char Str[] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
	char Temp[9] = { 0 };
	for (int i = 0; i < 7; i++) {
		int nIndex = Random(26);
		Temp[i] = Str[nIndex];
	}
	return Temp;
}


void ReleaseDriver(LPCSTR lpFilePath)
{
	//x64Grk
	//IDR_X64GRK1
	HRSRC hRsrc=FindResourceA(NULL, MAKEINTRESOURCEA(IDR_X64GRK1), "x64Grk");
	//资源大小
	DWORD dwSize = SizeofResource(NULL, hRsrc);
	//加载资源
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc);
	//获取资源指针
	LPVOID lpResAddr = LockResource(hGlobal);
	HANDLE hFile = CreateFileA((LPCSTR)lpFilePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwWriteLen = 0;
	WriteFile(hFile, lpResAddr, dwSize, &dwWriteLen, NULL);
	CloseHandle(hFile);
	CloseHandle(hRsrc);
}

void InstallDriver(LPCSTR lpServiceName, LPCSTR lpDisplayName, LPCSTR lpDriverPath)
{
	hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	hSer = CreateServiceA(hSCM, lpServiceName, lpDisplayName, SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_IGNORE, lpDriverPath, NULL, NULL, NULL, NULL, NULL);
	if (hSer==NULL)
	{
		std::cout << "驱动安装失败!" << std::endl;
	}
	else {
		std::cout << "驱动安装成功" << std::endl;
	}


}

void StartDriver()
{
	BOOL bSucc = StartService(hSer, NULL, NULL);
	if (bSucc==TRUE)
	{
		std::cout << "驱动启动成功" << std::endl;
	}
	else {
		std::cout << "驱动启动失败" << std::endl;
	}
}

void StopDriver()
{
	SERVICE_STATUS SerStatus = { 0 };
	BOOL bSucc = ControlService(hSer, SERVICE_CONTROL_STOP, &SerStatus);
	if (bSucc == TRUE)
	{
		std::cout << "驱动停止成功" << std::endl;
	}
	else {
		std::cout << "驱动停止失败" << std::endl;
	}


}

void UnInstallDriver()
{
	BOOL bSucc = DeleteService(hSer);
	if (bSucc == TRUE)
	{
		std::cout << "驱动卸载成功" << std::endl;
	}
	else {
		std::cout << "驱动卸载失败" << std::endl;
	}
	CloseServiceHandle(hSCM);
	CloseServiceHandle(hSer);
}
