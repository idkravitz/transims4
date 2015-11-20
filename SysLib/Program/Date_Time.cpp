//********************************************************* 
//	Date_Time.cpp - date-time functions
//*********************************************************

#include "User_Program.hpp"

#include "Date_Time.hpp"

//---------------------------------------------------------
//	Data_Time
//---------------------------------------------------------

bool User_Program::Date_Time (void)
{
	time_t tim;
	struct tm dt, *dtim;
	Stack_Data *s2, *s3, *s4, *s5;

	//--- time stamp function ----

	if (cmd_ptr->token == TIME_STAMP) {
		if (s->type != STRING_DATA) {
			exe->Error ("Time_Stamp does not support %s Data", Data_Type (s->type));

			s->type = INT_DATA;
			s->lvalue = (int) Convert_Time_String (s->svalue);
			return (true);
		}
	}

	//---- other functions ----

	if (s->type != INT_DATA) goto error;

	tim = (time_t) s->lvalue;

	switch (cmd_ptr->token) {
		case DOW:
			s->lvalue = Day_of_Week (tim);
			break;
			
		case HOUR:
			s->fvalue = Hour_of_Day (tim);
			s->type = FLOAT_DATA;
			break;

		case MINUTE:
			dtim = local_time (&tim);

			s->fvalue = (double) dtim->tm_min + (dtim->tm_sec / 60.0);
			s->type = FLOAT_DATA;
			break;
		
		case SECOND:
			dtim = local_time (&tim);
			s->lvalue = dtim->tm_sec;
			break;

		case DAY:
			dtim = local_time (&tim);
			s->lvalue = dtim->tm_mday;
			break;

		case MONTH:
			dtim = local_time (&tim);
			s->lvalue = dtim->tm_mon + 1;
			break;

		case YEAR:
			dtim = local_time (&tim);
			s->lvalue = dtim->tm_year + 1900;
			break;
			
		case DATE_STR:
			str_cpy (token, sizeof (token), Date_String (tim));

			s->type = STRING_DATA;
			s->length = sizeof (token);
			s->svalue = token;
			break;
			
		case TIME_STR:
			str_cpy (token, sizeof (token), Time_String (tim));

			s->type = STRING_DATA;
			s->length = sizeof (token);
			s->svalue = token;
			break;

		case DATE:

			if (sindex <= 2) goto error;

			s2 = stack [sindex - 2];

			if (s->type != INT_DATA || s1->type != INT_DATA) goto error;

			if (s2->lvalue < 1900) {
				dt.tm_year = s2->lvalue;
			} else {
				dt.tm_year = s2->lvalue - 1900;
			}
			dt.tm_mon = s1->lvalue - 1;
			dt.tm_mday = s->lvalue;
			dt.tm_hour = 0;
			dt.tm_min = 0;
			dt.tm_sec = 0;
			dt.tm_isdst = -1;

			sindex -= 2;
			s = s2;
			s->lvalue = (int) mktime (&dt);
			break;

		case TIME:

			if (sindex <= 2) goto error;

			s2 = stack [sindex - 2];

			if (s->type != INT_DATA || s1->type != INT_DATA) goto error;

			dt.tm_year = 70;
			dt.tm_mon = 0;
			dt.tm_mday = 1;
			dt.tm_hour = s2->lvalue;
			dt.tm_min = s1->lvalue;
			dt.tm_sec = s->lvalue;
			dt.tm_isdst = -1;

			sindex -= 2;
			s = s2;
			s->lvalue = (int) mktime (&dt);
			break;

		case DATE_TIME:

			if (sindex <= 5) goto error;

			s2 = stack [sindex - 2];
			s3 = stack [sindex - 3];
			s4 = stack [sindex - 4];
			s5 = stack [sindex - 5];

			if (s5->type != INT_DATA || s4->type != INT_DATA ||
				s3->type != INT_DATA || s2->type != INT_DATA ||
				s1->type != INT_DATA) goto error;

			if (s5->lvalue < 1900) {
				dt.tm_year = s5->lvalue;
			} else {
				dt.tm_year = s5->lvalue - 1900;
			}
			dt.tm_mon = s4->lvalue - 1;
			dt.tm_mday = s3->lvalue;
			dt.tm_hour = s2->lvalue;
			dt.tm_min = s1->lvalue;
			dt.tm_sec = s->lvalue;
			dt.tm_isdst = -1;

			sindex -= 5;
			s = s5;
			s->lvalue = (int) mktime (&dt);
			break;

		default:
			exe->Error ("Date-Time Token %d was Unrecognized", cmd_ptr->token);
			return (false);
	}
	return (true);

error:
	return (Syntax_Error ("Data-Time Function"));
}
