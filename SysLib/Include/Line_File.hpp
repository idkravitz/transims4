//******************************************************** 
//	Line_File.hpp - Transit Route File Input/Output
//********************************************************

#ifndef LINE_FILE_HPP
#define LINE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Line_File Class definition
//---------------------------------------------------------

class  Line_File : public Db_Header
{
public:
	Line_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Line_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Line_File (void);

	int Route (void)				{ Get_Field (route, &lvalue); return (lvalue); }
	int Stops (void)				{ Get_Field (stops, &lvalue); return (lvalue); }
	char * Mode (void)				{ Get_Field (mode, svalue); return (svalue); }
	char * Name (void)				{ Get_Field (name, svalue); return (svalue); }
	int Stop (void)					{ Get_Field (stop, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int Zone (void)					{ Get_Field (zone, &lvalue); return (lvalue); }
	int Time_Flag (void)			{ Get_Field (flag, &lvalue); return (lvalue); }

	void Route (int value)			{ Put_Field (route, value); }
	void Stops (int value)			{ Put_Field (stops, value); }
	void Mode (char * value)		{ Put_Field (mode, value); }
	void Name (char * value)		{ Put_Field (name, value); }
	void Stop (int value)			{ Put_Field (stop, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Zone (int value)			{ Put_Field (zone, value); }
	void Time_Flag (int value)		{ Put_Field (flag, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int lvalue;

	int route, stops, mode, stop, link, node, zone, flag, name;
};

#endif
