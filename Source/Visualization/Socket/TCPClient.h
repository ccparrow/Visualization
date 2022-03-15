
// Fill out your copyright notice in the Description page of Project Settings.
 
#pragma once
 
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketRSThread.h"
#include "TCPClient.generated.h"
 
UCLASS(BlueprintType, Blueprintable)
class VISUALIZATION_API ATCPClient : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATCPClient();
 
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
 


	class FSocket* ClientSocket;
	TArray<class USocketRSThread*> SocketThreadArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TCPSocketClient)
	int32 SendDataSize = 16384;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TCPSocketClient)
	int32 RecDataDize = 16384;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, Category = TCPSocketClient)
		FReceiveSocketDelegate ReceiveClientDelegate;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
 


	UFUNCTION(BlueprintCallable, Category = TCPSocketClient)
		bool CreateClient(FString IP, int32 Port);
 
	UFUNCTION(BlueprintCallable, Category = TCPSocketClient)
		void SendToServer(FString Message);

	UFUNCTION(BlueprintCallable, Category = TCPSocketClient)
		void CloseClient();

	UFUNCTION()
		void OnServerDisconnect(class USocketRSThread* pThread);


};