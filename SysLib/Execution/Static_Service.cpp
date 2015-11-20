//*********************************************************
//	Static_Service.cpp - static program service
//*********************************************************

#include "Static_Service.hpp"

int Static_Scale::traveler_scale = 100;

Execution_Service * Static_Service::exe = NULL;

int Static_Service::default_format = 0;

//---------------------------------------------------------
//	Static_Scale constructor
//---------------------------------------------------------

Static_Scale::Static_Scale (int scale)
{
	Traveler_Scale (scale);
}

//---------------------------------------------------------
//	Static_Service constructor
//---------------------------------------------------------

Static_Service::Static_Service (Execution_Service *_exe, bool send_messages)
{
	if (_exe != NULL) {
		exe = _exe;
	}
	Send_Messages (send_messages);
}
