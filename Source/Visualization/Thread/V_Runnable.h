// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

class VISUALIZATION_API V_Runnable : public FRunnable
{
public:
	V_Runnable();
	~V_Runnable();

	//通过UV_CPPLibrary::CallFuntionByName实现复写
	uint32 Run();

	AActor* ActorRef;
	FString FunctionNameRef;

	//补充构造函数用于赋值
	V_Runnable(AActor* Actor, FString FunctionName);

};
