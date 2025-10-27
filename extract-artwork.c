/*
 * (C) 2015-2025 by Christian Hesse <mail@eworm.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include <libavformat/avformat.h>

int main(int argc, char **argv) {
	unsigned int i, j;
	FILE * album_art;
	char * album_art_file = NULL;
	AVPacket pkt;
	AVFormatContext * pFormatCtx;

	if (argc < 2) {
		printf("usage: %s file1.mp3 [file2.mp3 ...]\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* libav */
#if LIBAVFORMAT_VERSION_INT < AV_VERSION_INT(58, 9, 100)
	av_register_all();
#endif

	/* only fatal messages from libav */
	av_log_set_level(AV_LOG_FATAL);

	pFormatCtx = avformat_alloc_context();

	for (i = 1; i < argc; i++) {
		if (avformat_open_input(&pFormatCtx, argv[i], NULL, NULL) != 0) {
			fprintf(stderr, "%s: failed to open file for reading", argv[i]);
			goto next;
		}

		/* only flac & mp3 files contain artwork, so ignore others */
		if (strcmp(pFormatCtx->iformat->name, "flac") != 0 &&
		    strcmp(pFormatCtx->iformat->name, "mp3") != 0) {
			fprintf(stderr, "%s: not a supported media file\n", argv[i]);
			goto next;
		}

		/* find the first attached picture, if available */
		for (j = 0; j < pFormatCtx->nb_streams; j++) {
			if (pFormatCtx->streams[j]->disposition & AV_DISPOSITION_ATTACHED_PIC) {
				pkt = pFormatCtx->streams[j]->attached_pic;

				album_art_file = malloc(strlen(argv[i]) + 9);
				sprintf(album_art_file, "%s.%03u.jpg", argv[i], j);

				printf("%s: found artwork, writing to file %s\n", argv[i], album_art_file);

				if ((album_art = fopen(album_art_file, "wb")) == NULL)
					fprintf(stderr, "%s: failed to open file for writing\n", album_art_file);

				if (fwrite(pkt.data, pkt.size, 1, album_art) != 1)
					fprintf(stderr, "%s: failed to write to file\n", album_art_file);

				fclose(album_art);
				free(album_art_file);
			}
		}
next:
		avformat_close_input(&pFormatCtx);
	}
	avformat_free_context(pFormatCtx);

	return EXIT_SUCCESS;
}

// vim: set syntax=c:
