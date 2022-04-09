// Fill out your copyright notice in the Description page of Project Settings.


#include "HttpClient.h"

#include "string"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"


// Sets default values
AHttpClient::AHttpClient()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHttpClient::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AHttpClient::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//中文编码~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool AHttpClient::IsAllowedChar(UTF8CHAR InChar)
{
	static char AllowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-+=_.~:/#@?&";
	static bool bTableFilled = false;
	static bool AllowedTable[256] = { false };

	if (!bTableFilled)
	{
		for (int32 Idx = 0; Idx < UE_ARRAY_COUNT(AllowedChars) - 1; ++Idx)
		{
			const uint8 AllowedCharIdx = static_cast<uint8>(AllowedChars[Idx]);
			check(AllowedCharIdx < UE_ARRAY_COUNT(AllowedTable));
			AllowedTable[AllowedCharIdx] = true;
		}

		bTableFilled = true;
	}

	return AllowedTable[InChar];
}

FString AHttpClient::UrlEncode(const FString& InString)
{
	const FTCHARToUTF8 Converter(*InString);
	const UTF8CHAR* UTF8Data = (UTF8CHAR*)Converter.Get();
	FString EncodedString = TEXT("");

	TCHAR Buffer[2] = { 0, 0 };

	for (int32 ByteIdx = 0, Length = Converter.Length(); ByteIdx < Length; ++ByteIdx)
	{
		const UTF8CHAR ByteToEncode = UTF8Data[ByteIdx];

		if (IsAllowedChar(ByteToEncode))
		{
			Buffer[0] = ByteToEncode;
			FString TmpString = Buffer;
			EncodedString += TmpString;
		}
		else if (ByteToEncode != '\0')
		{
			EncodedString += TEXT("%");
			EncodedString += FString::Printf(TEXT("%.2X"), ByteToEncode);
		}
	}
	return EncodedString;
}

//代理函数~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AHttpClient::OnRequestStringComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessful)
{
	if (HttpRequest.IsValid() || HttpResponse.IsValid() || bSuccessful || EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
	{
		TArray<uint8> UintContent = HttpResponse->GetContent();
		const std::string StdContent(reinterpret_cast<const char*>(UintContent.GetData()), UintContent.Num());
		const FString Data = UTF8_TO_TCHAR(StdContent.c_str());
		OnRequestStringDelegate.ExecuteIfBound(Data);
		UE_LOG(LogTemp, Warning, TEXT("RequestData Success: %s"), *Data);
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("RequestData false!"));
}

void AHttpClient::OnRequestTextureComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessful)
{
	if ( bSuccessful && HttpResponse.IsValid() && HttpResponse->GetContentLength() > 0 )
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrappers[3] =
		{
			ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
            ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
            ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
        };

		for ( auto ImageWrapper : ImageWrappers )
		{
			if ( ImageWrapper.IsValid() && ImageWrapper->SetCompressed(HttpResponse->GetContent().GetData(), HttpResponse->GetContentLength()) )
			{
				TArray64<uint8> RawData;
				if ( ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData) )
				{
					const int Width = ImageWrapper->GetWidth();
					const int Height = ImageWrapper->GetHeight();
					UTexture2D* Texture2D = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
					if (Texture2D)
					{
						void* TextureData = Texture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
						FMemory::Memcpy(TextureData, RawData.GetData(), RawData.Num());
						Texture2D->GetPlatformData()->Mips[0].BulkData.Unlock();
						Texture2D->UpdateResource();
						UE_LOG(LogTemp, Error, TEXT("RequestData Success!"));
						OnRequestTextureDelegate.ExecuteIfBound(Texture2D);
					}
				}
			}
		}
	}


}

void AHttpClient::OnRequestObjectComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSuccessful)
{
	
}

//蓝图函数~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void AHttpClient::RequestString(FString URL, FOnRequestStringDelegate OnComplete)
{
	OnRequestStringDelegate = OnComplete;
	const FString EncodeURL = AHttpClient::UrlEncode(URL);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(EncodeURL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
	
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpClient::OnRequestStringComplete);
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("RequestString"));
}

void AHttpClient::RequestTexture(FString URL, FOnRequestTextureDelegate OnComplete)
{
	OnRequestTextureDelegate = OnComplete;
	const FString EncodeURL = AHttpClient::UrlEncode(URL);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(EncodeURL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
	
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpClient::OnRequestTextureComplete);
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("RequestTexture"));
}

void AHttpClient::RequestObject(FString URL, FOnRequestObjectDelegate OnComplete)
{
	OnRequestObjectDelegate = OnComplete;
	const FString EncodeURL = AHttpClient::UrlEncode(URL);
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(EncodeURL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/octet-stream"));
	
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &AHttpClient::OnRequestObjectComplete);
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Warning, TEXT("RequestObject"));
}
