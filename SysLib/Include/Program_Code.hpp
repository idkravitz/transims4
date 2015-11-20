//*********************************************************
//	Program_Code.hpp - user program codes
//*********************************************************

#ifndef PROGRAM_CODE_HPP
#define PROGRAM_CODE_HPP

#include "Static_Service.hpp"
#include "Program_Data.hpp"

//---------------------------------------------------------
//	Program_Code Class definition
//---------------------------------------------------------

class Program_Code : public Static_Service
{
public:
	Program_Code (void);

protected:

	enum Token_Type {LIMIT = 1, LOGICAL, RELATION, MATH, FUNCTION, CONVERT, DATETIME, IN_OUT, 
		TABLE, RETURN, DECLARE,	DATA, INTEGER, FLOAT, STRING, LVALUE, FVALUE, SVALUE, RECORD};

	enum Data_Type {INT_DATA, FLOAT_DATA, STRING_DATA, STRUCT_DATA};

	enum Tokens { EQUALS = 1, OPEN_PAR, CLOSE_PAR, COMMA, END_CMD, RANGE_DOT,	//---- limit ----
		IF, THEN, ELSE, ENDIF, WHILE, LOOP, BREAK, ENDLOOP,						//---- logical ----
		EQ, NEQ, LT, LE, GT, GE, NOT, AND, OR,									//---- relational ----
		PLUS, MINUS, MULTIPLY, DIVIDE, MODULO, POWER, NEGATIVE,					//---- math ----
		FMIN, FMAX, ABS, SQRT, EXP, LOG, LOG10, RANDOM,							//---- function ----
		FINT, FFLOAT, ROUND, ATOI, ATOF, ITOA, FTOA, SUBSTR,					//---- convert ----
		DOW, HOUR, MINUTE, SECOND, DAY, MONTH, YEAR, DATE, TIME,				//---- datetime ----
		DATE_TIME, DATE_STR, TIME_STR, TIME_STAMP,								//---- datetime ----
		PRINT, LIST, FORMAT, FREAD, FWRITE, INPUT, OUTPUT };					//---- in_out ----

	enum Table_Tokens { ITAB, RTAB, STAB, END_TAB };

	enum Declare_Tokens { IVAR, RVAR, SVAR, END_DEF };

	Token_Data * Keyword (char *token);
	Token_Data * Symbol (char *token);
	Token_Data * Special (char *token);
	Token_Data * Variable (char *token);

	char * Token_Name (int type, int code);
	char * Variable_Name (int type, int code);

	char * Data_Type (int type)							{ return ((type >= 0 && type <= STRUCT_DATA) ? data_type [type] : NULL); }
	bool Add_Variable (Token_Data *variable)			{ return (variables.Add (variable)); }

private:
	Token_Data *token_ptr;
	Token_Array variables;

	static Token_Data keywords [];
	static Token_Data symbols [];
	static Token_Data specials [];
	static char *data_type [];
};

#endif
