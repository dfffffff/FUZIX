#include "../config.h"
#ifdef CONFIG_IDE_ON_TTY
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <boardio.h>


//#define VERBOSE
//#define FAST
//#define SLOW

#define FUSIX_IMAGE "Kernel/fuzix.bin"
#define DISK_IMAGE "filesystem-dfboard.img"
//#define IMAGE "Standalone/filesystem-src/fuzixfs.dsk"

void fatal(int code);
FILE *fp;


#ifdef CONFIG_IDE_ON_TTY
int tty = -1;
struct termios oldt;

void close_tty(void)
{
	if (tty != -1) {
		tcsetattr(tty, TCSANOW, &oldt);
		close(tty);
		tty = -1;
	}
}

void sighandler(int sig)
{
	fatal(sig+128);
}

int open_tty(void)
{
	int newtty;
	struct termios newt;
	signal(SIGINT, sighandler);
	signal(SIGTERM, sighandler);
	signal(SIGQUIT, sighandler);
	newtty = open("/dev/tty", O_RDWR | O_CLOEXEC | O_NOCTTY);
	if (newtty != -1) {
		if (!tcgetattr(newtty, &oldt)) {
			newt = oldt;
			cfmakeraw(&newt);
			newt.c_oflag |= OPOST | ONLCR;
			newt.c_lflag |= ISIG;
			if (!tcsetattr(newtty, TCSANOW, &newt)) {
				tty = newtty;
				return newtty;
			}
		}
		close(newtty);
	}
	return -1;
}
#endif


void fatal(int code)
{
#ifdef CONFIG_IDE_ON_TTY
	close_tty();
	if (code >= 128)
		fprintf(stderr, "received signal %d\n", code-128);
	else
#endif
	if (code)
		fprintf(stderr, "exited at location %d\n", code);
	exit(code);
}

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
	fp = fopen(DISK_IMAGE, "r+");
	if (!fp) {
		fprintf(stderr, "cannot open image '%s'\n", DISK_IMAGE);
		fatal(100);
	}
}

int main(int argc, char **argv)
{
	unsigned char cmd, sector[512];
	unsigned int addr;
	long offset;
#ifdef CONFIG_IDE_ON_TTY
	char chr;
	fd_set fdset;
	int boardfd, ttyfd, ret, nfds;
#endif

#ifndef CONFIG_IDE_ON_TTY
	setenv(BOARDIO_ENV_DEVICE, "/dev/boardio2", 1);
//	setenv(BOARDIO_ENV_SPEED,  "2000000", 1); /* 32 MHz */
//	setenv(BOARDIO_ENV_SPEED,  "1500000", 1); /* 24 MHz */
	setenv(BOARDIO_ENV_SPEED,  "1000000", 1); /* 16 MHz */
//	setenv(BOARDIO_ENV_SPEED,  "921600", 1); /* 14.7456 MHz */
//	setenv(BOARDIO_ENV_SPEED,  "750000", 1); /* 12 MHz */
//	setenv(BOARDIO_ENV_SPEED,  "500000", 1); /* 8 MHz */
	setenv(BOARDIO_ENV_RTSCTS, "2", 1);
	setenv(BOARDIO_ENV_STOPB,  "1", 1);
	setenv(BOARDIO_ENV_SYNC,   "0", 1);
#endif

	if (!boardio_acquire(0))
		fatal(1);

	if (!boardio_open(0))
		fatal(2);

#ifdef CONFIG_IDE_ON_TTY
	if (!(argc == 2 && !strcmp(argv[1], "--resume")))
	{
		if (!boardio_reset())
			fatal(3);

#ifdef FAST
		chr = 0xF8;
		if (!boardio_writemem(0xFF80, 1, &chr))
			fatal(4);
#endif
#ifdef SLOW
		chr = 0xD8;
		if (!boardio_writemem(0xFF80, 1, &chr))
			fatal(4);
#endif

		if (!boardio_load(FUSIX_IMAGE, BOARDIO_EXEC, NULL))
			fatal(5);
	}
	else
		openimg();

	boardfd = boardio_getfd(BOARDIO_GETFD_DAT);
	if (boardfd == -1)
		fatal(6);

	ttyfd = open_tty();
	if (ttyfd == -1)
		fatal(7);

	nfds = boardfd;
	if (ttyfd > nfds)
		nfds = ttyfd;
	nfds++;
	FD_ZERO(&fdset);
#endif

	for (;;) {
#ifdef CONFIG_IDE_ON_TTY
		FD_SET(boardfd, &fdset);
		FD_SET(ttyfd, &fdset);
		ret = select(nfds, &fdset, NULL, NULL, NULL);
		if (ret == -1)
			fatal(8);
		if (FD_ISSET(boardfd, &fdset)) {
			if (!boardio_read(&cmd, 1))
				fatal(9);
			if (cmd == 0) {
				chr = 0;
				if (!boardio_write(&chr, 1))
					fatal(10);
			}
			else {
				ret = write(ttyfd, &cmd, 1);
				if (ret != 1)
					fatal(11);
				continue;
			}
		}
		else if (FD_ISSET(ttyfd, &fdset)) {
			ret = read(ttyfd, &chr, 1);
			if (ret != 1)
				fatal(12);
			/* strip null char */
			if (chr!=0 && !boardio_write(&chr, 1))
				fatal(13);
			continue;
		}
loop:
#endif

		if (!boardio_read(&cmd, 1))
			fatal(14);

#ifdef CONFIG_IDE_ON_TTY
		if (cmd == 0)
			continue;
#endif

		if (cmd == 'Z') {
			openimg();
			if (!boardio_write("U\0\377", 3)) /* sync stream */
				fatal(15);
#ifdef VERBOSE
			fprintf(stderr, "\x1B[2J%c\n", cmd); fflush(stdout);
#endif
			goto endloop;
		}
#ifdef VERBOSE
		fprintf(stderr, "%c", cmd); fflush(stdout);
#endif
		if (!fp)
			goto endloop;
		if (cmd == 'R' || cmd == 'W') {
			if (!boardio_read(&sector, 3))
				fatal(16);
			addr = sector[0]<<(16+0) | sector[1]<<(8+0) | sector[2]<<(0+0);
#ifdef VERBOSE
			fprintf(stderr, "%06X ", addr); fflush(stdout);
#endif
			offset = (long)addr << 9;
			if (fseek(fp, offset, SEEK_SET) != 0)
				fatal(17);
		}
		else
			fatal(18);
		if (cmd == 'R') {
			if (fread(sector, 512, 1, fp) != 1)
				fatal(19);
			if (!boardio_write(sector, 512))
				fatal(20);
		}
		else if (cmd == 'W') {
			if (!boardio_read(sector, 512))
				fatal(21);
			if (fwrite(sector, 512, 1, fp) != 1)
				fatal(22);
			if (fflush(fp))
				fatal(23);
		}
		else
			fatal(24);
#ifdef VERBOSE
		fprintf(stderr, "%02X%02X%02X%02X%02X%02X%02X%02X\n", sector[0], sector[1], sector[2], sector[3], sector[4], sector[5], sector[6], sector[7]);
#endif

endloop:
#ifdef CONFIG_IDE_ON_TTY
		goto loop;
#else
		continue;
#endif
	}

	fatal(0);
	return 0;
}
