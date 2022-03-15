// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AliyunCppPlugin.h"
#include "AliyunOssTask.h"


#define LOCTEXT_NAMESPACE ""



void FAliyunOSSModule::StartupModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
// we call this function before unloading the module.
	FOssClient::InitializeSdk();
}

void FAliyunOSSModule::ShutdownModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FOssClient::ShutdownSdk();
	
}


#undef LOCTEXT_NAMESPACE
	


IMPLEMENT_MODULE(FAliyunOSSModule, AliyunOss)