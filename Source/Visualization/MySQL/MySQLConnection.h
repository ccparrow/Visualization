#pragma once

#include "mysql.h"
#include "MySQLConnection.generated.h"

/**
* SQLite main database class.
*/
UCLASS()
class UMySQLConnection : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	MYSQL* globalCon = nullptr;

	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		bool CheckConnection();

	// checks if the connection is valid and closes it, resets the pointers to nullptr on success
	// true on success, false in case the connection is not established or the argument is nullptr
	UFUNCTION(BlueprintCallable, Category = MySQL, meta = (Keywords = MySQL))
		static bool CloseConnection(UMySQLConnection* Connection);
};
