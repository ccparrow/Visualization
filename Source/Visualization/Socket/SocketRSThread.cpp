
// Fill out your copyright notice in the Description page of Project Settings.


#include "SocketRSThread.h"
#include "SocketSubsystem.h"


uint32 USocketRSThread::Run()
{
	while (!bThreadStop)
	{
		//这个地方是之前将socket设置为阻塞模式 在这里5s内判断是否断开连接
		uint32 Size;
		bool bDisconnect = false;
		ConnectSocket->HasPendingConnection(bDisconnect);
		ConnectSocket->Wait(ESocketWaitConditions::WaitForReadOrWrite, FTimespan(0, 0, 5));
		if (bDisconnect)
		{
			//UE_LOG(LogTemp, Warning, TEXT(" doesn't Connect "));
			Stop();
			DisconnectDelegate.Broadcast(this);
			continue;
		}

		/** 处理接收数据 */
		if (ConnectSocket && ConnectSocket->HasPendingData(Size))
		{
			ReceiveData.Init(0, Size);
			int32 Readed;
			ConnectSocket->Recv(ReceiveData.GetData(), Size, Readed);
			FString ReceivedString = FString(ANSI_TO_TCHAR(reinterpret_cast<const char*>(ReceiveData.GetData())));
			ReceiveSocketDelegate.Broadcast(ReceivedString);
		}
	}
	return 0;
}

void USocketRSThread::Start(FSocket* Socket, uint32 InSendDataSize, uint32 InRecDataSize)
{
	this->ConnectSocket = Socket;
	this->SendDataSize = InSendDataSize;
	this->RecDataSize = InRecDataSize;
	FRunnableThread::Create(this, TEXT("Receive Threald"));
}

void USocketRSThread::Stop()
{
	bThreadStop = true;
	ConnectSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectSocket);
	ConnectSocket = nullptr;
}

void USocketRSThread::Send(FString Message)
{
	///** 处理发送数据 */
	TCHAR* SendMessage = Message.GetCharArray().GetData();
	int32 size = FCString::Strlen(SendMessage) + 1;
	int32 sent = 0;

	if (ConnectSocket && ConnectSocket->Send((uint8*)TCHAR_TO_UTF8(SendMessage), size, sent))
	{
		UE_LOG(LogTemp, Warning, TEXT("___Send Succeed: %s"), *Message);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("___Send Failed!"));
	}

}
