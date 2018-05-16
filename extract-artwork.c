/*
 * (C) 2015-2018 by Christian Hesse <mail@eworm.de>
 *
 * This software may be used and distributed according to the terms
 * of the GNU General Public License, incorporated herein by reference.
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
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 9, 100)
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

		/* only mp3 file contain artwork, so ignore others */
		if (strcmp(pFormatCtx->iformat->name, "mp3") != 0) {
			fprintf(stderr, "%s: not a mp3 file\n", argv[i]);
			goto next;
		}

		if (pFormatCtx->iformat->read_header(pFormatCtx) < 0) {
			fprintf(stderr, "%s: could not read the format header\n", argv[i]);
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
