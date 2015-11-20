//******************************************************** 
//	Event_File.hpp - Event Input/Output
//********************************************************

#ifndef EVENT_FILE_HPP
#define EVENT_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Event_File Class definition
//---------------------------------------------------------

class  Event_File : public Db_Header
{
public:

	Event_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Event_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Event_File (void);

	int Household (void)			{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)				{ Get_Field (person, &lvalue); return (lvalue); }
	int Trip (void)					{ Get_Field (trip, &lvalue); return (lvalue); }
	int Leg (void)					{ Get_Field (leg, &lvalue); return (lvalue); }
	int Mode (void)					{ Get_Field (mode, &lvalue); return (lvalue); }
	int Event (void)				{ Get_Field (type, &lvalue); return (lvalue); }
	char * Schedule (void)			{ Get_Field (schedule, svalue); return (svalue); }
	char * Actual (void)			{ Get_Field (actual, svalue); return (svalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Offset (void)				{ Get_Field (offset, &lvalue); return (lvalue); }

	void Household (int value)		{ Put_Field (hhold, value); }
	void Person (int value)			{ Put_Field (person, value); }
	void Trip (int value)			{ Put_Field (trip, value); }
	void Leg (int value)			{ Put_Field (leg, value); }
	void Mode (int value)			{ Put_Field (mode, value); }
	void Event (int value)			{ Put_Field (type, value); }
	void Schedule (char *value)		{ Put_Field (schedule, value); }
	void Actual (char *value)		{ Put_Field (actual, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Offset (int value)			{ Put_Field (offset, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, person, mode, trip, leg, type, schedule, actual, link, offset;
};

#endif
