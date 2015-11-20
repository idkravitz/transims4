//******************************************************** 
//	Driver_File.hpp - Transit Driver File Input/Output
//********************************************************

#ifndef DRIVER_FILE_HPP
#define DRIVER_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Driver_File Class definition
//---------------------------------------------------------

class  Driver_File : public Db_Header
{
public:
	Driver_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Driver_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Driver_File (void);

	int Route (void)				{ Get_Field (route, &lvalue); return (lvalue); }
	int Links (void)				{ Get_Field (links, &lvalue); return (lvalue); }
	int Veh_Type (void)				{ Get_Field (veh_type, &lvalue); return (lvalue); }
	int Sub_Type (void)				{ Get_Field (sub_type, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }

	void Route (int value)			{ Put_Field (route, value); }
	void Links (int value)			{ Put_Field (links, value); }
	void Veh_Type (int value)		{ Put_Field (veh_type, value); }
	void Sub_Type (int value)		{ Put_Field (sub_type, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	int lvalue;

	int route, links, veh_type, sub_type, link, dir;
};

#endif
