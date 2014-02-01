/*
 * Code reference from author: aacczury(https://github.com/aacczury)
 * Source code: https://github.com/aacczury/qemu-semihost/blob/master/hello.c
 */

#include <stdio.h>
#include <inttypes.h>

#define READ_COUNTER_ADDR 0x40050000

/* File open mode macro corresponding to ISO C fopen() mode */
#define O_RDONLY	(0)			/* 0: r: Read only */
#define O_RDWR		(2)			/* 2: r+: Read and write */
#define O_WDONLY	(4)			/* 4: w: Create file and Write only 
								   (The old file will be discarded.) */
#define O_CREAT		(4)
#define O_APPEND	(8)			/* 8: a: Append at the end of file */

/* Semihost system call */
enum SemihostReasons {
	/* Standard ARM Semihosting commands: */
	Semihost_SYS_OPEN	= 0x01,		/* Open a file on the host. */
	Semihost_SYS_CLOSE	= 0x02,		/* Close a file on the host. */
	Semihost_SYS_WRITE	= 0x05,		/* Write to a file on the host. */
	Semihost_SYS_READ	= 0x06,		/* Read the contents of a file into buffer. */
};

/* Prevent the compiler generating any function entry and exist code. */
int SemihostCall( enum SemihostReasons reason, void *arg ) __attribute__ ((naked));
int SemihostCall( enum SemihostReasons reason, void *arg )
{
	__asm__( \
			"BKPT	0xAB	\n" \
			"nop			\n" \
			"BX		LR		\n" \
			:::	\
			);
}

/*
 * Return the Length of the string.
 * Reference from rtenv.
 */
size_t strlen(const char *s) __attribute__ ((naked));
size_t strlen(const char *s)
{
	__asm__( \
		"	sub  r3, r0, #1			\n"	\
        "strlen_loop:               \n"	\
		"	ldrb r2, [r3, #1]!		\n"	\
		"	cmp  r2, #0				\n"	\
        "   bne  strlen_loop        \n"	\
		"	sub  r0, r3, r0			\n"	\
		"	bx   lr					\n"	\
		:::	\
	);
}

/* SemihostCall parameter structure */
union param_t {
	int		pdInt;
	void	*pdPtr;
	char	*pdChrPtr;
};

/* Open a file on the host */
static int open( const char *path, int flag )
{
	union param_t param[3] = {0};

	/* 
	 * a three-word argument block:
	 * Word 1: A pointer to the file of divice name.
	 * Word 2: An integer that specifies the file opening mode.
	 * Word 3: An integer that give the length of the string pointed to by
	 * 		Word 1.
	 */
	param[0].pdChrPtr = path;
	param[1].pdInt = flag;
	param[2].pdInt = strlen( path );

	/*
	 * a nozero handle if the call is successful
	 * -1 if the call is failed
	 */
	return SemihostCall( Semihost_SYS_OPEN, param );
}

/* Close a file on the host */
static int close( int fd )
{
	union param_t param[1] = {0};

	/* Word 1: A handle for an open file. */
	param[0].pdInt = fd;

	/* 0 if the call is successful; Otherwise, -1. */
	return SemihostCall( Semihost_SYS_CLOSE, param );
}

int main(void)
{
	char *filename = "test.dat";
	int fd, result;

	if ( ( fd = open( filename, O_RDWR ) ) == -1 )
		printf( "Failed in opening-file call\n" );
	else
	{
		printf( "Success in opening-file call: %d\n", fd );
		result = close( fd );
		printf( "%d\n", result );
	}

	return 0;
}
