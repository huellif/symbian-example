// sqlexample.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

/**
@file
Contains the CSqlExample class.
*/
#ifndef __SQLEXAMPLE_H__
#define __SQLEXAMPLE_H__

#include <e32base.h> 
/**
Demonstrates some uses of Symbian OS SQL component.
 
The class demonstrates how to 
- Create and open secure and non secure databases	
- Copy a database 
- Attach two databases together
- Execute a simple query (RSqlDatabase::Exec)
- Prepare and execute a query with parameters (RSqlStatement)
- Prepare and execute a query with a large parameter, writing that parameter using streaming (RParamWriteStream)
- Prepare and execute a query which returns data, and read that data
- Prepare and execute a query which returns data, and read that data using streaming (RColumnReadStream) 
- Query for a single value (TSqlScalarFullSelectQuery)
*/
class CSqlExample: public CBase
	{
public:
	static CSqlExample* NewLC();

	~CSqlExample();
	
	void CreateNonSecureDBL();
	void CreateAndOpenSecureDBL();
	void CopyDatabaseL();
	void AttachDatabasesL();
	void DataTypesQueryL();
	void ScalarFullSelectL();
	void ColumnBinaryStreamL();
	void DeleteL();
	
private:
	CSqlExample();
	void ConstructL();
private:
	/** Pointer to the console interface */
	CConsoleBase* 		iConsole;
    };

#endif //__SQLEXAMPLE_H__
