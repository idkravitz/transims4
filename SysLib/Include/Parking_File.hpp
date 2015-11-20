//******************************************************** 
//	Parking_File.hpp - Parking File Input/Output
//********************************************************

#ifndef PARKING_FILE_HPP
#define PARKING_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Parking_File Class definition
//---------------------------------------------------------

class  Parking_File : public Db_Header
{
public:

	Parking_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Parking_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Parking_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	char * Type (void)				{ Get_Field (type, svalue); return (svalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	int Space (void)				{ Get_Field (space, &lvalue); return (lvalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	char * End (void)				{ Get_Field (end, svalue); return (svalue); }
	int Hourly (void)				{ Get_Field (hourly, &lvalue); return (lvalue); }
	int Daily (void)				{ Get_Field (daily, &lvalue); return (lvalue); }

	void ID (int value)				{ Put_Field (id, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Node (int value)			{ Put_Field (node, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Type (char * value)		{ Put_Field (type, value); }
	void Use (char * value)			{ Put_Field (use, value); }
	void Space (int value)			{ Put_Field (space, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void End (char * value)			{ Put_Field (end, value); }
	void Hourly (int value)			{ Put_Field (hourly, value); }
	void Daily (int value)			{ Put_Field (daily, value); }

	virtual bool Create_Fields (void);

protected:

	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int id, link, node, offset, type, use, space, start, end, hourly, daily;
};

#endif
