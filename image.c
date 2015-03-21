#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>
#include "image.h"

image read_bitmap(FILE *fp) {
	image img;

	// read header size
	fgetc(fp);
	fgetc(fp);
	img.header_size = (unsigned char) fgetc(fp);
	fseek(fp, 0, SEEK_SET);
	
	// read header
	img.header = malloc(img.header_size);
	fread(img.header, 1, img.header_size, fp);

	// read image size
	img.w = 256 * (int) (unsigned char) img.header[19] + (int) (unsigned char) img.header[18];
	img.h = 256 * (int) (unsigned char) img.header[23] + (int) (unsigned char) img.header[22];
	img.max_x = img.w - 1;
	img.max_y = img.h - 1;
	img.size = img.w * img.h;

	// read format
	img.fmt = (unsigned char) img.header[28];

	img.pixels = calloc(img.size, sizeof(struct _pixel));

	// read image
	char *raw;
	if (img.fmt == RGBA) {
		raw = malloc(img.size * 4);
		fread(raw, 1, img.size * 4, fp);
		for (int i=0; i<img.size; i++) {
			img.pixels[i].color[0] = raw[4 * i];
			img.pixels[i].color[1] = raw[4 * i + 1];
			img.pixels[i].color[2] = raw[4 * i + 2];
			img.pixels[i].intensity = raw[4 * i + 3];
		}
	} else if (img.fmt == RGB24) {
		raw = malloc(img.size * 3);
		fread(raw, 1, img.size * 3, fp);
		for (int i=0; i<img.size; i++) {
			img.pixels[i].color[0] = raw[3 * i];
			img.pixels[i].color[1] = raw[3 * i + 1];
			img.pixels[i].color[2] = raw[3 * i + 2];
		}
	} else {
		fprintf(stderr, "unrecognized format\n");
		exit(1);
	}

	fclose(fp);
	free(raw);
	return img;
}

void write_bitmap(image img, FILE *fp) {
	fwrite(img.header, 1, img.header_size, fp);

	char *out;
	if (img.fmt == RGBA) {
		out = malloc(4 * img.size);
		for (int i=0; i<img.size; i++) {
			pixel p = img.pixels[i];
			out[4 * i] = p.color[0];
			out[4 * i + 1] = p.color[1];
			out[4 * i + 2] = p.color[2];
			out[4 * i + 3] = p.intensity;
		}
		fwrite(out, 1, 4 * img.size, fp);
	} else if (img.fmt == RGB24) {
		out = malloc(3 * img.size);
		for (int i=0; i<img.size; i++) {
			pixel p = img.pixels[i];
			out[3 * i] = p.color[0];
			out[3 * i + 1] = p.color[1];
			out[3 * i + 2] = p.color[2];
		}
		fwrite(out, 1, 3 * img.size, fp);
	}

	fclose(fp);
	free(out);
}
