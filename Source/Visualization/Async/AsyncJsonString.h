// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

class VISUALIZATION_API AsyncJsonString : public FNonAbandonableTask
{
	FString Descriptor;
	FString Key;
	FString& Value;
	bool& Success;
	friend class FAsyncTask<AsyncJsonString>;
public:
	// 构造函数
	AsyncJsonString(FString Descriptor, FString Key, FString& Value, bool& Success)
		: Descriptor(Descriptor), Key(Key), Value(Value),  Success(Success)
	{
		
	}
	// 析构函数
	~AsyncJsonString() 
	{
		//UE_LOG(LogTemp, Warning, TEXT("Task[%s] End"), *Value);
	}

	// 具体任务逻辑执行
	void DoWork()
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Descriptor);

		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			//1.查找StringValue
			if (JsonObject->TryGetStringField(Key, Value))
			{
				//UE_LOG(LogTemp, Warning, TEXT("StringValue:"), *Value);
				Success = true;
				return;
			}
			//2.查找JsonStringValue
			const TSharedPtr<FJsonObject>* JsonObjectPtr;
			if (JsonObject->TryGetObjectField(Key, JsonObjectPtr))
			{
				const TSharedPtr<FJsonObject> JsonObjectRef = JsonObject->GetObjectField(Key);
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Value);
				Success = FJsonSerializer::Serialize(JsonObjectRef.ToSharedRef(), Writer);
				//UE_LOG(LogTemp, Warning, TEXT("JsonStringValue:"), *Value);
			}
		}
	}

	// 固定写法，本类将作为函数参数
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(SimpleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};
