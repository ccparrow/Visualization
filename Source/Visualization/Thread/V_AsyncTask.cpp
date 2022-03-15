// Fill out your copyright notice in the Description page of Project Settings.


#include "V_AsyncTask.h"
#include "../V_Library.h"

V_AsyncTask::V_AsyncTask()
{
}

V_AsyncTask::~V_AsyncTask()
{
}

void V_AsyncTask::DoWork()
{
	UV_Library::CallFuntionByName(ActorRef, FunctionNameRef);
}

V_AsyncTask::V_AsyncTask(AActor* Actor, FString FunctionName)
{
	this->ActorRef = Actor;
	this->FunctionNameRef = FunctionName;
}
