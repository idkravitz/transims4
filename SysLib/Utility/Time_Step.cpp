//********************************************************* 
//	Time_Step.cpp - Time Step conversion utility
//*********************************************************

#include "Time_Step.hpp"

#include "Utility.hpp"

char Time_Step::buffer [2*TIME_BUFFER] = "";

//---- time type codes ----

char * Time_Step::time_text [] = {
	"HOURS", "24_HOUR_CLOCK", "12_HOUR_CLOCK", "SECONDS", "TIME_CODE", NULL
};

Time_Step::Time_Type  Time_Step::time_code [] = {
	HOURS, CLOCK24, CLOCK12, SECONDS, TIME_CODE
};

//---------------------------------------------------------
//	Time_Step constructor
//---------------------------------------------------------

Time_Step::Time_Step (int time_step, char *format) : Static_Service ()
{
	Initialize (time_step, format);
}

//---------------------------------------------------------
//	Time_Step constructor
//---------------------------------------------------------

Time_Step::Time_Step (int time_step, Time_Type format) : Static_Service ()
{
	Step_Size (time_step);
	Format (format);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Time_Step::Initialize (int time_step, char *format)
{
	Step_Size (time_step);
	return (Format (format));
}

//---------------------------------------------------------
//	Step_Size - set the steps per second
//---------------------------------------------------------

int  Time_Step::Step_Size (int _time_step)
{
	time_step = _time_step;

	return (time_step);
}

//---------------------------------------------------------
//	Format - set the time format code
//---------------------------------------------------------

bool Time_Step::Format (char *format)
{
	if (format != NULL) {
		for (int i=0; time_text [i] != NULL; i++) {
			if (str_cmp (format, time_text [i]) == 0) {
				Format (time_code [i]);
				return (true);
			}
		}
		if (Send_Messages ()) {
			exe->Error ("Converting Time Format Type %s", format);
		}
		return (false);
	} else {
		time_format = SECONDS;
		pm_flag = false;
	}
	return (true);
}

//---------------------------------------------------------
//	Format - set the time format code
//---------------------------------------------------------

void Time_Step::Format (Time_Type format)
{
	time_format = format;
	pm_flag = (time_format == CLOCK12);
}

//---------------------------------------------------------
//	Time_String - converts a string to Time Steps
//---------------------------------------------------------

int  Time_Step::Time_String (char *string, bool duration)
{
	//---- NOON ----
	//---- MIDNIGHT ----
	//---- d@hh:mm:ss.xAM/PM -----
	//---- d@hh:mm:ss.x -----
	//---- d@hh:mm -----
	//---- d@hh:mmAM/PM -----
	//---- d@hh.xxx ----
	//---- d@ssssss ----
	//---- hh:mm:ss -----
	//---- hh:mm:ss_AM/PM -----
	//---- hh:mm -----
	//---- hh:mmpm -----
	//---- hh.xxx ----
	//---- ssssss ----
	//---- dddhh:mm ----
	
	static char *day_text [] = {
		"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT", "WKE", "WKD", "ALL", NULL
	};

	int steps, hour, min_steps, hour_steps, noon_steps, day_steps;
	char *str_ptr, *ptr;

	if (time_step == 0) {
		if (Send_Messages ()) {
			exe->Error ("Time Step is Zero");
		}
		return (-1);
	}
	str_ptr = string;

	while (*str_ptr == ' ' || *str_ptr == '\t') {
		str_ptr++;
	}
	if (*str_ptr == '\0') goto time_error;

	str_cpy (buffer, sizeof (buffer), str_ptr);

	str_ptr = buffer;
	steps = 0;

	min_steps = time_step * 60;
	hour_steps = min_steps * 60;
	noon_steps = hour_steps * 12;
	day_steps = noon_steps * 2;

	//---- day ----

	ptr = strchr (str_ptr, '@');

	if (ptr != NULL) {
		*ptr++ = '\0';

		if (*str_ptr < '0' || *str_ptr > '9') goto time_error;

		steps = atoi (str_ptr) * day_steps;

		if (*ptr == '\0') return (steps);
		str_ptr = ptr;
	}

	//---- check for special codes ----

	if (*str_ptr < '0' || *str_ptr > '9') {

		if (str_cmp (str_ptr, "NOON") == 0) {
			return (steps + noon_steps);
		} 
		if (str_cmp (str_ptr, "MIDNIGHT") == 0) {
			return (steps);
	//		return (steps + day_steps);
		}

		//---- VERSION3 Time Codes ----

		for (int i=0; day_text [i] != NULL; i++) {
			if (str_cmp (str_ptr, day_text [i], strlen (day_text [i])) == 0) {
				str_ptr += strlen (day_text [i]);
				char *ptr = strchr (str_ptr, ':');
				if (ptr == NULL) {
					if (*str_ptr == '\0') {
						return (0);
					} else {
						return (atoi (str_ptr) * 3600);
					}
				} else {
					*ptr++ = '\0';
					return (atoi (str_ptr) * 3600 + atoi (ptr) * 60);
				}
			}
		}
	}

	//---- check time units ----

	ptr = strchr (str_ptr, ':');

	if (ptr == NULL) {
		if (duration) {
			if (time_format == HOURS) {
				if (*str_ptr < '0' || *str_ptr > '9' && *str_ptr != '.') goto time_error;

				steps += (int) (atof (str_ptr) * hour_steps + 0.5); 
			} else {
				if (*str_ptr < '0' || *str_ptr > '9') return (-1);

				steps += atoi (str_ptr) * time_step;
			}
		} else {
			if (*str_ptr < '0' || *str_ptr > '9' && *str_ptr != '.') goto time_error;

			if (time_format == SECONDS) {
				steps += (int) (atof (str_ptr) * time_step + 0.5); 
			} else if (time_format == HOURS) {
				steps += (int) (atof (str_ptr) * hour_steps + 0.5); 
			} else if (strchr (str_ptr, '.') != NULL) {
				steps += (int) (atof (str_ptr) * hour_steps + 0.5); 
			} else {
				int temp = atoi (str_ptr);

				if (temp < 50) {
					steps += temp * hour_steps;
				} else {
					steps += temp * time_step;
				}
			}
		}
		return (steps);
	}

	//---- hour of the day ----

	*ptr++ = '\0';
			
	if (*str_ptr < '0' || *str_ptr > '9') goto time_error;

	hour = atoi (str_ptr);

	steps += hour * hour_steps;

	if (*ptr == '\0') return (steps);
	str_ptr = ptr;

	//---- check for pm ----

	ptr = strchr (str_ptr, 'p');

	if (ptr == NULL) {
		ptr = strchr (str_ptr, 'P');
	}
	if (ptr != NULL) {
		*ptr = '\0';

		if (hour < 12) {
			steps += noon_steps;
		}
	} else {

		//---- check for am ----

		ptr = strchr (str_ptr, 'a');

		if (ptr == NULL) {
			ptr = strchr (str_ptr, 'A');
		}
		if (ptr != NULL) {
			*ptr = '\0';

			if (hour == 12) {
				steps -= noon_steps;
			}
		}
	}

	//---- minute of the hour ----

	ptr = strchr (str_ptr, ':');

	if (ptr != NULL) {
		*ptr++ = '\0';
	}
	if (*str_ptr < '0' || *str_ptr > '9') goto time_error;

	steps += atoi (str_ptr) * min_steps;
	
	if (ptr == NULL || *ptr == '\0') return (steps);
	str_ptr = ptr;

	//---- seconds of the minute ----
			
	if (*str_ptr < '0' || *str_ptr > '9') goto time_error;

	steps += (int) (atof (str_ptr) * time_step + 0.5);

	return (steps);

time_error:
	if (Send_Messages ()) {
		exe->Warning ("Converting Time String %s", string);
	}
	return (-1);
}

//---------------------------------------------------------
//	Format_Step - converts time steps to a time string
//---------------------------------------------------------

char * Time_Step::Format_Step (int step, char *buf)
{
	bool pm = false;
	int hour, minute, second, min_steps, hour_steps, noon_steps, day_steps;
    char string [TIME_BUFFER];

	min_steps = time_step * 60;
	hour_steps = min_steps * 60;
	noon_steps = hour_steps * 12;
	day_steps = noon_steps * 2;

	if (step >= MAX_INTEGER) {
		step = MIDNIGHT * time_step;
	} else if (step > 3 * MIDNIGHT * time_step) {
		step = 3 * MIDNIGHT * time_step;
	}
	buffer [0] = '\0';

	if (time_step == 0) return (NULL);

	if (time_format == HOURS) {
		str_fmt (buffer, sizeof (buffer), "%g", (double) step / hour_steps);
		if (strchr (buffer, '.') == NULL) {
			str_fmt (buffer, sizeof (buffer), "%.1lf", (double) step / hour_steps);
		}
		goto output;
	} else if (time_format == SECONDS) {
		if (time_step > 1) {
			str_fmt (buffer, sizeof (buffer), "%g", (double) step / time_step);
		} else {
			str_fmt (buffer, sizeof (buffer), "%d", step);
		}
		goto output;
	} else if (time_format == TIME_CODE) {
		minute = (step % hour_steps);
		str_fmt (buffer, sizeof (buffer), "ALL%02d:%02d", (step / hour_steps), (minute / 60));
		goto output;
	}

	//---- check for negative values ----

	if (step < 0) {
		buffer [0] = '-';
		buffer [1] = '\0';
		step = -step;
	}

	//---- clock formating ----

	if (pm_flag && step == 0) {
		str_cat (buffer, sizeof (buffer), "MIDNIGHT");
		goto output;
	} else if (step >= day_steps) {
		if (!pm_flag && step == day_steps) {
			str_cat (buffer, sizeof (buffer), "24:00");
			goto output;
		}
		str_cat (buffer, sizeof (buffer), str_fmt (string, sizeof (string), "%d@", step / day_steps));

		step %= day_steps;
		if (pm_flag && step == 0) {
			str_cat (buffer, sizeof (buffer), "MIDNIGHT");
			goto output;
		}
	}
	if (pm_flag && step >= noon_steps) {
		pm = true;
		step -= noon_steps;
		if (step == 0) {
			str_cat (buffer, sizeof (buffer), "NOON");
			goto output;
		}
	}
	hour = step / hour_steps;
	step -= hour * hour_steps;
	minute = step / min_steps;
	step -= minute * min_steps;

	if (pm_flag && hour == 0) {
		hour = 12;
	}
	str_cat (buffer, sizeof (buffer), str_fmt (string, sizeof (string), "%d:%02d", hour, minute));

	if (step > 0) {
		second = step / time_step;
		step -= second * time_step;

		str_cat (buffer, sizeof (buffer), str_fmt (string, sizeof (string), ":%02d", second));

		if (step > 0) {
			step = step * 1000 / time_step;
			if ((step % 10) == 0) {
				step /= 10;
				if ((step % 10) == 0) {
					str_fmt (string, sizeof (string), ".%d", step / 10);
				} else {
					str_fmt (string, sizeof (string), ".%02d", step);
				}
			} else {
				str_fmt (string, sizeof (string), ".%03d", step);
			}
			str_cat (buffer, sizeof (buffer), string);
		}
	}
	if (pm_flag) {
		if (pm) {
			str_cat (buffer, sizeof (buffer), "PM");
		} else {
			str_cat (buffer, sizeof (buffer), "AM");
		}
	}

output:
	if (buf != NULL) {
		str_cpy (buf, TIME_BUFFER, buffer, TIME_BUFFER-1);
		if (buffer [0] == '\0') return (NULL);
		return (buf);
	} else {
		if (buffer [0] == '\0') return (NULL);
		return (buffer);
	}
}

//---------------------------------------------------------
//	Format_Duration - converts step duration to a time string
//---------------------------------------------------------

char * Time_Step::Format_Duration (int step, char *buffer)
{
	bool flag;
	char *result;

	flag = pm_flag;
	pm_flag = false;

	result = Format_Step (step, buffer);

	pm_flag = flag;
	return (result);
}

//---------------------------------------------------------
//	Time_Code
//---------------------------------------------------------

Time_Step::Time_Type  Time_Step::Time_Code (char *text)
{
	for (int i=0; time_text [i] != NULL; i++) {
		if (str_cmp (text, time_text [i]) == 0) {
			return (time_code [i]);
		}
	}
	if (Send_Messages ()) {
		exe->Error ("Converting Time Format Type %s", text);
	}
	return (SECONDS);
}

char * Time_Step::Time_Code (Time_Step::Time_Type code)
{
	for (int i=0; time_text [i] != NULL; i++) {
		if (code == time_code [i]) {
			return (time_text [i]);
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Time_Label - converts a label to Time Steps
//---------------------------------------------------------

int  Time_Step::Time_Label (char *string)
{
	int len;
	char *str_ptr, label [8];
	bool flag;

	str_ptr = string;

	while (*str_ptr == ' ' || *str_ptr == '\t') {
		str_ptr++;
	}
	if (*str_ptr != '\0') {
		if (strchr (str_ptr, '.') == NULL && strchr (str_ptr, ':') == NULL) {
			len = (int) strlen (str_ptr);
				
			//---- scan for clock time ----

			if (len >= 3 && len <= 4) {
				flag = false;

				if (str_ptr [0] == '0') {
					flag = true;
				} else if (len == 4) {
					if (str_ptr [0] <= '2' && str_ptr [2] <= '5') {
						flag = true;
					}
				} else if (str_ptr [1] <= '5') {
					flag = true;
				}
				if (flag) {
					if (len == 4) {
						label [0] = str_ptr [0];
						label [1] = str_ptr [1];
						label [2] = ':';
						label [3] = str_ptr [2];
						label [4] = str_ptr [3];
						label [5] = '\0';
					} else {
						label [0] = str_ptr [0];
						label [1] = ':';
						label [2] = str_ptr [1];
						label [3] = str_ptr [2];
						label [4] = '\0';
					}
					str_ptr = label;
				}
			}
		}
	}
	return (Time_String (str_ptr, false));
}

char * Time_Step::Time_Label (int step, bool pad_flag)
{
	int hour, minute, second, min_steps, hour_steps;

	min_steps = time_step * 60;
	hour_steps = min_steps * 60;

	if (time_step == 0) return (NULL);

	if (step >= MAX_INTEGER) {
		step = MIDNIGHT * time_step;
	} else if (step > 3 * MIDNIGHT * time_step) {
		step = 3 * MIDNIGHT * time_step;
	}
	if (time_format == HOURS) {
		if (pad_flag) {
			return (str_fmt (buffer, sizeof (buffer), "%09.6lf", (double) step / hour_steps));
		} else {
			return (str_fmt (buffer, sizeof (buffer), "%g", (double) step / hour_steps));
		}
	} else if (time_format == SECONDS) {
		if (time_step > 1) {
			if (pad_flag) {
				return (str_fmt (buffer, sizeof (buffer), "%08.2lf", (double) step / time_step));
			} else {
				return (str_fmt (buffer, sizeof (buffer), "%g", (double) step / time_step));
			}
		} else {
			if (pad_flag) {
				return (str_fmt (buffer, sizeof (buffer), "%05d", step));			
			} else {
				return (str_fmt (buffer, sizeof (buffer), "%d", step));
			}
		}
	}
	hour = step / hour_steps;
	step -= hour * hour_steps;
	minute = step / min_steps;
	step -= minute * min_steps;
	second = step / time_step;
	step -= second * time_step;

	if (pad_flag) {
		if (time_step > 1) {
			if (((1000 / time_step) % 10) == 0) {
				if ((100 / time_step % 10) == 0) {
					str_fmt (buffer, sizeof (buffer), "%02d%02d%02d%d", hour, minute, second, (step * 10 / time_step));
				} else {
					str_fmt (buffer, sizeof (buffer), "%02d%02d%02d%02d", hour, minute, second, (step * 100 / time_step));
				}
			} else {
				str_fmt (buffer, sizeof (buffer), "%02d%02d%02d%03d", hour, minute, second, (step * 1000 / time_step));
			}
		} else {
			str_fmt (buffer, sizeof (buffer), "%02d%02d%02d", hour, minute, second);
		}
	} else if (step > 0) {
		step = step * 1000 / time_step;
		if ((step % 10) == 0) {
			step /= 10;
			if ((step % 10) == 0) {
				str_fmt (buffer, sizeof (buffer), "%d%02d%02d%d", hour, minute, second, step / 10);
			} else {
				str_fmt (buffer, sizeof (buffer), "%d%02d%02d%02d", hour, minute, second, step);
			}
		} else {
			str_fmt (buffer, sizeof (buffer), "%d%02d%02d%03d", hour, minute, second, step);
		}
	} else if (second > 0) {
		str_fmt (buffer, sizeof (buffer), "%d%02d%02d", hour, minute, second);
	} else {
		str_fmt (buffer, sizeof (buffer), "%d%02d", hour, minute);
	}
	return (buffer);
}
