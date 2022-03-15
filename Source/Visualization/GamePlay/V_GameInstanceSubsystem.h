// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "V_GameInstanceSubsystem.generated.h"

/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class VISUALIZATION_API UV_GameInstanceSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BluePrintImplementableEvent)//蓝图实现使其Tick
	void TickFunction();
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return !IsTemplate(); }//不是CDO才Tick
	virtual TStatId GetStatId() const override{RETURN_QUICK_DECLARE_CYCLE_STAT(UMyScoreSubsystem, STATGROUP_Tickables);}

};
