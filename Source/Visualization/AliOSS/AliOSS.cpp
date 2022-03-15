// Fill out your copyright notice in the Description page of Project Settings.


#include "AliOSS.h"
#include <alibabacloud/oss/OssClient.h>
#include <memory>
#include <fstream>

using namespace AlibabaCloud::OSS;

// Sets default values
AAliOSS::AAliOSS()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAliOSS::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AAliOSS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




bool AAliOSS::CreateBucket(FString InAccessKeyId, FString InAccessKeySecret, FString InEndpoint, FString InBucketName)
{
	OssAccessKeyId = InAccessKeyId;
	OssAccessKeySecret = InAccessKeySecret;
	OssEndpoint = InEndpoint;
	OssBucketName = InBucketName;
	
	std::string AccessKeyId = TCHAR_TO_UTF8(*OssAccessKeyId);
	std::string AccessKeySecret = TCHAR_TO_UTF8(*OssAccessKeySecret);
	std::string Endpoint = TCHAR_TO_UTF8(*OssEndpoint);
	std::string BucketName = TCHAR_TO_UTF8(*OssBucketName);
	
	/* 初始化网络等资源 */
	InitializeSdk();
	
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	
	/* 指定新创建bucket的名称、存储类型和ACL */
	CreateBucketRequest request(BucketName, StorageClass::IA, CannedAccessControlList::PublicReadWrite);
	
	/* 创建bucket */
	auto outcome = client.CreateBucket(request);
	
	if (!outcome.isSuccess()) {
		/* 异常处理 */
		std::cout << "CreateBucket fail" <<
			",code:" << outcome.error().Code() <<
			",message:" << outcome.error().Message() <<
			",requestId:" << outcome.error().RequestId() << std::endl;
		ShutdownSdk();
		UE_LOG(LogTemp, Error, TEXT("CreateBucket fail"));
		return false;
	}
	
	/* 释放网络等资源 */
	ShutdownSdk();
	UE_LOG(LogTemp, Warning, TEXT("CreateBucket Success"));
	return true;

}

bool AAliOSS::PutObject(FString OssFileName, FString LocalFileName)
{
	std::string AccessKeyId = TCHAR_TO_UTF8(*OssAccessKeyId);
	std::string AccessKeySecret = TCHAR_TO_UTF8(*OssAccessKeySecret);
	std::string Endpoint = TCHAR_TO_UTF8(*OssEndpoint);
	std::string BucketName = TCHAR_TO_UTF8(*OssBucketName);
	
	/* yourObjectName表示上传文件到OSS时需要指定包含文件后缀在内的完整路径，例如abc/efg/123.jpg */
	std::string ObjectName = TCHAR_TO_UTF8(*OssFileName);
	std::string LocalName = TCHAR_TO_UTF8(*LocalFileName);
	
	/* 初始化网络等资源 */
	InitializeSdk();
	
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	
	/* 上传文件 */
	/* yourLocalFilename由本地文件路径加文件名包括后缀组成，例如/users/local/myfile.txt */
	auto outcome = client.PutObject(BucketName, ObjectName, LocalName);
	
	if (!outcome.isSuccess()) {
		/* 异常处理 */
		std::cout << "PutObject fail" <<
			",code:" << outcome.error().Code() <<
			",message:" << outcome.error().Message() <<
			",requestId:" << outcome.error().RequestId() << std::endl;
	
		ShutdownSdk();
		UE_LOG(LogTemp, Error, TEXT("PutObject fail"));
		return false;
	}
	
	/* 释放网络等资源 */
	ShutdownSdk();
	UE_LOG(LogTemp, Warning, TEXT("PutObject success"));
	return true;
}

bool AAliOSS::GetObject(FString OssFileName, FString LocalFileName)
{
	std::string AccessKeyId = TCHAR_TO_UTF8(*OssAccessKeyId);
	std::string AccessKeySecret = TCHAR_TO_UTF8(*OssAccessKeySecret);
	std::string Endpoint = TCHAR_TO_UTF8(*OssEndpoint);
	std::string BucketName = TCHAR_TO_UTF8(*OssBucketName);
	std::string ObjectName = TCHAR_TO_UTF8(*OssFileName);
	/*设置下载文件的文件名*/
	std::string FileNametoSave = TCHAR_TO_UTF8(*LocalFileName);
	
	/*初始化网络等资源*/
	InitializeSdk();
	
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	
	/*获取文件到本地文件*/
	GetObjectRequest request(BucketName, ObjectName);
	request.setResponseStreamFactory([=]() {return std::make_shared<std::fstream>(FileNametoSave, std::ios_base::out | std::ios_base::in | std::ios_base::trunc | std::ios_base::binary); });
	
	auto outcome = client.GetObject(request);
	
	
	if (outcome.isSuccess()) {
		std::cout << "GetObjectToFile success" << outcome.result().Metadata().ContentLength() << std::endl;
		UE_LOG(LogTemp, Warning, TEXT("GetObject success"));
	}
	else {
		/*异常处理*/
		std::cout << "GetObjectToFile fail" <<
			",code:" << outcome.error().Code() <<
			",message:" << outcome.error().Message() <<
			",requestId:" << outcome.error().RequestId() << std::endl;
		ShutdownSdk();
		UE_LOG(LogTemp, Error, TEXT("GetObject fail"));
		return false;
	}
	
	/*释放网络等资源*/
	ShutdownSdk();
	return true;

}

