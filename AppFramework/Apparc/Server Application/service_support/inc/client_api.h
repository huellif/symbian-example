// client_api.h
//
// Copyright (c) Symbian Software Ltd 2006. All rights reserved.
//

#ifndef __CLIENT_API_H
#define __CLIENT_API_H

//CLASS DECLARATION

#include <EikServerApp.h>


//RServAppService declaration

class RServAppService : public REikAppServiceBase /*derives from RApaAppServiceBase */
	{
	public:
		IMPORT_C RServAppService();
		IMPORT_C ~RServAppService();
		
		IMPORT_C TInt Send(const TDesC & aMessage);  // simple method that poses a request to server
	private:
		TUid ServiceUid() const;
	};
	
#endif  //__CLIENT_API_H