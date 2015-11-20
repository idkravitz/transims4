//******************************************************** 
//	Sign_File.hpp - Sign File Input/Output
//********************************************************

#ifndef SIGN_FILE_HPP
#define SIGN_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Sign_File Class definition
//---------------------------------------------------------

class  Sign_File : public Db_Header
{
public:

	Sign_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Sign_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Sign_File (void);

	int Node (void)					{ Get_Field (node, &lvalue); return (lvalue); }
	int Link (void)					{ Get_Field (link, &lvalue); return (lvalue); }
	char *Sign (void)				{ Get_Field (sign, svalue); return (svalue); }

	void Node (int value)			{ Put_Field (node, value); }
	void Link (int value)			{ Put_Field (link, value); }
	void Sign (char *value)			{ Put_Field (sign, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int node, link, sign;
};

#endif