bool AAliOSS::ListObject(TArray<FString>& NameArray, TArray<int>& SizeArray)
{
	std::string AccessKeyId = TCHAR_TO_UTF8(*OssAccessKeyId);
	std::string AccessKeySecret = TCHAR_TO_UTF8(*OssAccessKeySecret);
	std::string Endpoint = TCHAR_TO_UTF8(*OssEndpoint);
	std::string BucketName = TCHAR_TO_UTF8(*OssBucketName);
	
	/* 初始化网络等资源 */
	InitializeSdk();
	
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	
	/* 列举文件 */
	ListObjectsRequest request(BucketName);
	auto outcome = client.ListObjects(request);
	
	if (!outcome.isSuccess()) {
		/* 异常处理 */
		std::cout << "ListObjects fail" <<
			",code:" << outcome.error().Code() <<
			",message:" << outcome.error().Message() <<
			",requestId:" << outcome.error().RequestId() << std::endl;
		ShutdownSdk();
		UE_LOG(LogTemp, Error, TEXT("ListObjects fail"));
		return false;
	}
	else {
		for (const auto& object : outcome.result().ObjectSummarys()) {
			std::cout << "object" <<
				",name:" << object.Key() <<
				",size:" << object.Size() <<
				",last modified time:" << object.LastModified() << std::endl;
	
			FString ObjectName(object.Key().c_str());
			int ObjectSize = object.Size();
			NameArray.Add(ObjectName);
			SizeArray.Add(ObjectSize);
			UE_LOG(LogTemp, Warning, TEXT("object: %s,size: %i"), *ObjectName, ObjectSize);
		}
	}
	
	/* 释放网络等资源 */
	ShutdownSdk();
	return true;
}

void AAliOSS::ProgressCallback(size_t increment, int64_t transfered, int64_t total, void* userData)
{
	std::cout << "ProgressCallback[" << userData << "] => " <<
		increment << " ," << transfered << "," << total << std::endl;
	UE_LOG(LogTemp, Warning, TEXT("increment: %i, transfered: %i, total: %i"), increment, transfered, total);
}

bool AAliOSS::PutObjectProgress(FString LocalFileName)
{
	std::string AccessKeyId = TCHAR_TO_UTF8(*OssAccessKeyId);
	std::string AccessKeySecret = TCHAR_TO_UTF8(*OssAccessKeySecret);
	std::string Endpoint = TCHAR_TO_UTF8(*OssEndpoint);
	std::string BucketName = TCHAR_TO_UTF8(*OssBucketName);
	std::string FileName = TCHAR_TO_UTF8(*LocalFileName);
	
	/* 初始化网络等资源 */
	InitializeSdk();
	
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	
	std::shared_ptr<std::iostream> content = std::make_shared<std::fstream>(__FILE__, std::ios::in | std::ios::binary);
	PutObjectRequest request(BucketName, FileName, content);
	TransferProgress progressCallback = { ProgressCallback , this };
	request.setTransferProgress(progressCallback);
	
	auto outcome = client.PutObject(request);
	
	if (!outcome.isSuccess()) {
		/* 异常处理 */
		std::cout << "petObject fail" <<
			",code:" << outcome.error().Code() <<
			",message:" << outcome.error().Message() <<
			",requestId:" << outcome.error().RequestId() << std::endl;
		ShutdownSdk();
		return false;
	}
	
	/* 释放网络等资源 */
	ShutdownSdk();
	return true;
}

bool AAliOSS::GetObjectProgress(FString OssFileName)
{
	std::string AccessKeyId = TCHAR_TO_UTF8(*OssAccessKeyId);
	std::string AccessKeySecret = TCHAR_TO_UTF8(*OssAccessKeySecret);
	std::string Endpoint = TCHAR_TO_UTF8(*OssEndpoint);
	std::string BucketName = TCHAR_TO_UTF8(*OssBucketName);
	std::string FileName = TCHAR_TO_UTF8(*OssFileName);
	
	/* 初始化网络等资源 */
	InitializeSdk();
	
	ClientConfiguration conf;
	OssClient client(Endpoint, AccessKeyId, AccessKeySecret, conf);
	
	/* 获取文件 */
	GetObjectRequest request(BucketName, FileName);
	TransferProgress progressCallback = { ProgressCallback , this };
	request.setTransferProgress(progressCallback);
	
	auto outcome = client.GetObject(request);
	
	if (!outcome.isSuccess()) {
		/* 异常处理 */
		std::cout << "getObject fail" <<
			",code:" << outcome.error().Code() <<
			",message:" << outcome.error().Message() <<
			",requestId:" << outcome.error().RequestId() << std::endl;
		ShutdownSdk();
		return false;
	}
	
	/* 释放网络等资源 */
	ShutdownSdk();
	return true;
}

