
// Fill out your copyright notice in the Description page of Project Settings.

#include "TCPServer.h"
#include "Networking/Public/Common/TcpSocketBuilder.h"
#include "SocketRSThread.h"


// Sets default values
ATCPServer::ATCPServer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATCPServer::BeginPlay()
{
	Super::BeginPlay();

}

void ATCPServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	CloseServer();
}

// Called every frame
void ATCPServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



bool ATCPServer::CreateServer(const FString& IP, int32 Port)
{
	FIPv4Address ServerAddr;
	if (!FIPv4Address::Parse(IP, ServerAddr))
	{
		UE_LOG(LogTemp, Error, TEXT("Server Ip %s:%i is illegal"), *IP, Port);
		return false;
	}
	ServerSocket = FTcpSocketBuilder(TEXT("Socket Listener"))
		.AsReusable()
		.AsBlocking()
		.BoundToAddress(ServerAddr)
		.BoundToPort(Port)
		.Listening(8)
		.WithReceiveBufferSize(RecDataDize)
		.WithSendBufferSize(SendDataSize);
	if (ServerSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server Create Success---%s:%i"), *IP, Port);
		CreateSocketDelegate.Broadcast(true);
		GetWorld()->GetTimerManager().SetTimer(ConnectCheckHandler, this, &ATCPServer::ConnectCheck, 1, true);
		return true;
	}

	UE_LOG(LogTemp, Error, TEXT("Server Create Failed!"));
	CreateSocketDelegate.Broadcast(false);
	return false;

}

void ATCPServer::ConnectCheck()
{
	bool bPending = false;
	if (ServerSocket->HasPendingConnection(bPending) && bPending)
	{
		//有新的socket连接进来
		TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		FSocket* RecSocket = ServerSocket->Accept(*RemoteAddress, TEXT("Receive Socket"));
		USocketRSThread* RSThread = NewObject<USocketRSThread>();
		SocketThreadArray.Add(RSThread);
		RSThread->ReceiveSocketDelegate = ReceiveServerDelegate;
		RSThread->DisconnectDelegate.AddDynamic(this, &ATCPServer::OnClientDisconnect);
		RSThread->Start(RecSocket, SendDataSize, RecDataDize);
		NewConnectDelegate.Broadcast(RemoteAddress->ToString(false), RemoteAddress->GetPort());
	}
}

void ATCPServer::SendToClient(FString Message)
{

	for (auto SocketThread : SocketThreadArray)
	{
		SocketThread->Send(Message);
	}
}

void ATCPServer::CloseServer()
{
	if (ServerSocket)
	{
		ServerSocket->Close();
		for (auto RecThreald : SocketThreadArray)
		{
			RecThreald->Stop();
		}
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);

	}
}

void ATCPServer::OnClientDisconnect(USocketRSThread* pThread)
{
	UE_LOG(LogTemp, Warning, TEXT("Client Disconnect"));
	SocketThreadArray.Remove(pThread);
}
