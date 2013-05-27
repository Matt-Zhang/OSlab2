#include "string.h"
#include "assert.h"
#include "common.h"

/*
 * use putchar_func to print a string
 *   @putchar_func is a function pointer to print a character
 *   @format is the control format string (e.g. "%d + %d = %d")
 *   @data is the address of the first variable argument
 * please implement it.
 */
int
vfprintf(void (*putchar_func)(char), const char *format, void **data) {
	const char *s;
	/*for (s = "please implement me\n"; *s; s ++) {
		putchar_func(*s);
	}
	return 0;
	*/
	int count = 0;
	char *str;
	for(s = format; *s; s++){
		if(*s != '%'){
			count++;
			putchar_func(*s);
		}
		else{
			s++;
			switch(*s){
				case 'd':{
					str = itoa((int )*data, 10);
					while(*str){
						putchar_func(*str);
						count++;
						str++;
					}
					break;
				}
				case 'x':{
					str = utoa((int )*data, 16);
					putchar_func('0');
					putchar_func('x');
					while(*str){
						putchar_func(*str);
						count++;
						str++;
					}
					break;
				}
				case 'c':{
					putchar_func((int )*data);
					count++;
					break;
					}
				case 's':{
					str = (char *)*data;
					while(*str){
						putchar_func(*str);
						str++;
						count++;
					}
					break;
				}
				default:
						assert(0);
						break;
			}
			data++;
		}
	}
	return count;
}

