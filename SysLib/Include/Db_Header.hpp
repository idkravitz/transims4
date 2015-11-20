//********************************************************* 
//	Db_Header.hpp - generic database header interface
//*********************************************************

#ifndef DB_HEADER_HPP
#define DB_HEADER_HPP

#include "Db_Base.hpp"

#define SCAN_ALL_RECORDS	-1

//---------------------------------------------------------
//	Db_Header Class definition
//---------------------------------------------------------

class  Db_Header : public Db_Base
{
public:
	Db_Header (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	Db_Header (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool notes_flag = false);
	~Db_Header ();

	virtual bool Open (char *filename);
	virtual bool Close (void);

	void Header_Lines (int number)				{ header_lines = number; }
	int  Header_Lines (void)					{ return (header_lines); }

	void Scan_Lines (int number = 100)			{ scan_lines = number; }

	char * Header_Record (void)					{ return (header_record); }
	bool Header_Record (char * header);

	bool Custom_Header (void)					{ return (custom_header); } 
	void Custom_Header (bool flag)				{ custom_header = flag; }

	int  LinkDir_Type (void)					{ return (linkdir_type); }
	void LinkDir_Type (Direction_Type type)		{ linkdir_type = type; }

	bool Notes_Flag (void)						{ return (notes_flag); }
	void Notes_Flag (bool flag)					{ notes_flag = flag; }

	int  Notes_Field (void)						{ return (notes); }
	void Notes_Field (int field)				{ Notes_Flag ((notes = field) != 0); }

	char *Notes (void)							{ Get_Field (notes, svalue); return (svalue); }
	void Notes (char *value)					{ Put_Field (notes, value); }

	virtual bool Create_Fields (void)			{ return (Set_Field_Numbers ()); }
	virtual bool Write_Header (char *user_lines = NULL);
	
protected:
	virtual bool Read_Header (bool stat);
	virtual bool Set_Field_Numbers (void)		{ return (true); }
	virtual bool Default_Definition (void)		{ return (false); }

	bool Write_Def_Header (char *user_lines = NULL);

	char svalue [FIELD_BUFFER];

private:

	//---- header structure -----

	typedef struct {
		char  version;
		char  date [3];
		int   num_records;
		short header_size;
		short record_size;
		char  reserve1 [2];
		char  transaction_flag;
		char  encryption_flag;
		char  reserve2 [12];
		char  mdx_flag;
		char  reserve3 [3];
	} dBase_Header;

	//---- field structure ----

	typedef  struct {
		char  name [11];	
		unsigned char  type;
		unsigned char  reserve1 [4];
		unsigned char  width;
		unsigned char  decimal;
		unsigned short offset;
		unsigned char  work_id;
		unsigned char  reserve3 [11];
	} dBase_Field;

	bool Read_CSV_Header (bool stat);
	bool Write_CSV_Header (char *user_lines = NULL);

	bool Read_dBase_Header (bool stat);
	bool Write_dBase_Header (void);
	
	bool Read_SQLite3_Header (bool stat);
	bool Write_SQLite3_Header (void);

	bool Read_Def_Header (void);

	int header_lines;
	int scan_lines;
	char *header_record;
	bool custom_header;
	bool notes_flag;
	int notes;
	int linkdir_type;

	static char aecom_header [];
	static char nested_flag [];
};
#endif
