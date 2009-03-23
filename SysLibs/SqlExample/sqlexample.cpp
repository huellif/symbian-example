// sqlexample.cpp
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//
/** 
@file
This example program demonstrates the use of SQL APIs. 
The code demonstrates how to create non secure and secure databases on 
the writable drive and perform basic operations on the databases.
*/
#include "sqlexample.h"
#include <e32cons.h>
#include <SqlDb.h>

_LIT(KTitle, "SQL example");
_LIT(KTextPressAKey, "\n\nPress any key to step through the example\n");
_LIT(KExit,"Press any key to exit the application ");
_LIT(KPressAKey,"Press any key to continue \n");
_LIT(KNonSecure,"\nCreating a non secure database \n");
_LIT(KSecure,"\nCreating a secure database \n");
_LIT(KOpen,"Opening  the secure database \n");
_LIT(KDelete,"Deleting the database(s)\n");
_LIT(KClose,"Closing the database(s)\n");
_LIT(KCopyNonSec,"\nCopying a non secure database to another non secure one \n");
_LIT(KCopySecure,"\nCopying a secure database to another secure database \n");                                    
_LIT(KAttach,"\nOpen a secure database and attach another secure database\n");
_LIT(KCreateTable,"\nCreating a table\n");
_LIT(KInsert,"Inserting records into the table\n");
_LIT(KPrepare,"Preparing a query\n");
_LIT(KExecute,"Executing a query\n");

// Names of the databases created, operated upon and later deleted.
_LIT(KDbName, "\\Example_db.db");
_LIT(KAnotherDbName, "\\Example_Anotherdb.db");
_LIT(KSecureDb1, "[E80000AF]db1.db");
_LIT(KSecureDb2, "[E80000AF]db2.db");
_LIT(KDatabase, "\\Sqlscalarfullselect.db");

// Security policies used
const TSecurityPolicy KPolicy1(ECapabilityReadUserData, ECapabilityNetworkControl,  ECapabilityWriteUserData);
const TSecurityPolicy KPolicy2(ECapabilityReadUserData);

/**
Allocates and constructs a CSqlExample object and 
leaves it on the cleanup stack.
Initialises all member data to their default values.
*/	
CSqlExample* CSqlExample::NewLC()
	{
	CSqlExample* rep = new(ELeave) CSqlExample();
	CleanupStack::PushL(rep);
	rep->ConstructL();
	return rep;
	}
	
/**
Constructor
*/
CSqlExample::CSqlExample()
	{
	}	

void CSqlExample::ConstructL()
	{
	iConsole = Console::NewL(KTitle,TSize(KConsFullScreen,KConsFullScreen));
	iConsole->Printf ( KTextPressAKey );
	iConsole->Getch ();
	}

/**
Destructor
*/
CSqlExample::~CSqlExample()
	{
	iConsole->Printf(KExit);
	iConsole->Getch();
	
	delete iConsole;
	}
	
/**
Creates a non secure database, which has a table with two integers, and closes it. 
It is deleted at a later point of time after the copy operation.
@leave KErrNotFound, KErrAbort, KErrPermissionDenied,
KErrArgument, system-wide error codes.
*/
void CSqlExample::CreateNonSecureDBL()
	{
	TBuf<200> buffer;
	RSqlDatabase db;
		
	// Create non-secure database
	iConsole->Printf(KNonSecure);
	TInt error;
	TRAP(error,db.Create(KDbName););

	TBuf<100> sql;
	_LIT(KTable,"CREATE TABLE A1(F1 INTEGER, F2 INTEGER)");
	sql.Copy(KTable);
	User::LeaveIfError(error = db.Exec(sql));

	db.Close();
	iConsole->Printf(KClose);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	}
	
