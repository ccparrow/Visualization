// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "V_Library.h"
#include "Core.h"
#include "string"
#include "SocketSubsystem.h"//IP
#include "Visualization.h"
#include "Engine\Classes\Engine\ObjectLibrary.h"//资源加载
#include "ImageWrapper\Public\IImageWrapper.h"//base64解码图片
#include "ImageWrapper\Public\IImageWrapperModule.h"
#include "UObject\UObjectGlobals.h"
#include "Thread/V_Runnable.h"//多线程
#include "HAL/RunnableThread.h"
#include "Thread/V_AsyncTask.h"
#include "Async/AsyncJsonString.h"//异步JSON
#include "Async/AsyncJsonArray.h"


UV_Library::UV_Library(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

//自定义LOG
DEFINE_LOG_CATEGORY(VisualizationLOG);

//是否开发
bool UV_Library::Editor()
{
	return GEngine->IsEditor();
}

void UV_Library::IsEditor(eResult& Result)
{
	if (GEngine->IsEditor())
	{	
		Result = eResult::Success;
		return;
	}
	Result = eResult::Fail;
}

//打印
void UV_Library::PrintTo(FString String, float Duration, bool bScreen, bool bLog, float Scale, FLinearColor Color, eLogType Type)
{
	if (bScreen && GEngine->IsEditor())
	{
		GEngine->AddOnScreenDebugMessage(-1, Duration, Color.ToFColor(true), String, true, FVector2D(Scale, Scale));
	}
	if (bLog)
	{
		PrintToLog(String, Type);
	}
}
//打印到LOG
void UV_Library::PrintToLog(FString String, eLogType Type)
{
	if (Type == eLogType::Warning)
	{
		UE_LOG( VisualizationLOG, Warning, TEXT( "%s" ), *String );
	}
	else if (Type == eLogType::Error)
	{
		UE_LOG( VisualizationLOG, Error, TEXT( "%s" ), *String );
	}
	else
	{
		UE_LOG( VisualizationLOG, Log, TEXT( "%s" ), *String );
	}
}

//读取file
TArray<FString> UV_Library::ReadFile(eDirType DirType, FString Directory, FString FileName)
{
	TArray<FString> Result;

	if (DirType == eDirType::ProjectDir)
	{
		Directory = FPaths::ProjectDir() + Directory;
	}
	
	else if (DirType == eDirType::PluginsDir)
	{
		Directory = FPaths::ProjectPluginsDir() + Directory;
	}
	
	Directory += "/";
	Directory += FileName;

	FFileHelper::LoadFileToStringArray(Result, *Directory);
	return Result;
}
//写入file
bool UV_Library::WriteFile(eDirType DirType, TArray<FString> TextArray, FString Directory, FString FileName, bool bOverWrite)
{
	if (DirType == eDirType::ProjectDir)
	{
		Directory = FPaths::ProjectDir() + Directory;
	}
	
	else if (DirType == eDirType::PluginsDir)
	{
		Directory = FPaths::ProjectPluginsDir() + Directory;
	}
	
	Directory += "/";
	Directory += FileName;

	if (!bOverWrite)
	{
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*Directory))
		{
			return false;
		}
	}
	return FFileHelper::SaveStringArrayToFile(TextArray, *Directory);
}

//读取config
void UV_Library::ReadConfig(FString Directory, FString FileName, const FString& Section, const FString& Key, FString& Value, bool bAbsDir)
{
	if (bAbsDir)
	{
		Directory = FPaths::ProjectConfigDir() + Directory;
	}
	Directory += "/";
	Directory += FileName;

	GConfig->Flush(true, Directory);
	GConfig->GetString(*Section, *Key, Value, Directory);
}
//写入config
void UV_Library::WriteConfig(FString Directory, FString FileName, const FString& Section, const FString& Key, const FString& Value, bool& Success, bool bOverWrite)
{
	Directory = FPaths::ProjectConfigDir() + Directory;
	Directory += "/";
	Directory += FileName;
	Directory += ".ini";

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	//文件是否存在
	if (PlatformFile.FileExists(*Directory))
	{
		//是否覆盖写入
		if (!bOverWrite)
		{
			Success = false;
			return;
		}
	}
	Success = FFileHelper::SaveStringToFile(TEXT(""), *Directory);
	GConfig->Flush(false, Directory);
	GConfig->SetString(*Section, *Key, *Value, Directory);
}

