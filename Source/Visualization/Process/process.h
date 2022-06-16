// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "process.generated.h"

UCLASS()
class VISUALIZATION_API Aprocess : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Aprocess();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UFUNCTION(BlueprintCallable, Category = "visualization|proc", meta = (Keywords = proc))
		static bool createProcess(FString url, FString param);
	UFUNCTION(BlueprintCallable, Category = "visualization|proc", meta = (Keywords = proc))
		static bool isProcessRunning();
	UFUNCTION(BlueprintCallable, Category = "visualization|proc", meta = (Keywords = proc))
		static void closeProcess(FString name);
	UFUNCTION(BlueprintCallable, Category = "visualization|proc", meta = (Keywords = proc))
		static int getProcessId();

	
};
