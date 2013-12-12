/* Copyright 2013 Brian Swetland <swetland@frotz.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define _GNU_SOURCE /* for SEEK_HOLE */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "util.h"

static u8 buffer[1024*1024];

int main(int argc, char **argv) {
	struct chunk c;
	off_t a, b = 0;
	int fin, fout;

	if (argc != 3) {
		fprintf(stderr, "usage: mksparse <infile> <outfile>");
		return -1;
	}

	if ((fin = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "error: cannot open '%s' for reading\n", argv[1]);
		return -1;
	}

	if (!strcmp(argv[2], "-")) {
		fout = 1;
	} else if ((fout = open(argv[2], O_WRONLY | O_CREAT, 0600)) < 0) {
		fprintf(stderr, "error: cannot open '%s' for writing\n", argv[2]);
		return -1;
	}

	while ((a = lseek(fin, b, SEEK_DATA)) >= 0) {
		b = lseek(fin, a, SEEK_HOLE);
		c.start = a;
		c.length = b - a;
		if (lseek(fin, a, SEEK_SET) != a)
			goto fail;
		if (writex(fout, &c, sizeof(c)))
			goto fail;
		if (copyx(fin, fout, c.length, buffer, sizeof(buffer)))
			goto fail;
		/* fprintf(stderr, "%lu bytes at %lu\n", c.length, c.start); */
	}

	c.start = c.length = 0;
	if (writex(fout, &c, sizeof(c)))
		goto fail;

	if (close(fout))
		goto fail;

	return 0;

fail:
	fprintf(stderr, "error: %s\n", strerror(errno));
	return -1;
}