//读取JSON文件
void UV_Library::ReadJsonFile(eDirType DirType, FString Directory, FString FileName, FString& JSONString)
{
	if (DirType == eDirType::ProjectDir)
	{
		Directory = FPaths::ProjectDir() + Directory;
	}

	Directory += "/";
	Directory += FileName;
	
	if (FFileHelper::LoadFileToString(JSONString, *Directory))
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(JSONString);
		FJsonSerializer::Deserialize(JsonReader, JsonObject);

	}
}
//写入JSON文件
bool UV_Library::WriteJsonFile(eDirType DirType, FString Directory, FString FileName, FString JsonString,
	bool bOverWrite)
{
	if (DirType == eDirType::ProjectDir)
	{
		Directory = FPaths::ProjectDir() + Directory;
	}
	
	else if (DirType == eDirType::PluginsDir)
	{
		Directory = FPaths::ProjectPluginsDir() + Directory;
	}
	
	Directory += "/";
	Directory += FileName;

	if (!bOverWrite)
	{
		if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*Directory))
		{
			return false;
		}
	}
	return FFileHelper::SaveStringToFile(JsonString, *Directory);
}
//JSON得到字符串
void UV_Library::GetJsonString(FString Descriptor, FString Key, FString& Value, bool& Success)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Descriptor);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		//1.查找StringValue
		if (JsonObject->TryGetStringField(Key, Value))
		{
			Success = true;
			return;
		}
		//2.查找JsonStringValue
		const TSharedPtr<FJsonObject>* JsonObjectPtr;
		if (JsonObject->TryGetObjectField(Key, JsonObjectPtr))
		{
			const TSharedPtr<FJsonObject> JsonObjectRef = JsonObject->GetObjectField(Key);
			TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Value);
			Success = FJsonSerializer::Serialize(JsonObjectRef.ToSharedRef(), Writer);
			return;
		}
	}
}
//JSON异步得到字符串
void UV_Library::AsyncGetJsonString(FString Descriptor, FString Key, FString& Value, bool& Success)
{
	FAsyncTask<AsyncJsonString>* MyTask = new FAsyncTask<AsyncJsonString>( Descriptor,  Key,  Value,  Success);
	MyTask->StartBackgroundTask();
	// MyTask->IsDone()  可以配合定时器检测是否完成任务

	//等待任务完成后，进行手动删除
	MyTask->EnsureCompletion();
	delete MyTask;
	MyTask = nullptr;
}
//JSON添加字符串
void UV_Library::AddJsonString(const FString& Descriptor, FString Key, FString Value, FString& NewDescriptor, bool& Success)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	if (!Descriptor.IsEmpty())
	{
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Descriptor);
		if (!FJsonSerializer::Deserialize(JsonReader, JsonObject) || !JsonObject.IsValid())
		{
			Success = false;
			return;
		}
	}

	TSharedRef<FJsonValueString> JsonValueObject = MakeShareable(new FJsonValueString(Value));
	JsonObject->SetField(Key, JsonValueObject);

	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> JsonWriter = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&NewDescriptor);
	Success = FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);
}
//JSON得到字符串数组
void UV_Library::GetJsonArray(FString Descriptor, FString Key, TArray<FString>& Value, bool& Success)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Descriptor);

	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
	{
		//1.查找StringArrayValue
		if (JsonObject->TryGetStringArrayField(Key, Value))
		{
			Success = true;
			return;
		}

		//2.查找JsonStringArrayValue
		const TArray< TSharedPtr<FJsonValue> >* JsonValueArray;
		if (JsonObject->TryGetArrayField(Key, JsonValueArray))
		{
			for (int i = 0; i < JsonValueArray->Num(); i++)
			{
				FString ValueRef;
				//3.判断如果为数组的数组，则合并为字符串，逗号隔开
				if ((*JsonValueArray)[i]->AsArray().Num() > 0 )
				{
					for (int a = 0; a < (*JsonValueArray)[i]->AsArray().Num(); a++)
					{
						if (a == 0)
						{
							ValueRef = (*JsonValueArray)[i]->AsArray()[a]->AsString();
						}
						else
						{
							ValueRef += "," + (*JsonValueArray)[i]->AsArray()[a]->AsString();
						}
					}
					UE_LOG(LogTemp, Warning, TEXT("ArrayArray"));
				}
				else
				{
					TSharedPtr<FJsonObject> JsonRef = (*JsonValueArray)[i]->AsObject();
					TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ValueRef);
					FJsonSerializer::Serialize(JsonRef.ToSharedRef(), Writer);
					UE_LOG(LogTemp, Warning, TEXT("JsonArray"));
				}
				Value.Add(ValueRef);
			}
			Success = true;
		}
	}
}
//JSON异步得到字符串数组
void UV_Library::AsyncGetJsonArray(FString Descriptor, FString Key, TArray<FString>& Value, bool& Success)
{
	FAsyncTask<AsyncJsonArray>* MyTask = new FAsyncTask<AsyncJsonArray>( Descriptor,  Key,  Value,  Success);
	MyTask->StartBackgroundTask();
	// MyTask->IsDone()  可以配合定时器检测是否完成任务

	//等待任务完成后，进行手动删除
	MyTask->EnsureCompletion();
	delete MyTask;
	MyTask = nullptr;
}

