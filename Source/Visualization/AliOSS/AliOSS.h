// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AliOSS.generated.h"

UCLASS(MinimalAPI, BlueprintType)
class  AAliOSS : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAliOSS();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = AccessKeyId), Category = AliOSS)
		FString OssAccessKeyId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = AccessKeyId), Category = AliOSS)
		FString OssAccessKeySecret;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = AccessKeyId), Category = AliOSS)
		FString OssEndpoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = AccessKeyId), Category = AliOSS)
		FString OssBucketName;

	UFUNCTION(BlueprintCallable, Category = AliOSS)
		bool CreateBucket(FString InAccessKeyId, FString InAccessKeySecret, FString InEndpoint, FString InBucketName);
	UFUNCTION(BlueprintCallable, Category = AliOSS)
		bool PutObject(FString OssFileName, FString LocalFileName);
	UFUNCTION(BlueprintCallable, Category = AliOSS)
		bool GetObject(FString OssFileName, FString LocalFileName);
	UFUNCTION(BlueprintCallable, Category = AliOSS)
		bool ListObject(TArray<FString>& NameArray, TArray<int>& SizeArray);

	
		static void ProgressCallback(size_t increment, int64_t transfered, int64_t total, void* userData);
	UFUNCTION(BlueprintCallable, Category = AliOSS)
		bool PutObjectProgress(FString LocalFileName);
	UFUNCTION(BlueprintCallable, Category = AliOSS)
		bool GetObjectProgress(FString OssFileName);


};
