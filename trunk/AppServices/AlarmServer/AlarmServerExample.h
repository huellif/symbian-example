// AlarmServerExample.h
//
// Copyright (c) 2006 Symbian Software Ltd.  All rights reserved.
//
//

#if (!defined __ALARMSERVEREXAMPLE_H__)
#define __ALARMSERVEREXAMPLE_H__
#include <asshddefs.h>

class CExampleAlarmServer : public CBase
	{
public:
	static CExampleAlarmServer* NewL();
	void EgAlarmServerL();
	~CExampleAlarmServer();	
	
private:
	void ConstructL();
	void SetFloatingAlarmL();
	void AddFetchDeleteFixedAlarmL();
	void WaitForAlarmToBeCleared();
	void DisplayNotificationType(TAlarmChangeEvent aAlarmChangeEvent,TAlarmId aAlarmId);
	void DisplayAlarmState(TAlarmState aState); 
	
private:	

	RASCliSession iAlarmServerSession;
	};
#endif /*__ALARMSERVEREXAMPLE_H__ */
