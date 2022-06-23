// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "V_Library.generated.h"

//自定义LOG
DECLARE_LOG_CATEGORY_EXTERN( VisualizationLOG, Log, All ); 

UENUM(BlueprintType)
enum class eResult : uint8
{
	Success UMETA(DisplayName = "True"),
	Fail	UMETA(DisplayName = "False")
};

UENUM(BlueprintType)
enum eLogType
{
	Warning,
	Error,
	Log
};

UENUM(BlueprintType)
enum eDirType
{
	ProjectDir,
	PluginsDir,
	AbsoluteDir
};

UCLASS()
class UV_Library : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	
	//是否开发
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = editor, CompactNodeTitle = Edit))
		static bool Editor();
	//是否开发
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = editor, ExpandEnumAsExecs = "Result"))
		static void IsEditor(eResult& Result);

	//打印到屏幕（仅编辑）和日志
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = print, AdvancedDisplay = "2"))
		static void PrintTo(FString String = TEXT("Visualization"), float Duration = 10.0f, bool bScreen = true, bool bLog = true,
			float Scale = 1.1f, FLinearColor Color = FLinearColor(0, 1, 1, 1), 
			eLogType Type = eLogType::Warning);
	//打印到Log
		static void PrintToLog(FString String, eLogType Type);
	
	//读取file
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = file))
		static TArray<FString> ReadFile(eDirType DirType, FString Directory, FString FileName);
	//写入file
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = file))
		static bool WriteFile(eDirType DirType, TArray<FString> TextArray, FString Directory, FString FileName, bool bOverWrite = false);

	//读取config
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = config))
		static void ReadConfig(FString Directory, FString FileName, const FString& Section, const FString& Key, FString& Value, bool bAbsDir = true);
	//写入config
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = config))
		static void WriteConfig(FString Directory, FString FileName, const FString& Section, const FString& Key, const FString& Value, bool& Success, bool bOverWrite = false);

	//读取JSON文件
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = json))
		static void ReadJsonFile(eDirType DirType, FString Directory, FString FileName, FString& JSONString);
	//写入JSON文件
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = json))
		static bool WriteJsonFile(eDirType DirType, FString Directory, FString FileName, FString JsonString, bool bOverWrite = false);
	//JSON得到字符串
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = json))
		static void GetJsonString(FString Descriptor, FString Key, FString& Value, bool& Success);
	//JSON异步得到字符串
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = json))
		static void AsyncGetJsonString(FString Descriptor, FString Key, FString& Value, bool& Success);
	//JSON添加字符串
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = json))
		static void AddJsonString(const FString& Descriptor, FString Key, FString Value, FString& NewDescriptor, bool& Success);
	//JSON得到字符串数组
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = json))
		static void GetJsonArray(FString Descriptor, FString Key, TArray<FString>& Value, bool& Success);
	//JSON异步得到字符串数组
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = json))
		static void AsyncGetJsonArray(FString Descriptor, FString Key, TArray<FString>& Value, bool& Success);

	//base64解码图片
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = base64))
		static void GetBase64Texture(FString Descriptor, UTexture2D*& Texture, bool& Success);

	//调用函数通过函数名
		static void CallFuntionByName(AActor* Actor, FString FunctionName);

	//从路径加载资源
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = asset))
		static void LoadAssetDataFromDirectory(TArray<FAssetData>& AssetDataArray, FString Directory = "/Game");
	//从路径加载object
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = asset))
		static void LoadObjectFromDirectory(FString Directory, UObject*& Object);
	

	//Runnable
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = thread))
		static void CallRunnable(AActor* Actor, FString FunctionName);
	//AsyncTask
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = thread))
		static void CallAsyncTask(AActor* Actor, FString FunctionName);
	//GraphTask
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = thread))
		static void CallGraphTask(AActor* Actor, FString FunctionName);

	//经纬度坐标转换:墨卡托转WGS84
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = gps))
		static void WebMCToWGS84(FVector Location, FVector2D& Coor);
	//经纬度坐标转换:WGS84转墨卡托
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = gps))
		static void WGS84ToWebMC(FVector2D Coor, FVector& Location);
	//经纬度坐标转换:WGS84转火星坐标
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = gps))
		static void WGS84ToGCJ02(FVector2D Coor, FVector2D& NewCoor);
	//经纬度坐标转换:火星坐标转WGS84
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = gps))
		static void GCJ02ToWGS84(FVector2D Coor, FVector2D& NewCoor);
	//经纬度坐标转换:火星坐标转百度坐标
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = gps))
		static void GCJ02ToBD09(FVector2D Coor, FVector2D& NewCoor);
	//经纬度坐标转换:百度坐标转火星坐标
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = gps))
		static void BD09ToGCJ02(FVector2D Coor, FVector2D& NewCoor);

	//Binary转字符串
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = Binary))
		static FString BinaryToString(TArray<uint8> ASCII);

	//cmd
	UFUNCTION(BlueprintCallable, Exec)
		static void cmd(FString command);
	//本机IP
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = IP))
		static void GetIP(FString& IP);
	//本机MAC
    UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = mac))
		static void getMAC(FString& MAC);

	//MD5加密
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = md5))
		static void encodeMD5(FString string, FString& MD5);
	//MD5加密文件
	UFUNCTION(BlueprintPure, Category = V_Library, meta = (Keywords = md5))
		static void encodeMD5File(FString FilePath, FString& MD5);

	//测量面积
	UFUNCTION(BlueprintCallable, Category = V_Library, meta = (Keywords = area))
		static void GetArea(TArray<FVector> Points, float& M2);
	
};


