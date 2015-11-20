//******************************************************** 
//	Access_File.hpp - Process Link File Input/Output
//********************************************************

#ifndef ACCESS_FILE_HPP
#define ACCESS_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Access_File Class definition
//---------------------------------------------------------

class  Access_File : public Db_Header
{
public:

	Access_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Access_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Access_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }
	int From_ID (void)				{ Get_Field (from_id, &lvalue); return (lvalue); }
	char * From_Type (void)			{ Get_Field (from_type, svalue); return (svalue); }
	int To_ID (void)				{ Get_Field (to_id, &lvalue); return (lvalue); }
	char * To_Type (void)			{ Get_Field (to_type, svalue); return (svalue); }
	double Time (void)				{ Get_Field (time, &dvalue); return (dvalue); }
	double Cost (void)				{ Get_Field (cost, &dvalue); return (dvalue); }

	void ID (int value)				{ Put_Field (id, value); }
	void From_ID (int value)		{ Put_Field (from_id, value); }
	void From_Type (char *value)	{ Put_Field (from_type, value); }
	void To_ID (int value)			{ Put_Field (to_id, value); }
	void To_Type (char *value)		{ Put_Field (to_type, value); }
	void Time (double value)		{ Put_Field (time, value); }
	void Cost (double value)		{ Put_Field (cost, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;
	double dvalue;

	int id, from_id, to_id, from_type, to_type, time, cost;
};

#endif
