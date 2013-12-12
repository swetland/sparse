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

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "util.h"

static u8 buffer[1024*1024];

int main(int argc, char **argv) {
	struct chunk c;
	int fin = 0;
	int fout = 0;

	if (argc != 3) {
		fprintf(stderr, "usage: unsparse <infile> <outfile>\n");
		return -1;
	}

	if (!strcmp(argv[1], "-")) {
		fin = 0;
	} else if ((fin = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "error: cannot open '%s' for reading\n", argv[1]);
		return -1;
	}

	if ((fout = open(argv[2], O_WRONLY | O_CREAT, 0600)) < 0) {
		fprintf(stderr, "error: cannot open '%s' for writing\n", argv[2]);
		return -1;
	}

	for (;;) {
		if (readx(fin, &c, sizeof(c)))
			break;
		if ((c.start == 0) && (c.length == 0)) {
			if (close(fout))
				break;
			return 0;
		}
		if (lseek(fout, c.start, SEEK_SET) != c.start)
			break;
		if (copyx(fin, fout, c.length, buffer, sizeof(buffer)))
			break;
	}

	fprintf(stderr, "error: %s\n", strerror(errno));
	return -1;
}

