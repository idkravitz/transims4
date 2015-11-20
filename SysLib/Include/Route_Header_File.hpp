//******************************************************** 
//	Route_Header_File.hpp - Route Header File Input/Output
//********************************************************

#ifndef ROUTE_HEADER_FILE_HPP
#define ROUTE_HEADER_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Route_Header_File Class definition
//---------------------------------------------------------

class  Route_Header_File : public Db_Header
{
public:
	Route_Header_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Route_Header_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Route_Header_File (void);

	int Route (void)						{ Get_Field (route, &lvalue); return (lvalue); }
	char * Mode (void)						{ Get_Field (mode, svalue); return (svalue); }
	char * Name (void)						{ Get_Field (name, svalue); return (svalue); }
	int Min_Time (void)						{ Get_Field (time, &lvalue); return (lvalue); }
	int Periods (void)						{ return (periods); }
	int Headway (int period)				{ Get_Field (headway_field (period), &lvalue); return (lvalue); }
	int Offset (int period)					{ Get_Field (offset_field (period), &lvalue); return (lvalue); }
	int TTime (int period)					{ Get_Field (ttime_field (period), &lvalue); return (lvalue); }

	void Route (int value)					{ Put_Field (route, value); }
	void Mode (char * value)				{ Put_Field (mode, value); }
	void Name (char * value)				{ Put_Field (name, value); }
	void Min_Time (int value)				{ Put_Field (time, value); }
	bool Periods (int value);
	void Headway (int period, int value)	{ Put_Field (headway_field (period), value); }
	void Offset (int period, int value)		{ Put_Field (offset_field (period), value); }
	void TTime (int period, int value)		{ Put_Field (ttime_field (period), value); }

	bool Time_Flag (void)					{ return (time_flag); }
	void Time_Flag (bool flag)				{ time_flag = flag; }
	
	virtual bool Create_Fields (void);
	bool Create_Fields (int num_periods, bool time_flag = false);
	
	void Zero_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);
	
	void Clear_Fields (void);

	typedef struct {
		int headway;		//---- minutes ----
		int offset;			//---- minutes ----
		int ttime;			//---- minutes ----
	} Period_Data;
	
	int  headway_field (int p)			{ return ((period && p > 0 && p <= periods) ? period [p-1].headway : 0); }
	int  offset_field (int p)			{ return ((period && p > 0 && p <= periods) ? period [p-1].offset : 0); }
	int  ttime_field (int p)			{ return ((period && p > 0 && p <= periods) ? period [p-1].ttime : 0); }
	
	void headway_field (int p, int n)	{ if (period && p > 0 && p <= periods) period [p-1].headway = n; }
	void offset_field (int p, int n)	{ if (period && p > 0 && p <= periods) period [p-1].offset = n; }
	void ttime_field (int p, int n)		{ if (period && p > 0 && p <= periods) period [p-1].ttime = n; }

	int lvalue;

	bool time_flag;
	int periods;
	int route, mode, time, name;
	Period_Data *period;
};

#endif
