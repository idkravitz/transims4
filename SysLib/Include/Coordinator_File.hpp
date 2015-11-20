//******************************************************** 
//	Coordinator_File.hpp - Coordinator File Input/Output
//********************************************************

#ifndef COORDINATOR_FILE_HPP
#define COORDINATOR_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Coordinator_File Class definition
//---------------------------------------------------------

class  Coordinator_File : public Db_Header
{
public:

	Coordinator_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Coordinator_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	virtual ~Coordinator_File (void);

	int ID (void)					{ Get_Field (id, &lvalue); return (lvalue); }

	void ID (int value)				{ Put_Field (id, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int id;
};

#endif
