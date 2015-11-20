//******************************************************** 
//	System_Event_File.hpp - System Event Input/Output
//********************************************************

#ifndef SYSTEM_EVENT_FILE_HPP
#define SYSTEM_EVENT_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	System_Event_File Class definition
//---------------------------------------------------------

class  System_Event_File : public Db_Header
{
public:

	System_Event_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	System_Event_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~System_Event_File (void);

	char * Time (void)				{ Get_Field (time, svalue); return (svalue); }
	int Event (void)				{ Get_Field (type, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int Plan (void)					{ Get_Field (plan, &lvalue); return (lvalue); }
	int Phase (void)				{ Get_Field (phase, &lvalue); return (lvalue); }
	int Ring (void)					{ Get_Field (ring, &lvalue); return (lvalue); }
	int Group (void)				{ Get_Field (group, &lvalue); return (lvalue); }
	char * Status (void)			{ Get_Field (status, svalue); return (svalue); }

	void Time (char *value)			{ Put_Field (time, value); }
	void Event (int value)			{ Put_Field (type, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Plan (int value)			{ Put_Field (plan, value); }
	void Phase (int value)			{ Put_Field (phase, value); }
	void Ring (int value)			{ Put_Field (ring, value); }
	void Group (int value)			{ Put_Field (group, value); }
	void Status (char *value)		{ Put_Field (status, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int time, type, node, plan, phase, ring, group, status;
};

#endif
