// Fill out your copyright notice in the Description page of Project Settings.


#include "V_Pawn.h"

#include "../Thread/V_Runnable.h"

// Sets default values
AV_Pawn::AV_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AV_Pawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AV_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

// Called to bind functionality to input
void AV_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