/**
Creates a secure database with a table and a certain security policy. 
Inserts a record, opens it and closes the database. 
The database is reopened later and hence not deleted here. 
@leave KErrNone, KErrNoMemory, KErrBadName, KErrNotReady, KErrInUse,
KErrNotFound, KErrGeneral, KErrPermissionDenied, KErrNotSupported,
ESqlDbError, system-wide error codes 
*/	
void CSqlExample::CreateAndOpenSecureDBL()
	{
	RSqlDatabase db;
	RSqlSecurityPolicy securityPolicy;
	
	User::LeaveIfError(securityPolicy.Create(TSecurityPolicy(TSecurityPolicy::EAlwaysPass)));
	User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, KPolicy1));
		
	iConsole->Printf(KSecure);
	User::LeaveIfError(db.Create(KSecureDb1, securityPolicy));
		
	securityPolicy.Close();	
	
	// Check that the database security policy matches the policy used 
	//when the database was created.
	User::LeaveIfError(db.GetSecurityPolicy(securityPolicy));
	
	_LIT(KSecureCreate,"CREATE TABLE secure(int_fld integer, null_int_fld integer default null)");	
	User::LeaveIfError(db.Exec(KSecureCreate));

	// Attempt to write to the secure database
	_LIT(KSecureInsert,"INSERT INTO secure(int_fld) values(200)");
	User::LeaveIfError(db.Exec(KSecureInsert));
	db.Close();
	
	iConsole->Printf(KOpen);
	// Open the secure database	
	User::LeaveIfError(db.Open(KSecureDb1));
	
	db.Close();
	iConsole->Printf(KClose);
	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	securityPolicy.Close();

	}
	
/**
Essentially a file copy to allow users to copy a database from one file to another. 
SQL provides a mechanism to copy secure databases, (which live in the SQL 
private datacage) as a user is not allowed to do that.
The source database is then deleted after copying it to the destination.
@leave KErrAbort, KErrPermissionDenied, KErrArgument, system-wide error codes. 
*/
void CSqlExample::CopyDatabaseL()
	{
	RSqlDatabase db;	
	
	// Copy non-secure to non-secure database
	iConsole->Printf(KCopyNonSec);	
	User::LeaveIfError(RSqlDatabase::Copy(KDbName, KAnotherDbName));
	
	// Delete the source database
	iConsole->Printf(KDelete);
	User::LeaveIfError(RSqlDatabase::Delete(KAnotherDbName));

	// Create another secure database with a different security policy,
	// KSecureDb1 is already created with KPolicy1.
	RSqlSecurityPolicy securityPolicy;
	User::LeaveIfError(securityPolicy.Create(TSecurityPolicy(TSecurityPolicy::EAlwaysPass)));
	
	User::LeaveIfError(securityPolicy.SetDbPolicy(RSqlSecurityPolicy::EWritePolicy, KPolicy2));
		
	User::LeaveIfError(db.Create(KSecureDb2, securityPolicy));
	
	User::LeaveIfError(db.GetSecurityPolicy(securityPolicy));
	db.Close();
	
	// Copy secure to secure database. The application is the database owner.
	iConsole->Printf(KCopySecure);
	User::LeaveIfError(RSqlDatabase::Copy(KSecureDb1, KSecureDb2));
	
	// Delete the source database
	iConsole->Printf(KDelete);
	User::LeaveIfError(RSqlDatabase::Delete(KSecureDb1));

	iConsole->Printf(KPressAKey);
	iConsole->Getch();
	securityPolicy.Close();	
	db.Close();

	}
	
/**
Opens a secure database and attaches a non secure database to it.
Database has to be open before an attach can be executed.
@leave KErrNoMemory, KErrBadName, KErrNotReady, KErrInUse,
KErrNotFound, KErrGeneral, KErrPermissionDenied, KErrNotSupported,
ESqlDbError, system-wide error codes 
*/
void CSqlExample::AttachDatabasesL()
	{
	RSqlDatabase db;
	RSqlSecurityPolicy securityPolicy;

	_LIT(KAttachDb2, "Db2");
	
	User::LeaveIfError(db.Open(KSecureDb2));
	iConsole->Printf(KAttach);
	
	User::LeaveIfError(db.Attach(KDbName, KAttachDb2));
	
	// Attempt to write to the attached non secure database
	_LIT(KTabInsert,"INSERT INTO db2.a1(f1) valUES(10)");
	User::LeaveIfError(db.Exec(KTabInsert));
			
	// Attempt to read from the attached non secure database
	_LIT(KSelect,"SELECT * FROM db2.a1");
	User::LeaveIfError(db.Exec(KSelect));
		
	// Attempt to write to the main secure database
	_LIT(KAttachInsert,"INSERT INTO a1(f1) valUES(10)");
	User::LeaveIfError(db.Exec(KAttachInsert));
		
	db.Close();
	iConsole->Printf(KDelete);
	User::LeaveIfError(RSqlDatabase::Delete(KDbName));

	User::LeaveIfError(RSqlDatabase::Delete(KSecureDb2));

	securityPolicy.Close();
	
	}
		