//base64解码图片
void UV_Library::GetBase64Texture(FString Descriptor, UTexture2D*& Texture, bool& Success)
{
	if (!Descriptor.IsEmpty())
	{
		//base64解码为uint8
		TArray<uint8> unit8Data;
		if (FBase64::Decode(*Descriptor, unit8Data))
		{
			IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
			TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(unit8Data.GetData(), unit8Data.Num()))
			{
				TArray<uint8> UncompressedBGRA;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
				{
					//创建图片
					Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
					//图片元数据
					void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					//数据拷贝
					FMemory::Memcpy(TextureData, (&UncompressedBGRA)->GetData(), (&UncompressedBGRA)->Num());
					Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
					Texture->UpdateResource();
					Success = true;
				}

			}
		}
	}
}

//调用函数通过函数名
void UV_Library::CallFuntionByName(AActor* Actor, FString FunctionName)
{
	FName FunFName = FName(*FunctionName);
	UFunction* Function = Actor->FindFunction(FunFName);
	if (Function == NULL)
	{
		return;
	}
	Actor->ProcessEvent(Function, NULL);
}

//-----------------------------------------------------------------------------------------加载资源

void UV_Library::LoadAssetDataFromDirectory(TArray<FAssetData>& AssetDataArray, FString Directory)
{
	if (!Directory.IsEmpty())
	{
		UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(UObject::StaticClass(), false, false);
		ObjectLibrary->AddToRoot();
		ObjectLibrary->LoadAssetDataFromPath(Directory);
		ObjectLibrary->GetAssetDataList(AssetDataArray);
	}
}
//从路径加载object
void UV_Library::LoadObjectFromDirectory(FString Directory, UObject*& Object)
{
	if (!Directory.IsEmpty())
	{
		Object = LoadObject<UObject>(NULL, *Directory);
	}
}

//-----------------------------------------------------------------------------------------异步加载
//Runnable
void UV_Library::CallRunnable(AActor* Actor, FString FunctionName)
{
	auto* Runnable = new V_Runnable(Actor, FunctionName);
	auto* RunnableThread = FRunnableThread::Create(Runnable, *FunctionName);
}
//AsyncTask
void UV_Library::CallAsyncTask(AActor* Actor, FString FunctionName)
{
	//该方式会自动新建一个线程，并且调用V_AsyncTask::DoWork
	auto task = new FAutoDeleteAsyncTask<V_AsyncTask>(Actor, FunctionName);
	if (task)
	{
		task->StartBackgroundTask();
	}
}
//GraphTask
void UV_Library::CallGraphTask(AActor* Actor, FString FunctionName)
{

}

//-----------------------------------------------------------------------------------------GPS坐标转换
void UV_Library::WebMCToWGS84(FVector Location, FVector2D& Coor)
{
	double pi = 3.1415926535897932384626;//圆周率
	float rate = 100;//厘米换算成米

	double x = Location.X / rate / 20037508.34 * 180;
	double y = Location.Y / rate / 20037508.34 * 180;
	y = 180 / pi * (2 * FMath::Atan(FMath::Exp(y * pi / 180)) - pi / 2) * (-1);
	Coor = FVector2D(x, y);
	return;

}

