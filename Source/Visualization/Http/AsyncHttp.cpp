//@ https://www.cnblogs.com/shiroe/p/15500780.html
#include "AsyncHttp.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"

UAsyncHttp::UAsyncHttp()
{
	if (HasAnyFlags(RF_ClassDefaultObject) == false)
	{
		AddToRoot(); //防止自动GC
	}
}

 //蓝图请求
UAsyncHttp* UAsyncHttp::AsyncHttpRequest(const FString& url, const FString& content, const FString& verb, const FString& contentType)
{
	UAsyncHttp* AsyncHttp = NewObject<UAsyncHttp>();
	AsyncHttp->HttpRequestStar(url, content, verb, contentType);	
	return AsyncHttp;
}

//开始请求
void UAsyncHttp::HttpRequestStar(const FString& url, const FString& content, const FString& verb, const FString& contentType)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> request = FHttpModule::Get().CreateRequest();
	request->SetVerb(verb);
	request->SetURL(url);
	request->SetHeader(TEXT("Content-Type"), contentType);
	//request->SetTimeout(200);
	request->SetContentAsString(content); 
	//request->OnRequestProgress().BindUObject() 绑定进度
	request->OnProcessRequestComplete().BindUObject(this, &UAsyncHttp::HttpRequestBack); //请求回调
	request->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("HttpRequest"));
}
//请求回调
void UAsyncHttp::HttpRequestBack(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
{
	if (bWasSuccessful && response.IsValid() && EHttpResponseCodes::IsOk(response->GetResponseCode()))
		{
			FString data = response->GetContentAsString();
			Succeed.Broadcast(data); // 成功的执行
			//UE_LOG(LogTemp, Warning, TEXT("RequestData Success: %s"), *data);
			RemoveFromRoot();
			return;
		}
	Fail.Broadcast({}); // 失败的执行
	UE_LOG(LogTemp, Error, TEXT("RequestData false!"));
	RemoveFromRoot();
}
