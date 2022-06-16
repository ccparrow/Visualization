// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "mac.generated.h"



UCLASS()
class VISUALIZATION_API Amac : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Amac();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

	UFUNCTION(BlueprintPure, Category = mac, meta = (Keywords = mac))
        static void getMac(FString mac, FString data, FString& retData);

};
