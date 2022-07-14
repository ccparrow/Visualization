// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HTTPServer/Public/HttpServerModule.h"
#include "HTTPServer/Public/IHttpRouter.h"
#include "CoreMinimal.h"
#include "HttpServer.generated.h"

UCLASS()
class VISUALIZATION_API AHttpServer : public AActor
{
	GENERATED_BODY()

private:
	FHttpServerModule *Server;
	TSharedPtr<IHttpRouter> Router;
	FHttpRouteHandle RouteHandle;
	bool bClosed;

public:
	// Sets default values for this actor's properties
	AHttpServer();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestReceivedDelegate, FString, Body);

	UPROPERTY(BlueprintAssignable, Category = "Http")
	FRequestReceivedDelegate OnRequestReceived;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};