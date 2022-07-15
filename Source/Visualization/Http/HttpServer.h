

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HTTPServer/Public/HttpServerModule.h"
#include "HTTPServer/Public/IHttpRouter.h"
#include "CoreMinimal.h"
#include "HttpServer.generated.h"



UCLASS()
class VISUALIZATION_API AHttpServer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHttpServer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestReceivedDelegate, FString, Body);

	FHttpServerModule *Server;
	TSharedPtr<IHttpRouter> Router;
	FHttpRouteHandle RouteHandle;
	bool bClosed;
	
	UPROPERTY(BlueprintAssignable, Category = HttpServer)
	FRequestReceivedDelegate OnRequestReceived;

	UFUNCTION(BlueprintCallable, Category = httpServer)
		void startHttpServer(int port);

	UFUNCTION(BlueprintCallable, Category = httpServer)
		void stopHttpServer();

};
