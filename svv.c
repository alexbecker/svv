#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef TAILQ_HEAD(point_queue_head, _point_entry) point_queue_head;

typedef struct _point_entry {
	int x, y;
	TAILQ_ENTRY(_point_entry) entries;
} point_entry;

typedef struct _pixel {
	char edge_flag;
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

int max_diff;

int neighbors(pixel p, pixel q) {
	short diff[4] = {(short) p.color[0] - (short) q.color[0], (short) p.color[1] - (short) q.color[1], (short) p.color[2] - (short) q.color[2], (short) p.intensity - (short) q.intensity};
	return ABS(diff[0]) + ABS(diff[1]) + ABS(diff[2]) + ABS(diff[3]) < max_diff;
}

void flood_fill_from_point(image img, int group_color, int x, int y) {
	point_queue_head queue;
	TAILQ_INIT(&queue);

	point_entry *point = malloc(sizeof(struct _point_entry));
	point->x = x;
	point->y = y;
	TAILQ_INSERT_HEAD(&queue, point, entries);

	pixel orig = img.pixels[x + img.w * y];

	while (queue.tqh_first != NULL) {
		point = queue.tqh_first;
		TAILQ_REMOVE(&queue, point, entries);

		x = point->x;
		y = point->y;
		free(point);

		pixel *cur = &img.pixels[x + img.w * y];

		if (cur->group_color || !neighbors(orig, *cur)) {
			if (cur->group_color != group_color)
				cur->edge_flag = 1;
			continue;
		}

		cur->group_color = group_color;

		int min_x = MAX(x - 1, 0), max_x = MIN(x + 1, img.max_x);
		int min_y = MAX(y - 1, 0), max_y = MIN(y + 1, img.max_y);
		for (int j = min_y; j <= max_y; j++) {
			for (int i = min_x; i <= max_x; i++) {
				if (i != x || j != y) {
					if (img.pixels[i + img.w * j].group_color != group_color) {
						point = malloc(sizeof(struct _point_entry));
						point->x = i;
						point->y = j;
						TAILQ_INSERT_TAIL(&queue, point, entries);
					}
				}
			}
		}
	}
}

int flood_fill(image img) {
	int color = 0;

	for (int y=0; y<img.h; y++) {
		for (int x=0; x<img.w; x++) {
			int index = x + img.w * y;

			if (!img.pixels[index].group_color) {
				flood_fill_from_point(img, ++color, x, y);
			}
		}
	}

	return color;
}

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
	} else if (img.fmt == RGB24) {
		out = malloc(3 * img.size);
		for (int i=0; i<img.size; i++) {
			pixel p = img.pixels[i];
			out[3 * i] = p.color[0];
			out[3 * i + 1] = p.color[1];
			out[3 * i + 2] = p.color[2];
		}
	}

	fwrite(out, 1, 4 * img.size, fp);
	free(out);
}

void test_flood_fill(image img, FILE *fp) {
	int color_groups = flood_fill(img);
	printf("%d\n", color_groups);

	pixel *color_array = calloc(color_groups, sizeof(struct _pixel));
	for (int i=0; i<img.size; i++) {
		int color = img.pixels[i].group_color;
		if (!color_array[color - 1].group_color) {
			color_array[color - 1] = img.pixels[i];
		}
	}

	for (int i=0; i<img.size; i++) {
		int color = img.pixels[i].group_color;
//		img.pixels[i] = color_array[color - 1];
		img.pixels[i].color[0] = (55 * color) % 256;
		img.pixels[i].color[1] = color / 256 % 256;
		img.pixels[i].color[2] = color / (256 * 256) % 256;
		img.pixels[i].intensity = 0xff;

		if (img.pixels[i].edge_flag) {
			img.pixels[i].color[0] = 0x00;
			img.pixels[i].color[1] = 0x00;
			img.pixels[i].color[2] = 0xff;
		}
	}

	write_bitmap(img, fp);
}

int main(int argc, char **argv) {
	max_diff = atoi(argv[1]);

	FILE *in = fopen(argv[2], "rb");
	image img = read_bitmap(in);

	FILE *out = fopen(argv[3], "wb");
	test_flood_fill(img, out);

	free(img.pixels);
	free(img.header);
	exit(0);
}
