#ifndef _IMAGE_H
#define _IMAGE_H

typedef struct _pixel {
	int edge_flag;
	int group_color;
	unsigned char color[3];
	unsigned char intensity;
} pixel;

typedef enum _format {
	RGB24 = 0x18,
	RGBA = 0x20
} format;

typedef struct _image {
	int w, h, max_x, max_y, size;
	pixel *pixels;
	char *header;
	int header_size;
	format fmt;
} image;

image read_bitmap(FILE *fp);

void write_bitmap(image img, FILE *fp);

#endif
