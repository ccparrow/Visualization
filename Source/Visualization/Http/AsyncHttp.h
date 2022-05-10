//@ https://www.cnblogs.com/shiroe/p/15500780.html
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Interfaces/IHttpRequest.h"
#include "AsyncHttp.generated.h"



// 定义委托,作为返回
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRequestCompleteDelegate, const FString, Data); //注意此处的参数类型



UCLASS()
class VISUALIZATION_API UAsyncHttp : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
		
public:
	UAsyncHttp();
	/*
	 * url: 
	 * content: if POST
	 * verb: GET,POST,PUT,DELETE,CUSTOM
	 * contentType: application/json,application/octet-stream,application/x-www-form-urlencoded
	 */
	UFUNCTION(BlueprintCallable, Category = "AsyncHttp", meta=( BlueprintInternalUseOnly="true" ))
		static UAsyncHttp* AsyncHttpRequest(const FString& url, const FString& content, const FString& verb = "POST", const FString& contentType = "application/json");
	
	void HttpRequestStar(const FString& url, const FString& content, const FString& verb, const FString& contentType);
	void HttpRequestBack(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful);

	//输出
	UPROPERTY(BlueprintAssignable)
	FRequestCompleteDelegate Succeed;
	UPROPERTY(BlueprintAssignable)
	FRequestCompleteDelegate Fail;
};