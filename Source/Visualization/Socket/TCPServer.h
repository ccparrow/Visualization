
// Fill out your copyright notice in the Description page of Project Settings.
 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketRSThread.h"
#include "TCPServer.generated.h"
 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCreateSocketDelegate, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewConnectDelegate, FString, RemoteIP, int32, RemotePort);
 
 
UCLASS(BlueprintType, Blueprintable)
class VISUALIZATION_API ATCPServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATCPServer();
 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 


	class FSocket* ServerSocket;
	TArray<USocketRSThread*> SocketThreadArray;
	FTimerHandle ConnectCheckHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TCPSocketServer)
		int32 SendDataSize = 16384;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TCPSocketServer)
		int32 RecDataDize = 16384;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = TCPSocketServer)
		FCreateSocketDelegate CreateSocketDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = TCPSocketServer)
		FNewConnectDelegate NewConnectDelegate;
	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = TCPSocketServer)
		FReceiveSocketDelegate ReceiveServerDelegate;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 


	UFUNCTION(BlueprintCallable, Category = TCPSocketServer)
		bool CreateServer(const FString& IP, int32 Port);

	void ConnectCheck();
 
	UFUNCTION(BlueprintCallable, Category = TCPSocketServer)
		void SendToClient(FString Message);
 
	UFUNCTION(BlueprintCallable, Category = TCPSocketServer)
		void CloseServer();

	UFUNCTION()
		void OnClientDisconnect(class USocketRSThread* pThread);


};
 