//******************************************************** 
//	Link_Detail_File.hpp - Link Detail File Input/Output
//********************************************************

#ifndef LINK_DETAIL_FILE_HPP
#define LINK_DETAIL_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Link_Detail_File Class definition
//---------------------------------------------------------

class  Link_Detail_File : public Db_Header
{
public:

	Link_Detail_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Link_Detail_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Link_Detail_File (void);

	int    Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int    Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	char * Control (void)				{ Get_Field (control, svalue); return (svalue); }
	int    Left (void)                  { Get_Field (left, &lvalue); return (lvalue); }
	int    Left_Thru (void)             { Get_Field (left_thru, &lvalue); return (lvalue); }
	int    Thru (void)                  { Get_Field (thru, &lvalue); return (lvalue); }
	int    Right_Thru (void)            { Get_Field (right_thru, &lvalue); return (lvalue); }
	int    Right (void)                 { Get_Field (right, &lvalue); return (lvalue); }
	char * Use_Type (void)				{ Get_Field (use, svalue); return (svalue); }
	int    Use_Lanes (void)				{ Get_Field (lanes, &lvalue); return (lvalue); }
	char * Use_Period (void)			{ Get_Field (period, svalue); return (svalue); }

	void   Link (int value)	 			{ Put_Field (link, value); }
	void   Dir (int value)				{ Put_Field (dir, value); }
	void   Control (char * value)		{ Put_Field (control, value); }
	void   Left (int value)				{ Put_Field (left, value); }
	void   Left_Thru (int value)		{ Put_Field (left_thru, value); }
	void   Thru (int value)				{ Put_Field (thru, value); }
	void   Right_Thru (int value)		{ Put_Field (right_thru, value); }
	void   Right (int value)			{ Put_Field (right, value); }
	void   Use_Type (char * value)		{ Put_Field (use, value); }
	void   Use_Lanes (int value)		{ Put_Field (lanes, value); }
	void   Use_Period (char * value)	{ Put_Field (period, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int link, dir, control, left, left_thru, thru, right_thru, right, use, lanes, period;
};

#endif
