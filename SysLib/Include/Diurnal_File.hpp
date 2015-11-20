//******************************************************** 
//	Diurnal_File.hpp - Diurnal File Input/Output
//********************************************************

#ifndef DIURNAL_FILE_HPP
#define DIURNAL_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Diurnal_File Class definition
//---------------------------------------------------------

class  Diurnal_File : public Db_Header
{
public:

	Diurnal_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Diurnal_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Diurnal_File (void);

	char * Start_Time (void)		{ Get_Field (start, svalue); return (svalue); }
	char * End_Time (void)			{ Get_Field (end, svalue); return (svalue); }
	double Share (void)				{ Get_Field (share, &dvalue); return (dvalue); }

	void Start_Time (char *value)	{ Put_Field (start, value); }
	void End_Time (char *value)		{ Put_Field (end, value); }
	void Share (double value)		{ Put_Field (share, value); }

	int  Start_Field (void)         { return (start); }
	int  End_Field (void)           { return (end); }
	int  Share_Field (void)         { return (share); }
	void Share_Field (int value)    { share = value; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);
	virtual bool Default_Definition (void);

private:
	void Setup (void);

	double dvalue;

	int start, end, share;
};

#endif