/**
Prepares and executes both a simple query and a query with parameters.
@leave KErrNoMemory, KErrBadName, KErrNotReady, KErrInUse,
KErrNotFound, KErrGeneral, KErrPermissionDenied, KErrNotSupported,
ESqlDbError, system-wide error codes.
*/
void CSqlExample:: DataTypesQueryL()
	{
	RSqlDatabase db;
	iConsole->Printf(KSecure);
	
	User::LeaveIfError(db.Create(KDbName));

	// Create a table with different numeric field types
	iConsole->Printf(KCreateTable);
	iConsole->Printf(KExecute);
	
	_LIT(KSql1, "CREATE TABLE Tbl(A INTEGER, B SMALLINT, C REAL, D DOUBLE PRECISION, E FLOAT, \
					                    F DECIMAL)");				                    
	User::LeaveIfError(db.Exec(KSql1));
	
	// Insert one record in to the created table
	iConsole->Printf(KInsert);                                 
	iConsole->Printf(KExecute);
	
	_LIT(KSql2, "INSERT INTO Tbl(A,B,C,D,E,F) VALUES(2000000000, 30000, 123.45, 0.912E+55,\
	                                    1.34E-14, 1234.5678)");
	User::LeaveIfError(db.Exec(KSql2));
	
	// Get the inserted record data
	RSqlStatement stmt;
	iConsole->Printf(KPrepare);
	
	_LIT(KPrepQuery,"SELECT * FROM Tbl");
	User::LeaveIfError(stmt.Prepare(db, KPrepQuery));
	
	User::LeaveIfError(stmt.Next());
	stmt.Close();
	
	// The statement object has to be closed before db.Exec() call, 
	// otherwise the reported error is "database table is locked"
	// Insert second record in to the created table but inverse the column types.
	iConsole->Printf(KExecute);
	_LIT(KSql3, "INSERT INTO Tbl(A,   B,  C, D, E, F) VALUES(\
										-2.5,1.1,12,23,45,111)");									
	User::LeaveIfError(db.Exec(KSql3)); 
	
	// Get the inserted record data
	_LIT(KPrepQuery2,"SELECT * FROM Tbl");
	User::LeaveIfError(stmt.Prepare(db, KPrepQuery2));

	User::LeaveIfError(stmt.Next());
	
	stmt.Close();
	
	// Insert third record in to the created table
	_LIT(KSql4, "INSERT INTO Tbl(A,B,C,D,E,F) VALUES(\
	                                    2,3,123.45,1.5,2.5,1.56)");
	User::LeaveIfError(db.Exec(KSql4));
	
	stmt.Close();
	db.Close();
	User::LeaveIfError(RSqlDatabase::Delete(KDbName));
	}
	

