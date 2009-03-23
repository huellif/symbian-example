// Attributes.cpp
//
// Copyright (C) Symbian Software Ltd 2000-2005.  All rights reserved.

// This code creates several files and directories inside
// the private directory on drive C:
// i.e. the directory "C:\private\0FFFFF03."
//
// Note that no special capabilities are needed provided all file
// operations are directed at files that lie within
// the private directory. This is the case with this example.
//
// Before the program terminates,
// all files and directories created will be deleted.

#include <f32file.h>
#include "CommonFramework.h"		

LOCAL_D RFs fsSession;

// example functions
void DoDirectoryAttribsL();
void PrintDirectoryLists();	
void DeleteAll();

// utility functions
void FormatEntry(TDes& aBuffer, const TEntry& aEntry);
void FormatAtt(TDes& aBuffer, const TUint aValue);
void MakeSmallFile(const TDesC& aFileName);

void WaitForKey()
	{
	_LIT(KMessage,"Press any key to continue\n");
	console->Printf(KMessage);
	console->Getch();
	}

LOCAL_C void doExampleL()
    {
	  // connect to file server
	User::LeaveIfError(fsSession.Connect());
	
      // create the private directory
	  // on drive C:
	  // i.e. "C:\private\0FFFFF03\"
	  // Note that the number 0FFFFF03 is the 
      // process security id taken from the 2nd UID
      // specified in the mmp file.
    fsSession.CreatePrivatePath(EDriveC);
    
      // Set the session path to
      // this private directory on drive C:
    fsSession.SetSessionToPrivate(EDriveC);

      
	DoDirectoryAttribsL();
	WaitForKey();
	PrintDirectoryLists();
	WaitForKey();
	DeleteAll();
	
	  // close session with file server
	fsSession.Close(); 
	}

void DoDirectoryAttribsL()
	{
	    // Define text to be used for display at the console.
	_LIT(KAttsMsg,"\nAttributes and entry details\n");
	_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	_LIT(KDateMsg,"Using Entry():\nModification time of %S is %S\n");
	_LIT(KSizeMsg,"Size = %d bytes\n");
	_LIT(KBuffer,"%S");
	_LIT(KEntryMsg,"Using Modified():\nModification time of %S is %S\n");
	_LIT(KAttMsg,"Using Att():\n%S");

      // Define subdirectory name and the file name to be used.
    _LIT(KSubDirName,"f32examp\\");
	_LIT(KFileName,"tmpfile.txt");

	  // Create a file. 
	  // Display its entry details, its modification time 
	  // and its attributes.
	  // Then change some attributes and print them again.
	console->Printf(KAttsMsg);
	
	  // When referring to a directory rather than a file, 
	  // a backslash must be appended to the path.
	TFileName thePath;
	fsSession.PrivatePath(thePath);
	thePath.Append(KSubDirName);
	
	   // Make the directory   
	TInt err=fsSession.MkDir(thePath);
	if (err!=KErrAlreadyExists)  // Don't leave if it already exists
		User::LeaveIfError(err);
		
	  // Create a file in "private\0ffffff03\f32examp\ "
	thePath.Append(KFileName);
	MakeSmallFile(thePath);
	
	  // Get entry details for file and print them	
	TEntry entry;	
	User::LeaveIfError(fsSession.Entry(thePath,entry)); 
	TBuf<30> dateString;
	entry.iModified.FormatL(dateString,KDateString);
 	  
 	  // Modification date and time = time of file's creation
    console->Printf(KDateMsg,&entry.iName,&dateString);
	  
	  // Print size of file 
	console->Printf(KSizeMsg,entry.iSize);
	TBuf<80> buffer;
	FormatEntry(buffer,entry); // Archive attribute should be set
	console->Printf(KBuffer,&buffer);
	buffer.Zero();

	  // get the entry details using Att() and Modified()
	TTime time;
	User::LeaveIfError(fsSession.Modified(thePath,time));
	time.FormatL(dateString,KDateString);
 	
 	  // Modification date and time = time of file's creation
    console->Printf(KEntryMsg,&entry.iName,&dateString);
	TUint value;
	User::LeaveIfError(fsSession.Att(thePath,value));
	FormatAtt(buffer,value); // get and print file attributes
	console->Printf(KAttMsg,&buffer);
	buffer.Zero();

	  // Change entry details using SetEntry() to clear archive
	User::LeaveIfError(fsSession.SetEntry(thePath,time,
			NULL,KEntryAttArchive));
	}

