


#include "HttpServer.h"
#include "HTTPServer/Public/HttpPath.h"
#include "HTTPServer/Public/HttpResultCallback.h"
#include "HTTPServer/Public/HttpServerRequest.h"
#include "HTTPServer/Public/HttpServerResponse.h"

// Sets default values
AHttpServer::AHttpServer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHttpServer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHttpServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AHttpServer::startHttpServer(int port)
{
	Server = &FHttpServerModule::Get();
	UE_LOG(LogTemp, Log, TEXT("BeginHttpServer"));

	FHttpPath HttpPath(TEXT("/1")); // 指定(http://127.0.0.1/1
	Router = Server->GetHttpRouter(port);  // 端口

	RouteHandle = Router->BindRoute(
	HttpPath,
	EHttpServerRequestVerbs::VERB_GET | EHttpServerRequestVerbs::VERB_POST,
	[this](const FHttpServerRequest &Request, const FHttpResultCallback &OnComplete) {
	UE_LOG(LogTemp, Log, TEXT("BindRoute"));
		
	FUTF8ToTCHAR WByteBuffer(reinterpret_cast<const ANSICHAR *>(Request.Body.GetData()), Request.Body.Num());
	FString str = WByteBuffer.Get();

	UE_LOG(LogTemp, Log, TEXT("%s"), *str);

	// http://127.0.0.1/test_api?abc=123&name=hello
	for (auto param : Request.QueryParams)
	{
		UE_LOG(LogTemp, Log, TEXT("%s->%s"), *param.Key, *param.Value);
	}

	if (OnRequestReceived.IsBound())
	{
		OnRequestReceived.Broadcast(str);
	}

	OnComplete(FHttpServerResponse::Ok());

	return true;
});

	Server->StartAllListeners();
}

void AHttpServer::stopHttpServer()
{
	if (bClosed)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Stop HTTP"));
	Server->StopAllListeners();
	Server->HasPendingListeners();
	if (Router)
	{
		Router->UnbindRoute(RouteHandle);
	}
	bClosed = true;
}