/**
1) Prepares and executes a query with a large parameter, writing that parameter using streaming (RParamWriteStream)
2) Creates a database with a table containing integer, 64-bit
integer, float, text and Blob fields.
3) Inserts two records. 
4) Implements TSqlScalarFullSelectQuery functions for 64 bit integer and text fields and 
checks the returned value.
@leave KErrNoMemory, KErrBadName, KErrNotReady, KErrInUse,
KErrNotFound, KErrGeneral, KErrPermissionDenied, KErrNotSupported,
ESqlDbError, system-wide error codes 
*/
void CSqlExample::ScalarFullSelectL()
	{
	RSqlDatabase db;
	//Create  database.
	User::LeaveIfError(db.Create(KDatabase));
	
	_LIT(KTabCreateA,"CREATE TABLE A(F1 INTEGER, F2 INTEGER, F3 FLOAT, F4 TEXT, F5 BLOB)");
	User::LeaveIfError(db.Exec(KTabCreateA));
	
	_LIT(KTabInsert1A,"INSERT INTO A(F1, F2, F3, F4, F5) VALUES(1, 10000000000, 2.54, 'NAME1234567890', NULL)");
	User::LeaveIfError(db.Exec(KTabInsert1A));
	
	_LIT(KTabInsert2A,"INSERT INTO A(F1, F2, F3, F4) VALUES(2, 200, -1.11, 'ADDRESS')");	
	User::LeaveIfError(db.Exec(KTabInsert2A));
	
	RSqlStatement stmt;
	CleanupClosePushL(stmt);
	
	_LIT(KUpdate,"UPDATE A SET F5=:P WHERE F1 = 2");
	User::LeaveIfError(stmt.Prepare(db,KUpdate));

	// Open the parameter stream
	RSqlParamWriteStream strm;
	CleanupClosePushL(strm);
	
	// Prepare and set the parameter value (non-NULL value)
	User::LeaveIfError(strm.BindBinary(stmt, 0));

	for(TInt i=0;i<100;++i)
		{
		strm << static_cast <TUint8> (i);	
		}
	// Write the buffered data into stream
	strm.CommitL();
	// Execute the prepared SQL statement	
	User::LeaveIfError(stmt.Exec());	
		
	CleanupStack::PopAndDestroy(&strm);
	CleanupStack::PopAndDestroy(&stmt);
	
	TSqlScalarFullSelectQuery fullSelectQuery(db);

	TBuf<100> sql;
	
	// Query with F2 column (64-bit integer column)
	_LIT(KAnotherSql, "SELECT F2 FROM A WHERE F1 = 1");
	sql.Copy(KAnotherSql);
	// Read F2 as integer.
	TInt valInt = fullSelectQuery.SelectIntL(sql);
		
	// Read F2 as 64-bit integer. Expected value: 10000000000
	TInt64	valInt64 = fullSelectQuery.SelectInt64L(sql);
	
	// Read F2 as real. Expected value: 10000000000.0
	TReal valReal = fullSelectQuery.SelectRealL(sql);
	
	// Read F2 as text. Expected value: zero-length 16-bit descriptor.
	TBuf<10> valText;
	TInt err = fullSelectQuery.SelectTextL(sql, valText);

	// Read F2 as binary. Expected value: zero-length 8-bit descriptor.
	TBuf8<10> valBinary;
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);
	
	// Query with F4 column (text column) 
	_LIT(KSql4, "SELECT F4 FROM A WHERE F1 = 1");
	sql.Copy(KSql4);
	// Read F4 as integer. Expected value: 0.
	valInt = fullSelectQuery.SelectIntL(sql);
	
	//Read F4 as 64-bit integer. Expected value: 0.
	valInt64 = fullSelectQuery.SelectInt64L(sql);
	
	// Read F4 as real. Expected value: 0.0.
	valReal = fullSelectQuery.SelectRealL(sql);
	
	// Read F4 as text. Small buffer. Expected return code: positive value, which is the column length in characters.
	err = fullSelectQuery.SelectTextL(sql, valText);
	
	// Read F4 as text. Big enough buffer. 
	TBuf<32> valText2;
	err = fullSelectQuery.SelectTextL(sql, valText2);
	
	// Read F4 as binary. Expected error: KErrNone. Zero-length 8-bit descriptor.
	err = fullSelectQuery.SelectBinaryL(sql, valBinary);

	// Read F5 as binary. Big enough buffer.
	TBuf8<100> valBinary2;
	err = fullSelectQuery.SelectBinaryL(sql, valBinary2);
	
	// Text column value, small buffer, reallocation 
	HBufC* hbuf = HBufC::NewLC(10);
	TPtr name = hbuf->Des();
	sql.Copy(KSql4);
	err = fullSelectQuery.SelectTextL(sql, name);
	
	if(err > 0)
		{
		hbuf = hbuf->ReAllocL(err);
		CleanupStack::Pop();	
		CleanupStack::PushL(hbuf);
		name.Set(hbuf->Des());
		err = fullSelectQuery.SelectTextL(sql, name);
	
		}
	CleanupStack::PopAndDestroy(); // hbuf, can't be put as parameter, because may be reallocated
	//Close database, delete database file.
	db.Close();
	User::LeaveIfError(RSqlDatabase::Delete(KDatabase));
	
	}
	
