//********************************************************* 
//	Get_Token.cpp - reads a token from the command string
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Get_Token
//---------------------------------------------------------

bool User_Program::Get_Token (void)
{
	char *ptr;
	int lnum, *lptr;
	double fnum, *fptr;
	int i, j;

	Svalue_Data *sptr, srec;
	Svalue_Index *sindex, index_rec;

	static int type_map [] = {
		INTEGER, INTEGER, FLOAT, FLOAT, STRING, STRING, INTEGER, INTEGER, INTEGER, FLOAT
	};

	//---- remove leading spaces ----

	while (*buf == ' ' || *buf == '\t') {
		buf++;
	}
	if (*buf == '\0') return (false);
	
	ptr = token;
	
	//---- determine the token type ----
	
	if ((*buf >= '0' && *buf <= '9') || 
		(*buf == '.' && *(buf+1) != '.') || *buf == ':') {
	
		//---- process a number ----
		
		do {
			*ptr++ = *buf++;
		} while ((*buf >= '0' && *buf <= '9') || 
			(*buf == '.' && *(buf+1) != '.') || *buf == ':');
			
		*ptr = '\0';
		ptr = token;
	
		//---- check for decimal point ----
		
		cmd.type = LVALUE;
		cmd.token = 0;

		while (*ptr) {
			if (*ptr == ':') {
				cmd.token = 1;
				*ptr++ = '\0';
				break;
			} else if (*ptr++ == '.') {
				cmd.type = FVALUE;
				break;
			}
		}
		
		//---- store the number ----
			
		if (cmd.type == LVALUE) {
		
			//---- integer number ----

			if (cmd.token == 1) {
				lnum = (atol (token) << 16) + atol (ptr);
				cmd.token = 0;
			} else {
				lnum = atol (token);
			}

			//---- search for the number in the list ----

			for (lptr = lvalue.First (); lptr != NULL; lptr = lvalue.Next ()) {
				if (*lptr == lnum) {
					cmd.value = lvalue.Record_Index ();
					break;
				}
			}
			if (lptr == NULL) {
				if (!lvalue.Add (lnum)) goto mem_error;
				cmd.value = lvalue.Record_Index ();
			}
			
		} else {
		
			//---- floating point number ----
		
			fnum = atof (token);
			
			for (fptr = fvalue.First (); fptr != NULL; fptr = fvalue.Next ()) {
				if (*fptr == fnum) {
					cmd.value = fvalue.Record_Index ();
					break;
				}
			}
			if (fptr == NULL) {
				if (!fvalue.Add (fnum)) goto mem_error;
				cmd.value = fvalue.Record_Index ();
			}
		}
		return (true);
		
	} else if (*buf == '"') {
	
		//---- process a string ----
		
		buf++;
		
		while (*buf != '"' && *buf != '\0') {
			*ptr++ = *buf++;
		}
		buf++; 
		*ptr = '\0';
			
		cmd.type = SVALUE;
		cmd.token = 0;

		for (sptr = svalue.First (); sptr != NULL; sptr = svalue.Next ()) {
			if (!strcmp (sptr->string, token)) {
				cmd.value = svalue.Record_Index ();
				break;
			}
		}
		if (sptr == NULL) {
			srec.length = (int) strlen (token) + 1;
			srec.string = new char [srec.length];

			if (srec.string == NULL) goto mem_error;

			str_cpy (srec.string, srec.length, token);

			if (!svalue.Add (&srec)) goto mem_error;

			cmd.value = svalue.Record_Index ();
		}
		return (true);
	
	} else if ((*buf >= 'A' && *buf <= 'Z') || (*buf >= 'a' && *buf <= 'z')) {
	
		//---- process a variable or command ---- 
		
		do {
			*ptr++ = *buf++;
		} while ((*buf >= 'A' && *buf <= 'Z') || 
			(*buf >= 'a' && *buf <= 'z') ||
			(*buf >= '0' && *buf <= '9') || 
			(*buf == '.' && *(buf+1) != '.') || *buf == '_');
			
		*ptr = '\0';

		//---- check for a special variable name ----

		token_ptr = Special (token);

		if (token_ptr != NULL) {
			if (token_ptr->type == DECLARE) {
				declare_flag = true;
				declare_type = token_ptr->code;
				return (Initial_Declare ());
			}
			if (token_ptr->type == TABLE) {
				cmd.type = token_ptr->type;
				cmd.token = (token_ptr->code % 10);
				cmd.value = token_ptr->code / 10;

				if (table [cmd.token] [cmd.value].Num_Records () == 0) {
					table_flag = true;
					table_type = cmd.token;
					Initial_Table ();
					return (false);
				}
			}
			return (true);
		}
		
		//---- search for a keyword command ----

		token_ptr = Keyword (token);

		if (token_ptr != NULL) {
			cmd.type = token_ptr->type;
			cmd.token = token_ptr->code;

			//---- check for a variable name conflict ----

			if (cmd.type == FUNCTION || cmd.type == CONVERT || cmd.type == DATETIME || cmd.type == IN_OUT) {
				if (*buf == '(' || (*buf == ' ' && *(buf+1) == '(')) {
					if (cmd.token == FWRITE) {
						ascii_write = true;
					}
					return (true);
				}
			} else {
				return (true);
			}
		}

		//---- check for a file label ----

		ptr = strchr (token, '.');
		
		if (ptr != NULL) {
			*ptr++ = '\0';

			for (i=0; i < num_files; i++) {
				if (str_cmp (file [i]->File_ID (), token) == 0) break;
			}
			if (i >= num_files) {
				exe->Error ("File Name \"%s\" was Not Found", token);
				return (false);
			}
			j = file [i]->Field_Number (ptr);

			if (j == 0) {
				exe->Error ("File \"%s\" Field \"%s\" was Not Found", token, ptr);
				return (false);
			}

			Db_Field *fld = file [i]->Field (j);

			cmd.type = type_map [fld->Type ()];
			cmd.token = i;
			cmd.value = j;

			if (cmd.type == STRING) {
				int code = j * num_files + i;

				sindex = svalue_index.Get (code);

				if (sindex == NULL) {
					srec.length = fld->Size () + 1;

					srec.string = new char [srec.length];

					if (srec.string == NULL) goto mem_error;
					srec.string [0] = '\0';

					if (!svalue.Add (&srec)) goto mem_error;

					index_rec.code = code;
					index_rec.index = svalue.Record_Index ();

					if (!svalue_index.Add (&index_rec)) goto mem_error;
				}
			}
			return (true);
		} 

		//---- check for a variable name ----

		token_ptr = Variable (token);

		if (token_ptr != NULL) {
			cmd.type = token_ptr->type;
			cmd.token = 1;
			cmd.value = token_ptr->code;
			return (true);
		}

		//---- check for a file label ----

		for (i=0; i < num_files; i++) {
			if (!str_cmp (file [i]->File_ID (), token)) {
				cmd.type = DATA;
				cmd.token = i;

				if (ascii_write) {
					file_stat [i] |= 2;
					ascii_write = false;
						
					if (read_only [i]) {
						exe->Error ("File \"%s\" is Read Only", file [i]->File_ID ());
						return (false);
					}
				}
				return (true);
			}
		}

		//---- check for a special variable name ----
		
		if (!str_cmp ("RECORD", token)) {
			cmd.type = RECORD;
			cmd.token = 0;
			return (true);
		} 

	} else if (*buf == '(' || *buf == ')' || *buf == ',' || 
		(*buf == '.' && *(buf+1) == '.')) {
	
		//---- process a parenthesis and comma ----
	
		cmd.type = LIMIT;
		if (*buf == '.') {
			cmd.token = RANGE_DOT;
			buf++;
		} else if (*buf == ',') {
			cmd.token = COMMA;
		} else {
			cmd.token = (*buf == '(') ? OPEN_PAR : CLOSE_PAR;
		}
		buf++;
			
		return (true);
		
	} else {
	
		//---- process a symbolic command ----

		do {
			*ptr++ = *buf++;
		} while (!((*buf >= 'A' && *buf <= 'Z') || (*buf >= 'a' && *buf <= 'z') ||
			(*buf >= '0' && *buf <= '9') || (*buf == '.' && *(buf+1) != '.') || 
			*buf == ' ' || *buf == '(' || *buf == ')' || *buf == '\0'));
			
		*ptr = '\0';
	
		token_ptr = Symbol (token);

		if (token_ptr != NULL) {
			cmd.type = token_ptr->type;
			cmd.token = token_ptr->code;
			return (true);
		}	
	}
	exe->Error ("Token \"%s\" was Not Recognized", token);
	return (false);

mem_error:
	exe->Error ("Insufficient Memory for Token Processing");
	return (false);
}
