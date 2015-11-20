//*********************************************************
//	Program_Code.cpp - user program codes
//*********************************************************

#include "Program_Code.hpp"

#include "Utility.hpp"

Token_Data  Program_Code::keywords [] = {
		{"IF",			LOGICAL,	IF			},
		{"THEN",		LOGICAL,	THEN		},
		{"ELSE",		LOGICAL,	ELSE		},
		{"ENDIF",		LOGICAL,	ENDIF		},
		{"WHILE",		LOGICAL,	WHILE		},
		{"LOOP",		LOGICAL,	LOOP		},
		{"BREAK",		LOGICAL,	BREAK		},
		{"ENDLOOP",		LOGICAL,	ENDLOOP		},
		{"EQ",			RELATION,	EQ			},
		{"NE",			RELATION,	NEQ			},
		{"LT",			RELATION,	LT			},
		{"LE",			RELATION,	LE			},
		{"GT",			RELATION,	GT			},
		{"GE",			RELATION,	GE			},
		{"NOT",			RELATION,	NOT			},
		{"AND",			RELATION,	AND			},
		{"OR",			RELATION,	OR			},
		{"MIN",			FUNCTION,	FMIN		},
		{"MAX",			FUNCTION,	FMAX		},
		{"ABS",			FUNCTION,	ABS			},
		{"SQRT",		FUNCTION,	SQRT		},
		{"EXP",			FUNCTION,	EXP			},
		{"LOG",			FUNCTION,	LOG			},
		{"LOG10",		FUNCTION,	LOG10		},
		{"RANDOM",		FUNCTION,	RANDOM		},
		{"POWER",		FUNCTION,	POWER		},
		{"INT",			CONVERT,	FINT		},
		{"FLOAT",		CONVERT,	FFLOAT		},
		{"ROUND",		CONVERT,	ROUND		},
		{"ATOI",		CONVERT,	ATOI		},
		{"ATOF",		CONVERT,	ATOF		},
		{"ITOA",		CONVERT,	ITOA		},
		{"FTOA",		CONVERT,	FTOA		},
		{"SUBSTR",		CONVERT,	SUBSTR		},
		{"DOW",			DATETIME,	DOW			}, 
		{"HOUR",		DATETIME,	HOUR		},
		{"MINUTE",		DATETIME,	MINUTE		},
		{"SECOND",		DATETIME,	SECOND		},
		{"DAY",			DATETIME,	DAY			},
		{"MONTH",		DATETIME,	MONTH		},
		{"YEAR",		DATETIME,	YEAR		},
		{"DATE",		DATETIME,	DATE		},
		{"TIME",		DATETIME,	TIME		},
		{"DATE_TIME",	DATETIME,	DATE_TIME	},
		{"DATE_STR",	DATETIME,	DATE_STR	},
		{"TIME_STR",	DATETIME,	TIME_STR	},
		{"TIME_STAMP",	DATETIME,	TIME_STAMP	},
		{"PRINT",		IN_OUT,		PRINT		},
		{"LIST",		IN_OUT,		LIST		},
		{"FORMAT",		IN_OUT,		FORMAT		},
		{"READ",		IN_OUT,		FREAD		},
		{"WRITE",		IN_OUT,		FWRITE		},
		{"INPUT",		IN_OUT,		INPUT		},
		{"OUTPUT",		IN_OUT,		OUTPUT		},
		{"RETURN",		RETURN,		RETURN		},
		{"END",			LIMIT,		END_CMD		},
		{NULL,			0,			0			}
};

Token_Data  Program_Code::symbols [] = {
		{"=",		LIMIT,		EQUALS		},
		{"+",		MATH,		PLUS		},
		{"-",		MATH,		MINUS		},
		{"*",		MATH,		MULTIPLY	},
		{"/",		MATH,		DIVIDE		},
		{"%",		MATH,		MODULO		},
		{"**",		MATH,		POWER		},
		{"==",		RELATION,	EQ			},
		{"!=",		RELATION,	NEQ			},
		{"<",		RELATION,	LT			},
		{"<=",		RELATION,	LE			},
		{">",		RELATION,	GT			},
		{">=",		RELATION,	GE			},
		{"!",		RELATION,	NOT			},
		{"&&",		RELATION,	AND			},
		{"||",		RELATION,	OR			},
		{"{",		LOGICAL,	LOOP		},
		{"}",		LOGICAL,	ENDLOOP		},
		{NULL,		 0,			0			}
};

Token_Data  Program_Code::specials [] = {
		{"INTEGER",	DECLARE,	IVAR		},
		{"REAL",	DECLARE,	RVAR		},
		{"STRING",	DECLARE,	SVAR		},
		{"ITAB",	TABLE,		ITAB		},
		{"RTAB",	TABLE,		RTAB		},
		{"STAB",	TABLE,		STAB		},
		{"ITAB0",	TABLE,		ITAB		},
		{"RTAB0",	TABLE,		RTAB		},
		{"STAB0",	TABLE,		STAB		},
		{"ITAB1",	TABLE,		ITAB + 10	},
		{"RTAB1",	TABLE,		RTAB + 10	},
		{"STAB1",	TABLE,		STAB + 10	},
		{"ITAB2",	TABLE,		ITAB + 20	},
		{"RTAB2",	TABLE,		RTAB + 20	},
		{"STAB2",	TABLE,		STAB + 20	},
		{"ITAB3",	TABLE,		ITAB + 30	},
		{"RTAB3",	TABLE,		RTAB + 30	},
		{"STAB3",	TABLE,		STAB + 30	},
		{"ITAB4",	TABLE,		ITAB + 40	},
		{"RTAB4",	TABLE,		RTAB + 40	},
		{"STAB4",	TABLE,		STAB + 40	},
		{"ITAB5",	TABLE,		ITAB + 50	},
		{"RTAB5",	TABLE,		RTAB + 50	},
		{"STAB5",	TABLE,		STAB + 50	},
		{"ITAB6",	TABLE,		ITAB + 60	},
		{"RTAB6",	TABLE,		RTAB + 60	},
		{"STAB6",	TABLE,		STAB + 60	},
		{"ITAB7",	TABLE,		ITAB + 70	},
		{"RTAB7",	TABLE,		RTAB + 70	},
		{"STAB7",	TABLE,		STAB + 70	},
		{"ITAB8",	TABLE,		ITAB + 80	},
		{"RTAB8",	TABLE,		RTAB + 80	},
		{"STAB8",	TABLE,		STAB + 80	},
		{"ITAB9",	TABLE,		ITAB + 90	},
		{"RTAB9",	TABLE,		RTAB + 90	},
		{"STAB9",	TABLE,		STAB + 90	},
		{NULL,		0,			0			}
};

char * Program_Code::data_type [] = {
	"Integer",
	"Decimal",
	"String",
	"Structure"
};

//---------------------------------------------------------
//	Program_Code constructor
//---------------------------------------------------------

Program_Code::Program_Code (void) : Static_Service ()
{
}

//---------------------------------------------------------
//	Keyword
//---------------------------------------------------------

Token_Data * Program_Code::Keyword (char *token)
{
	for (token_ptr = keywords; token_ptr->name; token_ptr++) {
		if (str_cmp (token_ptr->name, token) == 0) {
			return (token_ptr);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Symbol
//---------------------------------------------------------

Token_Data * Program_Code::Symbol (char *token)
{
	for (token_ptr = symbols; token_ptr->name; token_ptr++) {
		if (str_cmp (token_ptr->name, token) == 0) {
			return (token_ptr);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Special
//---------------------------------------------------------

Token_Data * Program_Code::Special (char *token)
{
	for (token_ptr = specials; token_ptr->name; token_ptr++) {
		if (str_cmp (token_ptr->name, token) == 0) {
			return (token_ptr);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Variable
//---------------------------------------------------------

Token_Data * Program_Code::Variable (char *token)
{
	for (token_ptr = variables.First (); token_ptr != NULL; token_ptr = variables.Next ()) {
		if (str_cmp (token_ptr->name, token) == 0) {
			return (token_ptr);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Token_Name
//---------------------------------------------------------

char * Program_Code::Token_Name (int type, int code)
{
	for (token_ptr = keywords; token_ptr->name; token_ptr++) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	for (token_ptr = specials; token_ptr->name; token_ptr++) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	for (token_ptr = symbols; token_ptr->name; token_ptr++) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Variable_Name
//---------------------------------------------------------

char * Program_Code::Variable_Name (int type, int code)
{
	for (token_ptr = variables.First (); token_ptr != NULL; token_ptr = variables.Next ()) {
		if (token_ptr->type == type && token_ptr->code == code) {
			return (token_ptr->name);
		}
	}
	return (NULL);
}
