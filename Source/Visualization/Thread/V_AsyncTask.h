// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

class VISUALIZATION_API V_AsyncTask : public FNonAbandonableTask
{
public:
	V_AsyncTask();
	~V_AsyncTask();

	//该类必须生成标注生成线程的信息
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(PrimeCalculationAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

	//通过UV_CPPLibrary::CallFuntionByName实现复写
	void DoWork();

	AActor* ActorRef;
	FString FunctionNameRef;

	//补充构造函数用于赋值
	V_AsyncTask(AActor* Actor, FString FunctionName);

};
