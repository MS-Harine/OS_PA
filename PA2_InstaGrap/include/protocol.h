#ifndef	__PROTOCOL_H__
#define	__PROTOCOL_H__

#define	BUILD_SUCCESS	1
#define	BUILD_FAILED	2

#define	TEST_TIMEOUT	3
#define	TEST_FAILED		4
#define	TEST_SUCCESS	5
#define	TEST_PROCESS	6

#define	LOGIN_SUCCESS	1
#define	LOGIN_FAILED	2

#define READ_END	0
#define WRITE_END	1
#define TRUE	1
#define	FALSE	0
#define	BUF_SIZE		1024

#define	FREE(x)	free(x); x = NULL;

#ifdef	DEBUG
#define	DPRINT(func)	func
#else
#define	DPRINT(func)	;
#endif

union _type_caster {
	int length;
	char data[4];
};

#endif
