// Fill out your copyright notice in the Description page of Project Settings.


#include "mac.h"
#include "mac.c"


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

void Amac::getMac(FString mac, FString data, FString& retData)
{
	// unsigned char* _mac();
	// std::string smac = TCHAR_TO_UTF8(*mac);
	// smac.append(reinterpret_cast<const char*>(_mac()));
	
	// unsigned char* _mac= TCHAR_TO_ANSI(*mac);
	// unsigned char* _data= TCHAR_TO_ANSI(*data);
	// unsigned char* _retData= TCHAR_TO_ANSI(*retData);
	//
	// MacArithmetic(_mac,_data,19,_retData);
	
	MacArithmetic(_mac,_data,19,_retData);
	
}

