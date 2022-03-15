#pragma once

#include "MySQLConnection.h"
#include "MySQLConnectorStructs.h"
#include "Engine.h"
#include "MySQLDatabase.generated.h"

USTRUCT(BlueprintType)
struct FMySQLConnectorKeyValuePair
{
	GENERATED_BODY()

		/** The database table field name */

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString Key;

	/** The value of the field */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		FString Value;
};

USTRUCT(BlueprintType)
struct FMySQLConnectorQueryResultRow
{
	GENERATED_BODY()

		/** A list of field name, field value pairs */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		TArray<FMySQLConnectorKeyValuePair> Fields;
};

USTRUCT(BlueprintType)
struct FMySQLConnectoreQueryResult
{
	GENERATED_BODY()

		/** The resulting rows from the query */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySQL, meta = (Keywords = MySQL))
		TArray<FMySQLConnectorQueryResultRow> ResultRows;

	/** Was the query successful or not */
	UPROPERTY(BlueprintReadOnly, Category = MySQL, meta = (Keywords = thread))
		bool Success;

	/** If the query was unsuccessful a human readable error message will be populated here */
	UPROPERTY(BlueprintReadOnly, Category = MySQL, meta = (Keywords = thread))
		FString ErrorMessage;

};


// A few things for internal use here.
namespace MySQLConnectorResultValueTypes
{
	enum MySQLConnectorResultValType
	{
		Int,
		Varchar,
		UnsupportedValueType
	};
}


// Result field, used as an intermediary when collecting results from
// an MySQL query.
struct MySQLConnectorResultField
{
	FString StringValue;
	double DoubleValue;
	int64 IntValue;

	FString Name;
	MySQLConnectorResultValueTypes::MySQLConnectorResultValType Type;

	FString ToString()
	{
		if (Type == MySQLConnectorResultValueTypes::Varchar)
			return StringValue;
		if (Type == MySQLConnectorResultValueTypes::Int)
			return FString::Printf(TEXT("%i"), IntValue);
		return StringValue;
	}
};

struct MySQLConnectorResultValue
{
	TArray<MySQLConnectorResultField> Fields;
};

// The internal result object.
struct MySQLConnectorQueryResult
{
	bool Success;
	FString ErrorMessage;
	TArray<MySQLConnectorResultValue> Results;
};






/**
* MySQLConnector main database class.
*/
UCLASS()
class UMySQLDatabase : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static UMySQLConnection* Connection(FString Host, FString UserName, FString UserPassword, FString DatabaseName);


	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static bool ExecuteQuery(FString Query, UMySQLConnection* Connection);

	/** Drop Table*/
	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static bool DropTable(const FString TableName, UMySQLConnection* Connection);

	/** Truncate Table*/
	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static bool TruncateTable(const FString TableName, UMySQLConnection* Connection);

	/** Create table in the database. */
	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static FMySQLConnectorTable CreateTable(const FString TableName,
			const TArray<FMySQLConnectorTableField> Fields, UMySQLConnection* Connection);

	UFUNCTION(BlueprintPure, Category = MySQL, meta = (Keywords = MySQL))
		static FMySQLConnectorTableField INT(FString FieldName, const bool PK, const bool AI);

	UFUNCTION(BlueprintPure, Category = MySQL, meta = (Keywords = MySQL))
		static FMySQLConnectorTableField VARCHAR(const FString FieldName, const FString FieldLength,
			const bool PK, const bool Unique, const bool NotNull);

	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static bool InsertTest(const FString Query, UMySQLConnection* Connection);

	/** returns the address or empty String on error, returns address as Integer or -1 on error */
	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static bool GetControllerIP(FString& IP, int32& IP_int, APlayerController* PlayerController);

	/** Get data from the database using a select statement and return the rows. */
	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static FMySQLConnectoreQueryResult GetData(const FString& Query, UMySQLConnection* Connection);

private:
	/** Runs a query and returns fetched rows. */
	static MySQLConnectorQueryResult RunQuery(FString Query, UMySQLConnection* Connection);




};