/**
Prepare and execute a query which returns data, and 
read that data using streaming (RColumnReadStream)
@leave KErrNoMemory, KErrBadName, KErrNotReady, KErrInUse,
KErrNotFound, KErrGeneral, KErrPermissionDenied, KErrNotSupported,
ESqlDbError, system-wide error codes.
*/	
void CSqlExample::ColumnBinaryStreamL()
	{
	RSqlDatabase db;
	TInt error;
	User::LeaveIfError(db.Create(KDbName));

	// Create a table
	_LIT(KSqlStmt1, "CREATE TABLE A(Fld1 INTEGER, Fld2 BLOB);");
	TBuf<100> sqlBuf;
	sqlBuf.Copy(KSqlStmt1);
	
	User::LeaveIfError(error = db.Exec(sqlBuf));
	const TInt KDataLen = 100;

	_LIT(KSqlStmt2, "INSERT INTO A(Fld1, Fld2) VALUES(");
	
	// Allocate a buffer for the SQL statement.
	HBufC8* buf = HBufC8::New(KSqlStmt2().Length() + KDataLen * 2 + 10);
	
	TPtr8 sql = buf->Des();
	
	// Insert row 1

	_LIT(KHexValStr1, "7E");
	sql.Copy(KSqlStmt2);
	
	_LIT(KString1, "1, x'");
	sql.Append(KString1);
	TInt i;
	for(i=0;i<KDataLen;++i)
		{
		sql.Append(KHexValStr1);
		}
	_LIT(KString2, "')");	
	sql.Append(KString2);

	User::LeaveIfError(db.Exec(sql));

	// Insert row 2
	
	_LIT(KHexValStr2, "A3");
	sql.Copy(KSqlStmt2);
	_LIT(KString3, "2, x'");
	sql.Append(KString3);
	for(i=0;i<KDataLen;++i)
		{
		sql.Append(KHexValStr2);
		}
	_LIT(KString4, "')");
	sql.Append(KString4);

	User::LeaveIfError(db.Exec(sql));
	
	// Prepare SELECT SQL statement
	_LIT(KSqlStmt3, "SELECT * FROM A");
	sqlBuf.Copy(KSqlStmt3);
	
	RSqlStatement stmt;
	User::LeaveIfError(error = stmt.Prepare(db, sqlBuf));
	
	// Move on row 1
	User::LeaveIfError(error = stmt.Next());

	// Read the long binary column using a stream
	RSqlColumnReadStream columnStream;
	error = columnStream.ColumnBinary(stmt, 1);
	
	
	TInt size = stmt.ColumnSize(1);
	TPtr8 colData = buf->Des();
	TRAP(error, columnStream.ReadL(colData, size));
	columnStream.Close();
	
	User::LeaveIfError(stmt.Next());
		
	//Read row 2 using ColumnBinary(TInt aColumnIndex, TDes8& aDest).
	error = stmt.ColumnBinary(1, colData);

	//Read row 2 using a stream
	colData.Zero();
	error = columnStream.ColumnBinary(stmt, 1);
	
	size = stmt.ColumnSize(1);
	TRAP(error, columnStream.ReadL(colData, size));
	columnStream.Close();
	
	stmt.Close();
		
	delete buf; 
	buf = NULL;
	
	db.Close();

	error = RSqlDatabase::Delete(KDbName);
	
	}

				
LOCAL_C void MainL()
	{
	// Create an Active Scheduler to handle asychronous calls
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	CSqlExample* app = CSqlExample::NewLC();
	
	// Create a non secure database
	app->CreateNonSecureDBL();
	
	// Create and open a secure database
	app->CreateAndOpenSecureDBL();
	
	// Copy two databases
	app->CopyDatabaseL();
	
	// Attach two databases
	app->AttachDatabasesL();
	
	// Simple query and query with paramaters
	app->DataTypesQueryL();
	
	//Prepares and executes a query with a large parameter
	// writing that parameter using streaming (RParamWriteStream)
	app->ScalarFullSelectL();
	
	// Prepare and execute a query which returns data,
	// and read that data using streaming (RColumnReadStream)
	app->ColumnBinaryStreamL();
		
	CleanupStack::PopAndDestroy(2); //app, scheduler
	
	}

GLDEF_C TInt E32Main()
	{
    __UHEAP_MARK;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if(cleanup == NULL)
    	{
    	return KErrNoMemory;
    	}
    TRAPD(err, MainL());
	if(err != KErrNone)
		{
		User::Panic(_L("Failed to complete"),err);
		}

    delete cleanup;
    __UHEAP_MARKEND;
    return KErrNone;
	}
