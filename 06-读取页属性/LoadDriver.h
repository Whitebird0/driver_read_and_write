#pragma once
#include<string>
#include<iostream>
#include<Windows.h>
#include<time.h>
#include"resource.h"
//地址随机生成

LPCSTR RandomString();

 //资源释放
void ReleaseDriver(LPCSTR lpFilePath);
 
//安装
void InstallDriver(LPCSTR lpService, LPCSTR lpDisplayName, LPCSTR lpDriverPath);
//启动
void StartDriver();
//停止
void StopDriver();
//卸载

void UnInstallDriver();