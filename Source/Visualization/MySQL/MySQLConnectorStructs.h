#pragma once

#include "MySQLConnectorStructs.generated.h"

USTRUCT(BlueprintType)
struct FMySQLConnectorTableField
{
	GENERATED_USTRUCT_BODY()

		/** String with piece if SQL script*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString ResultStr = "";

	/** Field name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString FieldName = "";

	/** Field type*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString FieldType = "";

	/** Field value*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString FieldValue = "";

};

USTRUCT(BlueprintType)
struct FMySQLConnectorPrimaryKey
{
	GENERATED_USTRUCT_BODY()

		/** String with piece if SQL script*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString ResultStr = "";
};

USTRUCT(BlueprintType)
struct FMySQLConnectorTable
{
	GENERATED_USTRUCT_BODY()

		/** Database name*/
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString DatabaseName = "";

	/** Table name*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString TableName = "";

	/** Array with Fields*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		TArray<FMySQLConnectorTableField> Fields;

	///** Primary Key */
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
	//	FMySQLConnectorPrimaryKey PK;

	/** Created Key */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		bool Created = false;

};
