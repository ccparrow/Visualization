// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

class VISUALIZATION_API AsyncJsonArray : public FNonAbandonableTask
{
	FString Descriptor;
	FString Key;
	TArray<FString>& Value;
	bool& Success;
	friend class FAsyncTask<AsyncJsonArray>;
public:
	// 构造函数
	AsyncJsonArray(FString Descriptor, FString Key, TArray<FString>& Value, bool& Success)
		: Descriptor(Descriptor), Key(Key), Value(Value),  Success(Success)
	{
		
	}
	// 析构函数
	~AsyncJsonArray() 
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
			//1.查找StringArrayValue
			if (JsonObject->TryGetStringArrayField(Key, Value))
			{
				Success = true;
				return;
			}

			//2.查找JsonStringArrayValue
			const TArray< TSharedPtr<FJsonValue> >* JsonValueArray;
			if (JsonObject->TryGetArrayField(Key, JsonValueArray))
			{
				for (int i = 0; i < JsonValueArray->Num(); i++)
				{
					FString ValueRef;
					//3.判断如果为数组的数组，则合并为字符串，逗号隔开
					if ((*JsonValueArray)[i]->AsArray().Num() > 0 )
					{
						for (int a = 0; a < (*JsonValueArray)[i]->AsArray().Num(); a++)
						{
							if (a == 0)
							{
								ValueRef = (*JsonValueArray)[i]->AsArray()[a]->AsString();
							}
							else
							{
								ValueRef += "," + (*JsonValueArray)[i]->AsArray()[a]->AsString();
							}
						}
						UE_LOG(LogTemp, Warning, TEXT("ArrayArray"));
					}
					else
					{
						TSharedPtr<FJsonObject> JsonRef = (*JsonValueArray)[i]->AsObject();
						TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ValueRef);
						FJsonSerializer::Serialize(JsonRef.ToSharedRef(), Writer);
						UE_LOG(LogTemp, Warning, TEXT("JsonArray"));
					}
					Value.Add(ValueRef);
				}
				Success = true;
			}
		}
	}

	// 固定写法，本类将作为函数参数
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(SimpleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};