void UV_Library::WGS84ToWebMC(FVector2D Coor, FVector& Location)
{
	double pi = 3.1415926535897932384626;//圆周率
	float rate = 100;//厘米换算成米

	double x = Coor.X * 20037508.34 / 180 * rate;
	double y = FMath::Loge(FMath::Tan((90 + Coor.Y) * pi / 360)) / (pi / 180);
	y = y * 20037508.34 / 180 * rate * (-1);
	Location = FVector(x, y, 0);
	return;

}

void UV_Library::WGS84ToGCJ02(FVector2D Coor, FVector2D& NewCoor)
{
	double lon = Coor.X;
	double lat = Coor.Y;

	//判断是否在国内，不在国内则不做偏移
	if ((lon < 72.004 || lon > 137.8347) && (lat < 0.8293 || lat > 55.8271))
	{
		NewCoor = Coor;
		return;
	}

	double x = lon - 105.0;
	double y = lat - 35.0;

	double pi = 3.1415926535897932384626;//圆周率
	double a = 6378245.0;//卫星椭球坐标投影到平面地图坐标系的投影因子。
	double ee = 0.00669342162296594323;//椭球的偏心率。

	double dLat = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * FMath::Sqrt(FMath::Abs(x));
	dLat += (20.0 * FMath::Sin(6.0 * x * pi) + 20.0 * FMath::Sin(2.0 * x * pi)) * 2.0 / 3.0;
	dLat += (20.0 * FMath::Sin(y * pi) + 40.0 * FMath::Sin(y / 3.0 * pi)) * 2.0 / 3.0;
	dLat += (160.0 * FMath::Sin(y / 12.0 * pi) + 320 * FMath::Sin(y * pi / 30.0)) * 2.0 / 3.0;

	double dLon = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * FMath::Sqrt(FMath::Abs(x));
	dLon += (20.0 * FMath::Sin(6.0 * x * pi) + 20.0 * FMath::Sin(2.0 * x * pi)) * 2.0 / 3.0;
	dLon += (20.0 * FMath::Sin(x * pi) + 40.0 * FMath::Sin(x / 3.0 * pi)) * 2.0 / 3.0;
	dLon += (150.0 * FMath::Sin(x / 12.0 * pi) + 300.0 * FMath::Sin(x / 30.0 * pi)) * 2.0 / 3.0;

	double radLat = lat / 180.0 * pi;
	double magic = FMath::Sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = FMath::Sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
	dLon = (dLon * 180.0) / (a / sqrtMagic * FMath::Cos(radLat) * pi);

	NewCoor = FVector2D(lon + dLon, lat + dLat);
	return;

}

void UV_Library::GCJ02ToWGS84(FVector2D Coor, FVector2D& NewCoor)
{
	double pi = 3.1415926535897932384626;//圆周率
	double a = 6378245.0;//卫星椭球坐标投影到平面地图坐标系的投影因子。
	double ee = 0.00669342162296594323;//椭球的偏心率。

	double x = Coor.X - 105.0;
	double y = Coor.Y - 35.0;

	double dLon = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * FMath::Sqrt(FMath::Abs(x));
	dLon += (20.0 * FMath::Sin(6.0 * x * pi) + 20.0 * FMath::Sin(2.0 * x * pi)) * 2.0 / 3.0;
	dLon += (20.0 * FMath::Sin(x * pi) + 40.0 * FMath::Sin(x / 3.0 * pi)) * 2.0 / 3.0;
	dLon += (150.0 * FMath::Sin(x / 12.0 * pi) + 300.0 * FMath::Sin(x / 30.0 * pi)) * 2.0 / 3.0;

	double dLat = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * FMath::Sqrt(FMath::Abs(x));
	dLat += (20.0 * FMath::Sin(6.0 * x * pi) + 20.0 * FMath::Sin(2.0 * x * pi)) * 2.0 / 3.0;
	dLat += (20.0 * FMath::Sin(y * pi) + 40.0 * FMath::Sin(y / 3.0 * pi)) * 2.0 / 3.0;
	dLat += (160.0 * FMath::Sin(y / 12.0 * pi) + 320 * FMath::Sin(y * pi / 30.0)) * 2.0 / 3.0;

	double radLat = Coor.Y / 180.0 * pi;
	double magic = FMath::Sin(radLat);
	magic = 1 - ee * magic * magic;
	double sqrtMagic = FMath::Sqrt(magic);
	dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
	dLon = (dLon * 180.0) / (a / sqrtMagic * FMath::Cos(radLat) * pi);

	NewCoor = FVector2D(Coor.X - dLon, Coor.Y - dLat);
	return;

}

