//********************************************************* 
//	Ext_Header.hpp - File Extension Class
//*********************************************************

#ifndef EXT_HEADER_HPP
#define EXT_HEADER_HPP

#include "Db_Header.hpp"

#include <stdio.h>

//---------------------------------------------------------
//	Ext_Header Class definition
//---------------------------------------------------------

class  Ext_Header : public Db_Header
{
public:
	Ext_Header (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Ext_Header (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	~Ext_Header ();

	//---- file functions ----

	bool   Filename (char *rootname, int ext_number = -1);
	bool   Filename (int ext_number);

	char * Filename (void)					{ return (Db_Header::Filename ()); };	

	virtual bool Open (int ext_number);
	virtual bool Open (char *filename = NULL)	{ return (Db_Header::Open (filename)); }

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
