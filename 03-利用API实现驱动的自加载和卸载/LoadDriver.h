#pragma once
#include<string>
#include<iostream>
#include<Windows.h>
#include<time.h>
#include"resource.h"
//��ַ�������

LPCSTR RandomString();

 //��Դ�ͷ�
void ReleaseDriver(LPCSTR lpFilePath);
 
//��װ
void InstallDriver(LPCSTR lpService, LPCSTR lpDisplayName, LPCSTR lpDriverPath);
//����
void StartDriver();
//ֹͣ
void StopDriver();
//ж��

void UnInstallDriver();