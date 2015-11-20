//******************************************************** 
//	Tour_File.hpp - Tour File Input/Output
//********************************************************

#ifndef TOUR_FILE_HPP
#define TOUR_FILE_HPP

#include "Ext_Header.hpp"

//---------------------------------------------------------
//	Tour_File Class definition
//---------------------------------------------------------

class  Tour_File : public Ext_Header
{
public:

	Tour_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Tour_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Tour_File (void);

	int Household (void)				{ Get_Field (hhold, &lvalue); return (lvalue); }
	int Person (void)					{ Get_Field (person, &lvalue); return (lvalue); }
	int Tour (void)						{ Get_Field (tour, &lvalue); return (lvalue); }
	int Purpose (void)					{ Get_Field (purpose, &lvalue); return (lvalue); }
	int Mode (void)						{ Get_Field (mode, &lvalue); return (lvalue); }
	int Origin (void)					{ Get_Field (origin, &lvalue); return (lvalue); }
	int Destination (void)				{ Get_Field (destination, &lvalue); return (lvalue); }
	int Stop_Out (void)					{ Get_Field (stop_out, &lvalue); return (lvalue); }
	int Stop_In (void)					{ Get_Field (stop_in, &lvalue); return (lvalue); }
	int Start (void)					{ Get_Field (start, &lvalue); return (lvalue); }
	int Return (void)				    { Get_Field (end, &lvalue); return (lvalue); }
	int Group (void)					{ Get_Field (group, &lvalue); return (lvalue); }

	void Household (int value)			{ Put_Field (hhold, value); }
	void Person (int value)				{ Put_Field (person, value); }
	void Tour (int value)				{ Put_Field (tour, value); }
	void Purpose (int value)			{ Put_Field (purpose, value); }
	void Mode (int value)				{ Put_Field (mode, value); }
	void Origin (int value)				{ Put_Field (origin, value); }
	void Destination (int value)		{ Put_Field (destination, value); }
	void Stop_Out (int value)			{ Put_Field (stop_out, value); }
	void Stop_In (int value)			{ Put_Field (stop_in, value); }
	void Start (int value)				{ Put_Field (start, value); }
	void Return (int value)				{ Put_Field (end, value); }
	void Group (int value)				{ Put_Field (group, value); }

	virtual bool Create_Fields (void);
	
	int  HHold_Field (void)				{ return (hhold); }
	int  Person_Field (void)			{ return (person); }
	int  Tour_Field (void)				{ return (tour); }
	int  Purpose_Field (void)			{ return (purpose); }
	int  Mode_Field (void)				{ return (mode); }
	int  Origin_Field (void)			{ return (origin); }
	int  Dest_Field (void)				{ return (destination); }
	int  Stop_Out_Field (void)			{ return (stop_out); }
	int  Stop_In_Field (void)			{ return (stop_in); }  
	int  Start_Field (void)				{ return (start); }
	int  Return_Field (void)			{ return (end); }
	int  Group_Field (void)				{ return (group); }

	int  HHold_Field (char *name)		{ return ((hhold = Required_Field (name))); }
	int  Person_Field (char *name)		{ return ((person = Required_Field (name))); }
	int  Tour_Field (char *name)		{ return ((tour = Required_Field (name))); }
	int  Purpose_Field (char *name)		{ return ((purpose = Required_Field (name))); }
	int  Mode_Field (char *name)		{ return ((mode = Required_Field (name))); }
	int  Origin_Field (char *name)		{ return ((origin = Required_Field (name))); }
	int  Dest_Field (char *name)		{ return ((destination = Required_Field (name))); }
	int  Stop_Out_Field (char *name)	{ return ((stop_out = Required_Field (name))); }
	int  Stop_In_Field (char *name)		{ return ((stop_in = Required_Field (name))); }  
	int  Start_Field (char *name)		{ return ((start = Required_Field (name))); }
	int  Return_Field (char *name)		{ return ((end = Required_Field (name))); }
	int  Group_Field (char *name)		{ return ((group = Required_Field (name))); }

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int hhold, person, tour, purpose, mode, origin, destination, stop_out, stop_in, start, end, group;
};

#endif
