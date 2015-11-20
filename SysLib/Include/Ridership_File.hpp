//******************************************************** 
//	Ridership_File.hpp - Transit Ridership Input/Output
//********************************************************

#ifndef RIDERSHIP_FILE_HPP
#define RIDERSHIP_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Ridership_File Class definition
//---------------------------------------------------------

class  Ridership_File : public Db_Header
{
public:

	Ridership_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Ridership_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Ridership_File (void);

	char * Mode (void)				{ Get_Field (mode, svalue); return (svalue); }
	int Route (void)				{ Get_Field (route, &lvalue); return (lvalue); }
	int Run (void)					{ Get_Field (run, &lvalue); return (lvalue); }
	int Stop (void)					{ Get_Field (stop, &lvalue); return (lvalue); }
	char * Schedule (void)			{ Get_Field (schedule, svalue); return (svalue); }
	char * Time (void)				{ Get_Field (time, svalue); return (svalue); }
	int Board (void)				{ Get_Field (board, &lvalue); return (lvalue); }
	int Alight (void)				{ Get_Field (alight, &lvalue); return (lvalue); }
	int Load (void)					{ Get_Field (load, &lvalue); return (lvalue); }
	double Factor (void)			{ Get_Field (factor, &dvalue); return (dvalue); }

	void Mode (char *value)			{ Put_Field (mode, value); }
	void Route (int value)			{ Put_Field (route, value); }
	void Run (int value)			{ Put_Field (run, value); }
	void Stop (int value)			{ Put_Field (stop, value); }
	void Schedule (char *value)		{ Put_Field (schedule, value); }
	void Time (char *value)			{ Put_Field (time, value); }
	void Board (int value)			{ Put_Field (board, value); }
	void Alight (int value)			{ Put_Field (alight, value); }
	void Load (int value)			{ Put_Field (load, value); }
	void Factor (double value)		{ Put_Field (factor, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int mode, route, run, stop, schedule, time, board, alight, load, factor;
};

#endif
