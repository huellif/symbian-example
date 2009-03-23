// plugin.h
//
// Copyright (c) 2000 Symbian Ltd.  All rights reserved.
//

// Transient server example - plugin class

#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#include <e32std.h>

template <class T> class CPlugIn
	{
	typedef T* (*TFactoryL)();
public:
	static T* NewL(const TDesC& aDllName);
	void Close();
protected:
	inline virtual ~CPlugIn() {}
private:
	RLibrary iLibrary;
	};

template <class T> T* CPlugIn<T>::NewL(const TDesC& aDllName)
	{
	RLibrary l;
	User::LeaveIfError(l.Load(aDllName));
	CleanupClosePushL(l);
	T* plugin=TFactoryL(l.Lookup(1))();
	static_cast<CPlugIn<T>*>(plugin)->iLibrary=l;
	CleanupStack::Pop();
	return plugin;
	}

template <class T> void CPlugIn<T>::Close()
	{
	RLibrary l(iLibrary);
	delete this;
	l.Close();
	}

#endif

