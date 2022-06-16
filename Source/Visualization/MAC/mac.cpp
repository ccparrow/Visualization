// Fill out your copyright notice in the Description page of Project Settings.


#include "mac.h"
#include <sstream>


// Sets default values
Amac::Amac()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void Amac::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void Amac::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString Amac::encryptMAC(FString mac)
{
	unsigned char _cMac[8] = {0x4f, 0xf6, 0x3e, 0x5d, 0x02, 0xff, 0xca, 0x1b}; //密钥的定义
	unsigned char _cData[19] = {0x00, 0x00, 0x12, 0x34, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x20, 0x12, 0x12, 0x07, 0x16, 0x41, 0x00,0x80}; //加密数据的定义,加密数据定长.
	unsigned char _cOutData[4];

	const std::string& ustrPath(TCHAR_TO_UTF8(*mac));
	unsigned char cMac[8];
	memcpy(cMac,ustrPath.data(),ustrPath.size());
	
	MacArithmetic(_cMac, _cData, 19, _cOutData);

	std::string sRetData = "";
	for (int i = 0; i < 4; i++)
		sRetData += _cOutData[i];

	FString ustr = UTF8_TO_TCHAR(sRetData.c_str());
	
	return ustr;
}


