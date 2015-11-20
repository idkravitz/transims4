//******************************************************** 
//	Schedule_File.hpp - Transit Schedule File Input/Output
//********************************************************

#ifndef SCHEDULE_FILE_HPP
#define SCHEDULE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Schedule_File Class definition
//---------------------------------------------------------

class  Schedule_File : public Db_Header
{
public:

	Schedule_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Schedule_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Schedule_File (void);

	int Route (void)				{ Get_Field (route, &lvalue); return (lvalue); }
	int Time (void)					{ Get_Field (time, &lvalue); return (lvalue); }
	int Stop (void)					{ Get_Field (stop, &lvalue); return (lvalue); }

	void Route (int value)			{ Put_Field (route, value); }
	void Time (int value)			{ Put_Field (time, value); }
	void Stop (int value)			{ Put_Field (stop, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int route, time, stop;
};

#endif
