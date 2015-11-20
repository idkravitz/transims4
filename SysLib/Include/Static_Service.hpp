//*********************************************************
//	Static_Service.hpp - static program services
//*********************************************************

#ifndef STATIC_SERVICE_HPP
#define STATIC_SERVICE_HPP

#include "Execution_Service.hpp"

//---------------------------------------------------------
//	Static_Scale - static scaling factor
//---------------------------------------------------------

class Static_Scale
{
public:

	Static_Scale (int traveler_scale = 0);

	int Traveler_Scale (void)			{ return (traveler_scale); }
	void Traveler_Scale (int value)     { if (value > 1 && value <= 100) traveler_scale = value; }

private:

	static int  traveler_scale;
};

//---------------------------------------------------------
//	Static_Service - static program services
//---------------------------------------------------------

class Static_Service
{
public:

	Static_Service (Execution_Service *exe = NULL, bool send_messages = true);

	static Execution_Service *exe;

	bool Send_Messages (void)			{ return (send_messages); }
	void Send_Messages (bool send)		{ send_messages = (exe) ? send : false; }

	int  Default_Format (void)			{ return (default_format); }
	void Default_Format (int format)	{ default_format = format; }

private:
	bool send_messages;

	static int  default_format;
};
#endif
