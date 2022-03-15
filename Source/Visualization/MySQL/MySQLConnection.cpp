#include "MySQLConnection.h"


UMySQLConnection::UMySQLConnection(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UMySQLConnection::CheckConnection()
{
	if (globalCon)
	{
		return mysql_ping(globalCon) == 0;
	}
	return false;
}

bool UMySQLConnection::CloseConnection(UMySQLConnection* Connection)
{
	if (Connection) {
		if (Connection->CheckConnection())
		{
			mysql_close(Connection->globalCon);
			Connection->globalCon = nullptr;
			mysql_library_end();
			return true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("MySQLCloseConnection: Connection is valid but Server does no respond!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("MySQLCloseConnection: Connection is null!"));
	}
	return false;
}
