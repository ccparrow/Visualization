// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpModule.h"
#include "HttpClient.generated.h"

//代理~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestStringDelegate, FString, String);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestTextureDelegate, UTexture2D*, Texture);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRequestObjectDelegate, UObject*, Object);


UCLASS()
class VISUALIZATION_API AHttpClient : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHttpClient();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//中文编码~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	bool IsAllowedChar(UTF8CHAR InChar);
	FString UrlEncode( const FString &InString);
	
	//代理函数~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	void OnRequestStringComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessful);
	FOnRequestStringDelegate OnRequestStringDelegate;
	void OnRequestTextureComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessful);
	FOnRequestTextureDelegate OnRequestTextureDelegate;
	void OnRequestObjectComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessful);
	FOnRequestObjectDelegate OnRequestObjectDelegate;
	
	//蓝图函数~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	UFUNCTION(BlueprintCallable, Category = HttpClient)
		void RequestString(FString URL, FOnRequestStringDelegate OnComplete);
	UFUNCTION(BlueprintCallable, Category = HttpClient)
        void RequestTexture(FString URL, FOnRequestTextureDelegate OnComplete);
	UFUNCTION(BlueprintCallable, Category = HttpClient)
		void RequestObject(FString URL, FOnRequestObjectDelegate OnComplete);

	
};
