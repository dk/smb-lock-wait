#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static void usage(void)
{
	printf("smb_wait_lock - wait for samba file freed from locking, and exit\n");
	printf("\n");
	printf("format: smb_wait_lock [options] filename\n");
	printf("options:\n");
	printf("   -h          - this help\n");
	printf("   -v          - be verbose\n");
	printf("   -t TIMEOUT  - wait for TIMEOUT seconds, or indefinitely\n");
	printf("\n");
	printf("exit code: 0 - success, 1 - failure\n");
	exit(1);
}

int
main( int argc, char ** argv )
{
	int timeout = 0;
	int verbose = 0;
	int c;

	int file;

	char * endptr;
	char * filename;

	while ((c = getopt(argc, argv, "hvt:")) != -1) {
		switch (c) {
		case 'v':
			verbose = 1;
			break;
		case 't':
			timeout = (int) strtol(( const char *) optarg, &endptr, 10);
			if ( *endptr != 0 || timeout < 0 ) usage();
			if (timeout > 0) alarm(timeout);
			break;
		default:
			usage();
		}
	}

	if (optind == argc) usage();

	if ( geteuid() != 0 ) {
		fprintf(stderr, "Must be root\n");
		exit(1);
	}
			
	filename = argv[optind];
	if ( verbose ) fprintf(stderr, "open %s\n", filename);
	file = open(filename, O_RDONLY);
	if ( file < 0 ) {
		fprintf(stderr, "error opening %s: %s\n", filename, strerror(errno));
		exit(1);
	}

	while ( 1 ) {
		int r = fcntl( file, F_SETLEASE, F_RDLCK);
		if ( r < 0 ) {
			if ( errno == EAGAIN ) {
				if ( verbose ) fprintf(stderr, "sleeping...\n");
				usleep(1000000);
				continue;
			} else {
				fprintf(stderr, "error setting lease on %s: %s\n", filename, strerror(errno));
				close(file);
				exit(1);
			}
		} else {
			if ( verbose ) fprintf(stderr, "lease acquired\n");
			fcntl( file, F_SETLEASE, F_UNLCK);
			break;
		}
	}

	close(file);

	return 0;
}
