//******************************************************** 
//	Toll_File.hpp - Toll File Input/Output
//********************************************************

#ifndef TOLL_FILE_HPP
#define TOLL_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Toll_File Class definition
//---------------------------------------------------------

class  Toll_File : public Db_Header
{
public:

	Toll_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Toll_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Toll_File (void);

	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	int Dir (void)					{ Get_Field (dir, &lvalue); return (lvalue); }
	char * Start (void)				{ Get_Field (start, svalue); return (svalue); }
	char * End (void)				{ Get_Field (end, svalue); return (svalue); }
	char * Use (void)				{ Get_Field (use, svalue); return (svalue); }
	int Toll (void)					{ Get_Field (toll, &lvalue); return (lvalue); }

	void Link (int value)			{ Put_Field (link, value); }
	void Dir (int value)			{ Put_Field (dir, value); }
	void Start (char * value)		{ Put_Field (start, value); }
	void End (char * value)			{ Put_Field (end, value); }
	void Use (char * value)			{ Put_Field (use, value); }
	void Toll (int value)			{ Put_Field (toll, value); }

	bool Use_Flag (void)			{ return (use != 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int link, dir, start, end, use, toll;
};

#endif
