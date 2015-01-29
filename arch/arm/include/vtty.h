

#ifndef _VTTY_H_
#define _VTTY_H_

#define VTTY_BUFFER_SIZE 1024
struct vtty_s {
	unsigned int end;
	char buffer[VTTY_BUFFER_SIZE];
};


#endif
