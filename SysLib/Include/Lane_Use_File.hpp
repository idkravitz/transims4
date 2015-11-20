//******************************************************** 
//	Lane_Use_File.hpp - Lane Use File Input/Output
//********************************************************

#ifndef LANE_USE_FILE_HPP
#define LANE_USE_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Lane_Use_File Class definition
//---------------------------------------------------------

class  Lane_Use_File : public Db_Header
{
public:

	Lane_Use_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Lane_Use_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Lane_Use_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	int Lane (void)					{ Get_Field (lane, &lvalue); return (lvalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	char * Type (void)				{ Get_Field (type, svalue); return (svalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	char * End (void)				{ Get_Field (end, svalue); return (svalue); }
	double Offset (void)			{ Get_Field (offset, &dvalue); return (dvalue); }
	double Length (void)			{ Get_Field (length, &dvalue); return (dvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Lane (int value)			{ Put_Field (lane, value); }
	void Use (char * value)			{ Put_Field (use, value); }
	void Type (char * value)		{ Put_Field (type, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void End (char * value)			{ Put_Field (end, value); }
	void Offset (double value)		{ Put_Field (offset, value); }
	void Length (double value)		{ Put_Field (length, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int link, dir, lane, type, use, start, end, offset, length;
};

#endif
