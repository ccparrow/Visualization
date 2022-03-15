
// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPClient.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "SocketRSThread.h"


// Sets default values
ATCPClient::ATCPClient()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATCPClient::BeginPlay()
{
	Super::BeginPlay();

}

void ATCPClient::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CloseClient();
}

// Called every frame
void ATCPClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



bool ATCPClient::CreateClient(FString IP, int32 Port)
{
	//创建客户端
	ClientSocket = FTcpSocketBuilder(TEXT("Client Socket"))
		.AsReusable()
		.AsBlocking()
		.WithReceiveBufferSize(RecDataDize)
		.WithSendBufferSize(SendDataSize);

	if (!ClientSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Create Client Socket Error!"));
		return false;
	}

	//连接服务器
	FIPv4Endpoint ServerEndpoint;
	FIPv4Endpoint::Parse(IP, ServerEndpoint);
	TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Success = true;
	addr->SetIp(*IP, Success);
	if (!Success)
	{
		return false;
	}
	addr->SetPort(Port);

	if (ClientSocket->Connect(*addr))
	{
		USocketRSThread* RSThread = NewObject<USocketRSThread>();
		SocketThreadArray.Add(RSThread);
		RSThread->ReceiveSocketDelegate = ReceiveClientDelegate;
		RSThread->DisconnectDelegate.AddDynamic(this, &ATCPClient::OnServerDisconnect);
		RSThread->Start(ClientSocket, SendDataSize, RecDataDize);
		UE_LOG(LogTemp, Warning, TEXT("Client Connect Success---%s:%i"), *IP, Port);
		return true; 
	}

	ESocketErrors LastErr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLastErrorCode();
	UE_LOG(LogTemp, Warning, TEXT("Connect failed with error code (%d) error (%s)"), LastErr, ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetSocketError(LastErr));
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
	return false;

}

void ATCPClient::SendToServer(FString Message)
{
	for (auto SocketThread : SocketThreadArray)
	{
		SocketThread->Send(Message);
	}
}

void ATCPClient::CloseClient()
{
	if (ClientSocket)
	{
		for (auto RecThreald : SocketThreadArray)
		{
			RecThreald->Stop();
		}
	}
}

void ATCPClient::OnServerDisconnect(USocketRSThread* pThread)
{
	UE_LOG(LogTemp, Warning, TEXT("Server Disconnect"));
	SocketThreadArray.Remove(pThread);
}