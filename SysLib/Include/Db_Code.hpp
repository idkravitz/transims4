//********************************************************* 
//	Db_Code.hpp - database codes
//*********************************************************

#ifndef DB_CODE_HPP
#define DB_CODE_HPP

#include "Static_Service.hpp"

//---------------------------------------------------------
//	Db_Code Class definition
//---------------------------------------------------------

class Db_Code : public Static_Service
{
public:
	Db_Code (void);

	enum Format_Type { DEFAULT_FORMAT, UNFORMATED, BINARY, FIXED_COLUMN, COMMA_DELIMITED, 
						SPACE_DELIMITED, TAB_DELIMITED, CSV_DELIMITED, DBASE, VERSION3, SQLITE3 };

	enum Access_Type {READ, MODIFY, APPEND, CREATE};

	enum Field_Type {INTEGER, UNSIGNED, DOUBLE, FIXED, STRING, CHAR, DATE, TIME, DATE_TIME, DAY_TIME};

	enum Status_Code {OK, ERROR, MEMORY, RECORD_SIZE,					//---- Db_Record ----
				NOT_OPEN, NULL_POINTER, FILE_IO, CLOSING, POSITIONING,	//---- Db_File ----
				FIELD_LIST,	NO_FIELD, FIELD_BYTES, NULL_NAME,			//---- Db_Base ----
				EXTENSION,												//---- Ext_File ----
				PLAN_FIELDS,											//---- Plan_IO ----
				FILE_HEADER, RECORD_SYNTAX, FIELD_TYPE,					//---- Csv_File ----
				OPEN_DEF, DEF_FORMAT, DEF_NEST,							//---- Db_Header ----
				HEADER_MEMORY, INDEX_OPEN, HEADER_READ, HEADER_VERSION, //---- Btree_Index ----
				NODE_MEMORY, NODE_READ, PATH_NAME, HEADER_WRITE, 
				TOO_MANY_FILES, NODE_WRITE
	};

	//---- Plan File ----

	enum Plan_Mode {AUTO_MODE, TRANSIT_MODE, WALK_MODE, BIKE_MODE, ACTIVITY_MODE, OTHER_MODE, MAGIC_MODE, CARPOOL_MODE, DRIVER_MODE};

	enum Memory_Type {PRIVATE_MEMORY, ALLOCATE_MEMORY};

	enum Sort_Type {TRAVELER_SORT, TIME_SORT};

	enum Shape_Code { VIEW, LINE, ARC, CIRCLE, RECTANGLE, POLYGON, VECTOR, GTEXT, ARROW, DOT, MDOTS };

	enum Direction_Type { LINK_SIGN, LINK_DIR, LINK_NODE };

	Format_Type Format_Code (char *text);
	char * Format_Code (Format_Type code);

protected:

	Field_Type Field_Code (char *text);
	char * Field_Code (Field_Type code);

private:

	static char *format_text [];
	static Format_Type format_code [];

	static char *field_text [];
	static Field_Type field_code [];
};
#endif
