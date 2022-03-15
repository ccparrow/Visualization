
// Fill out your copyright notice in the Description page of Project Settings.
 
#pragma once
 
#include "CoreMinimal.h"
#include "Sockets.h"
#include "HAL/Runnable.h"
#include "SocketRSThread.generated.h"
 

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReceiveSocketDelegate, FString, Data);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDisconnectDelegate, USocketRSThread*, Thread);


UCLASS()
class VISUALIZATION_API USocketRSThread : public UObject, public FRunnable
{
	GENERATED_BODY()
public:
 
	// 线程接口
	virtual bool Init() override { return true; }
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override {}
 
	void Start(FSocket* Socket, uint32 InSendDataSize, uint32 InRecDataSize);
	void Send(FString Message);

	FReceiveSocketDelegate ReceiveSocketDelegate;
	FDisconnectDelegate	DisconnectDelegate;

protected:

	FSocket* ConnectSocket;
	uint32 SendDataSize;
	uint32 RecDataSize;
	TArray<uint8> ReceiveData;
	FRunnableThread* pThread;
	bool bThreadStop;

};