void PrintDirectoryLists()
	{
	   // Define text to be used for display at the console.
	_LIT(KListMsg,"\nDirectory listings\n");
	_LIT(KListMsg2,"\nDirectories and files:\n");
	_LIT(KDirList,"%S\n");
	_LIT(KDirs,"\nDirectories:\n");
	_LIT(KFilesSizes,"\nFiles and sizes:\n");
	_LIT(KBytes," %d bytes\n");
	_LIT(KNewLine,"\n");

       // Define subdirectory names and the file names to be used here.
    _LIT(KDir1,"f32examp\\tmpdir1\\");
	_LIT(KDir2,"f32examp\\tmpdir2\\");
	_LIT(KFile2,"f32examp\\tmpfile2.txt");
	_LIT(KDirName,"f32examp\\*");

	// Create some directories and files
	// in private"\f32examp\." 
	// List them using GetDir(), then list files and 
	// directories in separate lists. 

	console->Printf(KListMsg);

	TFileName thePrivatePath;
	fsSession.PrivatePath(thePrivatePath);
	
	TFileName thePath;
	TInt err;
	
	  // Create private\0fffff03\f32examp\tmpdir1
	thePath = thePrivatePath;
	thePath.Append(KDir1);
	err=fsSession.MkDir(thePath);
	if (err!=KErrAlreadyExists)
		User::LeaveIfError(err); // Don't leave if it already exists 
	
	  // Create "private\0fffff03\f32examp\tmpdir2"
	thePath = thePrivatePath;
	thePath.Append(KDir2);
	err=fsSession.MkDir(thePath);
	if (err!=KErrAlreadyExists)
		User::LeaveIfError(err); // Don't leave if it already exists 
		
	  // Create "private\0ffffff03\f32examp\tmpfile2.txt"
	thePath = thePrivatePath;
	thePath.Append(KFile2);
	MakeSmallFile(thePath);
	
	  // Now list all files and directories in "\f32examp\" 
	  // 
	  // in alphabetical order.
	thePath = thePrivatePath;
	thePath.Append(KDirName);
	CDir* dirList;
	 
	//err =  fsSession.GetDir(thePath,KEntryAttMaskSupported,ESortByName,dirList);
	
	User::LeaveIfError(fsSession.GetDir(thePath,KEntryAttMaskSupported,ESortByName,dirList));
	console->Printf(KListMsg2);
	TInt i;
	for (i=0;i<dirList->Count();i++)
		console->Printf(KDirList,&(*dirList)[i].iName);
	delete dirList;
	
	  // List the files and directories in \f32examp\ separately
	CDir* fileList;
	User::LeaveIfError(fsSession.GetDir(thePath,KEntryAttNormal,ESortByName,fileList,dirList));
	console->Printf(KDirs);
	for (i=0;i<dirList->Count();i++)
		console->Printf(KDirList,&(*dirList)[i].iName);
	console->Printf(KFilesSizes);
	for (i=0;i<fileList->Count();i++)
		{
		console->Printf(KDirList,&(*fileList)[i].iName);
		console->Printf(KBytes,(*fileList)[i].iSize);
		}
	console->Printf(KNewLine);
	delete dirList;
	delete fileList;

	}

void DeleteAll()
// Delete all the files and directories which have been created
	{
	// Define descriptor constants using the _LIT macro 
	_LIT(KDeleteMsg,"\nDeleteAll()\n");
	_LIT(KFile2,"f32examp\\tmpfile2.txt");
	_LIT(KDir1,"f32examp\\tmpdir1\\");
	_LIT(KDir2,"f32examp\\tmpdir2\\");
	_LIT(KFile1,"f32examp\\tmpfile.txt");
	_LIT(KTopDir,"f32examp\\");
	console->Printf(KDeleteMsg);
		
	TFileName thePrivatePath;
	fsSession.PrivatePath(thePrivatePath);
	
	TFileName thePath;
		
	thePath = thePrivatePath;
	thePath.Append(KFile2);
	User::LeaveIfError(fsSession.Delete(thePath));
	
	thePath = thePrivatePath;
	thePath.Append(KDir1);
	User::LeaveIfError(fsSession.RmDir(thePath));
	
	thePath = thePrivatePath;
	thePath.Append(KDir2);
	User::LeaveIfError(fsSession.RmDir(thePath));
	
	thePath = thePrivatePath;
	thePath.Append(KFile1);
	User::LeaveIfError(fsSession.Delete(thePath));
	
	thePath = thePrivatePath;
	thePath.Append(KTopDir);
	User::LeaveIfError(fsSession.RmDir(thePath));
	}

void MakeSmallFile(const TDesC& aFileName)
	{
	_LIT8(KFileData,"Some data");
 	RFile file;
 	User::LeaveIfError(file.Replace(fsSession,aFileName,EFileWrite));
	User::LeaveIfError(file.Write(KFileData));
	User::LeaveIfError(file.Flush()); // Commit data
	file.Close(); // close file having finished with it
	}
	 	 
void FormatEntry(TDes& aBuffer, const TEntry& aEntry)
	{
	_LIT(KEntryDetails,"Entry details: ");
	_LIT(KReadOnly," Read-only");
	_LIT(KHidden," Hidden");
	_LIT(KSystem," System");
	_LIT(KDirectory," Directory");
	_LIT(KArchive," Archive");
	_LIT(KNewLIne,"\n");
	aBuffer.Append(KEntryDetails);
	if(aEntry.IsReadOnly())
		aBuffer.Append(KReadOnly);
	if(aEntry.IsHidden())
		aBuffer.Append(KHidden);
	if(aEntry.IsSystem())
		aBuffer.Append(KSystem);
	if(aEntry.IsDir())
		aBuffer.Append(KDirectory);
	if(aEntry.IsArchive())
		aBuffer.Append(KArchive);		
	aBuffer.Append(KNewLIne);
	}

void FormatAtt(TDes& aBuffer, const TUint aValue)
	{
	_LIT(KAttsMsg,"Attributes set are:");
	_LIT(KNormal," Normal");
	_LIT(KReadOnly," Read-only");
	_LIT(KHidden," Hidden");
	_LIT(KSystem," System");
	_LIT(KVolume," Volume");
	_LIT(KDir," Directory");
	_LIT(KArchive," Archive");
	_LIT(KNewLine,"\n");
	aBuffer.Append(KAttsMsg);
	if (aValue & KEntryAttNormal)
		{
	 	aBuffer.Append(KNormal);
	 	return;
	 	}
	if (aValue & KEntryAttReadOnly)
	 	aBuffer.Append(KReadOnly);
	if (aValue & KEntryAttHidden)
		aBuffer.Append(KHidden);
	if (aValue & KEntryAttSystem)
		aBuffer.Append(KSystem);
	if (aValue & KEntryAttVolume)
		aBuffer.Append(KVolume);
	if (aValue & KEntryAttDir)
		aBuffer.Append(KDir);
	if (aValue & KEntryAttArchive)
		aBuffer.Append(KArchive);
	aBuffer.Append(KNewLine);
	}

