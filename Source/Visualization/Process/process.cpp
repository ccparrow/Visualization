// Fill out your copyright notice in the Description page of Project Settings.


#include "process.h"
#include <string>

// Sets default values
Aprocess::Aprocess()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void Aprocess::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void Aprocess::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


static FProcHandle procHandle;

bool Aprocess::createProcess(FString url, FString param)
{
	if (!FPlatformProcess::CanLaunchURL(*url))
	{
		UE_LOG(LogTemp, Warning, TEXT("OpenExe false!!!"));
		return false;
	}
	procHandle = FPlatformProcess::CreateProc(*url, *param, true, false, false, nullptr, 0, nullptr, nullptr);
	UE_LOG(LogTemp, Warning, TEXT("OpenExe URL:%s open success!!!"), *url);
	return FPlatformProcess::IsProcRunning(procHandle);
}

bool Aprocess::isProcessRunning()
{
	return FPlatformProcess::IsProcRunning(procHandle);
}
 
void  Aprocess::closeProcess(FString name)
{
	if (!procHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("procHandle is not valid"));
	}
	FPlatformProcess::CloseProc(procHandle);
	
	name = "TASKKILL /F /IM " + name;
	UE_LOG(LogTemp, Warning, TEXT("name: %s"), *name);
	const char* charString = TCHAR_TO_ANSI(*name);
	system(charString);
}

int Aprocess::getProcessId()
{
	return FPlatformProcess::GetCurrentProcessId();
}

