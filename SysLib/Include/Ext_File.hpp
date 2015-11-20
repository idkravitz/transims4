//********************************************************* 
//	Ext_File.hpp - File Extension Class
//*********************************************************

#ifndef EXT_FILE_HPP
#define EXT_FILE_HPP

#include "Db_File.hpp"

#include <stdio.h>

//---------------------------------------------------------
//	Ext_File Class definition
//---------------------------------------------------------

class  Ext_File : public Db_File
{
public:
	Ext_File (Access_Type access = READ, Format_Type format = UNFORMATED);
	Ext_File (char *filename, Access_Type access = READ, Format_Type format = UNFORMATED);
	~Ext_File ();

	//---- file functions ----

	bool   Filename (char *rootname, int ext_number = -1);
	bool   Filename (int ext_number);

	char * Filename (void)					{ return (Db_File::Filename ()); };	

	virtual bool Open (int ext_number);
	virtual bool Open (char *filename = NULL)	{ return (Db_File::Open (filename)); }

	char * Pathname (void)					{ return (pathname); };
	char * Groupname (void);

	char * Extension (void)					{ return (extension); };
	char * Extension (int ext_number);

	//--- type functions ----

	int  Extend (void)						{ return (extend); };
	int  Extend (int _extend)				{ if (!extend != !_extend) extend = _extend; return (extend);};

private:
	int extend;

	char *pathname, extension [8];

	static char buffer [STRING_BUFFER];
};
#endif