void UV_Library::GCJ02ToBD09(FVector2D Coor, FVector2D& NewCoor)
{
	double x = Coor.X;
	double y = Coor.Y;

	double xpi = 3.14159265358979324 * 3000.0 / 180.0;

	double z = FMath::Sqrt(x * x + y * y) + 0.00002 * FMath::Sin(y * xpi);
	double theta = FMath::Atan2(y, x) + 0.000003 * FMath::Cos(x * xpi);
	NewCoor.X = z * FMath::Cos(theta) + 0.0065;
	NewCoor.Y = z * FMath::Sin(theta) + 0.006;
	return;

}

void UV_Library::BD09ToGCJ02(FVector2D Coor, FVector2D& NewCoor)
{
	double xpi = 3.14159265358979324 * 3000.0 / 180.0;

	double x = Coor.X - 0.0065;
	double y = Coor.Y - 0.006;
	double z = FMath::Sqrt(x * x + y * y) - 0.00002 * FMath::Sin(y * xpi);
	double theta = FMath::Atan2(y, x) - 0.000003 * FMath::Cos(x * xpi);
	NewCoor.X = z * FMath::Cos(theta);
	NewCoor.Y = z * FMath::Sin(theta);
	return;

}

FString UV_Library::BinaryToString(TArray<uint8> ASCII)
{
	std::string cstr(reinterpret_cast<const char*>(ASCII.GetData()), ASCII.Num());
	return FString(UTF8_TO_TCHAR(cstr.c_str()));
}

void UV_Library::cmd(FString command)
{
	const char* charString = TCHAR_TO_ANSI(*command);
	system(charString);
	UE_LOG( VisualizationLOG, Warning, TEXT( "cmd: %s" ), *command );
}

void UV_Library::GetIP(FString& IP)
{
	IP = "NONE";
	bool canBind = false;
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get()->GetLocalHostAddr(*GLog,canBind);
	if(LocalIp->IsValid())
	{
		IP = LocalIp->ToString(false); //如果想附加端口就写 ture
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("GetIP False!"));
}

void UV_Library::getMAC(FString& MAC)
{
	MAC = FGenericPlatformMisc::GetMacAddressString();
}

void UV_Library::encodeMD5(FString string, FString& MD5)
{
	std::string szValue = TCHAR_TO_UTF8(*string);
	MD5 = FMD5::HashBytes((const uint8*)szValue.c_str(), szValue.length());
}

void UV_Library::encodeMD5File(FString FilePath, FString& MD5)
{
	// 打开文件
	IPlatformFile & PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	IFileHandle* pFileHandle = PlatformFile.OpenRead(*FilePath);
	if (pFileHandle)
	{
		// 信息变量声明
		int64 nFlieSize = pFileHandle->Size();
		uint8* pFileInfo = new uint8[nFlieSize];

		// 读取文件
		if (pFileHandle->Read(pFileInfo, nFlieSize))
		{
			MD5 = FMD5::HashBytes(pFileInfo, nFlieSize);
		}

		// 释放内存
		delete[] pFileInfo;
	}
}

void  UV_Library::GetArea(TArray<FVector> Points, float& M2)
{
	int pointNum = Points.Num();
	if(pointNum > 3)
	{
		double square = 0;
		for (int i = 0; i < pointNum; ++i)
		{
			square += Points[i].X * Points[(i + 1) % pointNum].Y - Points[i].Y * Points[(i + 1) % pointNum].X;
		}
		M2 = abs(square/2);
		M2 = M2/10000;//厘米转
	}
}


