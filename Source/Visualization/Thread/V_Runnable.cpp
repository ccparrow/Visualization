// Fill out your copyright notice in the Description page of Project Settings.


#include "V_Runnable.h"
#include "../V_Library.h"

V_Runnable::V_Runnable()
{
}

V_Runnable::~V_Runnable()
{
}

uint32 V_Runnable::Run()
{
	UV_Library::CallFuntionByName(ActorRef, FunctionNameRef);
	return 0;
}

V_Runnable::V_Runnable(AActor* Actor, FString FunctionName)
{
	this->ActorRef = Actor;
	this->FunctionNameRef = FunctionName;
}
