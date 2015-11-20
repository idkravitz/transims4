//*********************************************************
//	Initial_Table.cpp - Interpret a Table
//*********************************************************

#include "User_Program.hpp"

//---------------------------------------------------------
//	Initial_Table
//---------------------------------------------------------

bool User_Program::Initial_Table (void)
{
	char *token_ptr;

	double fnum, *fptr;
	int i, j, lnum, *lptr;

	Svalue_Data *sptr, srec;

	i = cmd.token;
	j = cmd.value;

	for (;;) {
	
		while (*buf == ' ' && *buf != '\0') {
			buf++;
		}
		if (*buf == '\0') break;
	
		token_ptr = token;
	
		//---- determine the token type ----
	
		if ((*buf >= '0' && *buf <= '9') || (*buf == '.' && *(buf+1) != '.')) {
	
			//---- process a number ----
		
			do {
				*token_ptr++ = *buf++;
			} while ((*buf >= '0' && *buf <= '9') || (*buf == '.' && *(buf+1) != '.'));
			
			*token_ptr = '\0';

			//---- set the table size ----
		
			if (open_par) {
				lnum = atol (token);
					
				if (lnum < 1) {
					exe->Error ("%s Table %d Size %d is Out of Range", Data_Type (i), j, lnum);
					return (false);
				}
				if (table [i] [j].Num_Records () > 0) {
					exe->Write (1);
					exe->Warning ("%s Table %d is Redefined", Data_Type (i), j);
					table [i] [j].Reset ();
				}
				if (!table [i] [j].Max_Records (lnum)) goto mem_error;

			} else {

				if (table_type == STAB) {
					exe->Error ("Attempt to Assign a Number to a String Table");
					return (false);
				}

				//---- search for the value in the list ----

				if (table_type == ITAB) {
					lnum = atol (token);

					for (lptr = lvalue.First (); lptr != NULL; lptr = lvalue.Next ()) {
						if (*lptr == lnum) break;
					}
					if (lptr == NULL) {
						if (!lvalue.Add (lnum)) goto mem_error;
					}
					lnum = lvalue.Record_Index ();
				} else {
					fnum = atof (token);

					for (fptr = fvalue.First (); fptr != NULL; fptr = fvalue.Next ()) {
						if (*fptr == fnum) break;
					}
					if (fptr == NULL) {
						if (!fvalue.Add (fnum)) goto mem_error;
					}
					lnum = fvalue.Record_Index ();
				}
				table [i] [j].Add (lnum);
			}

		} else if (*buf == '"') {

			if (table_type != STAB) goto error;

			//---- process a string ----

			buf++;
			
			while (*buf != '"' && *buf != '\0') {
				*token_ptr++ = *buf++;
			}
			buf++; 
			*token_ptr = '\0';

			for (sptr = svalue.First (); sptr != NULL; sptr = svalue.Next ()) {
				if (!str_cmp (sptr->string, token)) break;
			}
			if (sptr == NULL) {
				srec.length = (int) strlen (token) + 1;
				srec.string = new char [srec.length];

				if (srec.string == NULL) goto mem_error;

				str_cpy (srec.string, srec.length, token);

				if (!svalue.Add (&srec)) goto mem_error;
			}
			lnum = svalue.Record_Index ();

			table [i] [j].Add (lnum);

		} else if ((*buf >= 'A' && *buf <= 'Z') || (*buf >= 'a' && *buf <= 'z')) {
				
			do {
				*token_ptr++ = *buf++;
			} while ((*buf >= 'A' && *buf <= 'Z') || (*buf >= 'a' && *buf <= 'z') ||
				(*buf >= '0' && *buf <= '9') || (*buf == '.' && *(buf+1) != '.') || *buf == '_');
			
			*token_ptr = '\0';
			
			if (!(str_cmp ("ENDTAB", token))) {
				table_flag = false;
				open_par = false;
				return (true);
			} else {
				goto error;
			}

		} else if (*buf == '(' || *buf == ')' || *buf == ',' || *buf == '=') {

			if (*buf == '(') {
				open_par = true;
			} else if (*buf == ')') {
				open_par = false;
			} else if (*buf == ',' || *buf == '=') {
				if (open_par) {
					goto error;
				}
			}
			buf++;

		} else {
			goto error;
		}
	}
	return (true);

error:
	return (Syntax_Error ("Table Declaration"));

mem_error:
	exe->Error ("Insufficient Memory for %s Table %d", Data_Type (i), j);
	return (false);
}
