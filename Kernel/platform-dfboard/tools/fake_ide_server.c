#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "/home/david/projects/boardio/boardio.h"

//gcc -Wall fake_ide_server.c -lboardio && ./a.out ; echo $?

#define VERBOSE

#define IMAGE "filesystem-dfboard.img"
FILE *fp;


void closeimg(void)
{
	if (fp) {
		fclose(fp);
		fp = NULL;
	}
}

void openimg(void)
{
	closeimg();
	fp = fopen(IMAGE, "r+");
	if (!fp) {
		fprintf(stderr, "cannot open image '%s'\n", IMAGE);
		exit(16);
	}
}

int main(void)
{
	unsigned char cmd, sector[512];
	unsigned int addr;
	long offset;

	setenv(BOARDIO_ENV_DEVICE, "/dev/boardio2", 1);
	setenv(BOARDIO_ENV_RTSCTS, "2", 1);
	setenv(BOARDIO_ENV_SPEED,  "1000000", 1);
	setenv(BOARDIO_ENV_STOPB,  "1", 1);
	setenv(BOARDIO_ENV_SYNC,   "0", 1);

	if (!boardio_acquire(0))
		return 1;

	if (!boardio_open(0))
		return 2;

	for (;;) {
		if (!boardio_read(&cmd, 1))
			return 3;
		if (cmd == 'Z') {
			openimg();
			if (!boardio_write("U\0\377", 3)) /* sync stream */
				return 5;
#ifdef VERBOSE
			printf("\x1B[2J%c\n", cmd); fflush(stdout);
#endif
			continue;
		}
#ifdef VERBOSE
		printf("%c", cmd); fflush(stdout);
#endif
		if (!fp)
			continue;
		if (cmd == 'R' || cmd == 'W') {
			if (!boardio_read(&sector, 3))
				return 7;
			addr = sector[0]<<(16+0) | sector[1]<<(8+0) | sector[2]<<(0+0);
#ifdef VERBOSE
			printf("%06X ", addr); fflush(stdout);
#endif
			offset = (long)addr << 9;
			if (fseek(fp, offset, SEEK_SET) != 0)
				return 8;
		}
		else
			return 9;
		if (cmd == 'R') {
			if (fread(sector, 512, 1, fp) != 1)
				return 10;
			if (!boardio_write(sector, 512))
				return 11;
		}
		else if (cmd == 'W') {
			if (!boardio_read(sector, 512))
				return 12;
			if (fwrite(sector, 512, 1, fp) != 1)
				return 13;
			if (fflush(fp))
				return 14;
		}
		else
			return 15;
#ifdef VERBOSE
		printf("%02X%02X%02X%02X%02X%02X%02X%02X\n", sector[0], sector[1], sector[2], sector[3], sector[4], sector[5], sector[6], sector[7]);
#endif
	}

	return 0;
}
