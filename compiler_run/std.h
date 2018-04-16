// This file: virtual_machine built in functions declarations.
// For interpreted file to include, not compiler implementation.

__built_in int scanf(char const*, ...);
__built_in int printf(char const*, ...);
__built_in void assert(int);
__built_in void* malloc(int);
__built_in void free(void*);
__built_in void* memset(void*, int, int);
__built_in void* memcpy(void*, void const*, int);
__built_in int strcmp(char const*, char const*);
__built_in int strlen(char const*);
__built_in int puts(char const*);
__built_in int system(char const*);
__built_in int print_int(int);
__built_in int print_double(double);

#define va_list char*
#define va_start(ap, v) ((ap) = (va_list) &v + sizeof(v))
#define va_arg(ap, t) (*(t*)(((ap)=(ap)+sizeof(t))-sizeof(t)))
#define va_end(ap) ((ap) = 0)
