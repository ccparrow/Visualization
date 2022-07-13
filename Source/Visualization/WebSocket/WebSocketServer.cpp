// Fill out your copyright notice in the Description page of Project Settings.


#include "WebSocketServer.h"
#include "IWebSocketNetworkingModule.h"
#include "WebSocketNetworkingDelegates.h"
#include "string"

AWebSocketServer::AWebSocketServer()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWebSocketServer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWebSocketServer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	WebSocketServerTick(DeltaTime);
}

void AWebSocketServer::BeginDestroy()
{
	Super::BeginDestroy();
	Stop();
}

bool AWebSocketServer::Start(int Port)
{
	FWebSocketClientConnectedCallBack CallBack;
	CallBack.BindUObject(this, &AWebSocketServer::OnWebSocketClientConnected);

	Server = FModuleManager::Get().LoadModuleChecked<IWebSocketNetworkingModule>(TEXT("WebSocketNetworking")).CreateServer();

	if (!Server || !Server->Init(Port, CallBack))
	{
		Server.Reset();
		return false;
	}

	return true;
}

void AWebSocketServer::Stop()
{
	if (IsRunning()) {
		Server.Reset();
	}
}

bool AWebSocketServer::WebSocketServerTick(float DeltaTime)
{
	if (IsRunning()) {
		Server->Tick();
		return true;
	}
	else {
		return false;
	}
}

void AWebSocketServer::SendByID(FGuid ClientId, FString message)
{
	if (FWebSocketConnection* Connection = Connections.FindByPredicate([&ClientId](const FWebSocketConnection& InConnection)
	{ return InConnection.Id == ClientId; }))
	{
		TArray<uint8> uint8Array;  
		uint8Array.SetNum(message.Len());  
		memcpy(uint8Array.GetData(), TCHAR_TO_ANSI(*message), message.Len());

		Connection->Socket->Send(uint8Array.GetData(), uint8Array.Num(), /*PrependSize=*/false);
	}
}

void AWebSocketServer::Send(const FString message)
{
	TArray<uint8> uint8Array;  
	uint8Array.SetNum(message.Len());  
	memcpy(uint8Array.GetData(), TCHAR_TO_ANSI(*message), message.Len());

	for (auto& ws : Connections) {
		ws.Socket->Send(uint8Array.GetData(), uint8Array.Num(), /*PrependSize=*/false);
	}
}

bool AWebSocketServer::IsRunning() const
{
	return !!Server;
}

void AWebSocketServer::OnWebSocketClientConnected(INetworkingWebSocket* Socket)
{
	UE_LOG(LogTemp, Warning, TEXT("----Client Connected"));
	if (ensureMsgf(Socket, TEXT("Socket was null while creating a new websocket connection.")))
	{
		FWebSocketConnection Connection = FWebSocketConnection{ Socket };

		FWebSocketPacketReceivedCallBack ReceiveCallBack;
		ReceiveCallBack.BindUObject(this, &AWebSocketServer::ReceivedRawPacket, Connection.Id);
		Socket->SetReceiveCallBack(ReceiveCallBack);

		FWebSocketInfoCallBack CloseCallback;
		CloseCallback.BindUObject(this, &AWebSocketServer::OnSocketClose, Socket);
		Socket->SetSocketClosedCallBack(CloseCallback);

		Connections.Add(MoveTemp(Connection));
	}
}

void AWebSocketServer::ReceivedRawPacket(void* Data, int32 Size, FGuid ClientId)
{
	TArrayView<uint8> dataArrayView = MakeArrayView(static_cast<uint8*>(Data), Size);
	const std::string cstr(reinterpret_cast<const char*>(
		dataArrayView.GetData()),
		dataArrayView.Num());
	FString frameAsFString = UTF8_TO_TCHAR(cstr.c_str());

	OnClientMessage(frameAsFString);
}

void AWebSocketServer::OnSocketClose(INetworkingWebSocket* Socket)
{
	int32 Index = Connections.IndexOfByPredicate([Socket](const FWebSocketConnection& Connection) { return Connection.Socket == Socket; });

	UE_LOG(LogTemp, Error, TEXT("----Client Disconnected"));
	if (Index != INDEX_NONE)
	{
		OnConnectionClosed().Broadcast(Connections[Index].Id);
		Connections.RemoveAtSwap(Index);
	}
}