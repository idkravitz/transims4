//********************************************************* 
//	Db_Status.hpp - database status code
//*********************************************************

#ifndef DB_STATUS_HPP
#define DB_STATUS_HPP

#include "Db_Code.hpp"

//---------------------------------------------------------
//	Db_Status Class definition
//---------------------------------------------------------

class  Db_Status : public Db_Code
{
public:
	Db_Status (void);
	virtual ~Db_Status (void);

	bool Status (void)					{ return (status == OK); }
	bool Status (Status_Code stat, bool message = true);

	bool File_ID (char *file_id);
	char * File_ID (void)				{ return ((file_id != NULL) ? file_id : default_id); }

	bool File_Type (char *label);
	char * File_Type (void)				{ return ((file_type != NULL) ? file_type : default_type); }

	Status_Code Get_Status (void)		{ return (status); }

	char * Status_Message (void);

private:
	Status_Code status;

	char *file_type;
	char *file_id;

	static char *default_type;
	static char *default_id;
};
#endif
