#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>

#define MAX_DIFF 3
#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef TAILQ_HEAD(point_queue_head, int) point_queue_head;

typedef struct _point_entry {
	int x, y;
	TAILQ_ENTRY(struct _point_entry) entries;
} point_entry;

typedef struct _pixel {
	char edge_flag;
	short group_color;
	char color[3];
	char intensity;
} pixel;

typedef struct _image {
	int w, h, max_x, max_y;
	pixel *pixels;
} image;

inline int neighbors(pixel p, pixel q) {
	short diff[4] = {(short) p.color[0] - (short) q.color[0], (short) p.color[1] - (short) q.color[1], (short) p.color[2] - (short) q.color[2], (short) p.intensity - (short) q.intensity}
	return ABS(diff[0]) + ABS(diff[1]) + ABS(diff[2]) + ABS(diff[3]) < MAX_DIFF;
}

void flood_fill_from_point(image img, int x, int y) {
	int ind = x + img.w * y;

	point_queue_head queue;
	TAILQ_INIT(&queue);

	point_entry *point = malloc(sizeof(struct _point_entry));
	point->x = x;
	point->y = y;
	TAILQ_INSERT_TAIL(&queue, point, entries);

	pixel orig = img.pixels[ind];

	while (queue.tqh_first != NULL) {
		point = queue.tqh_first;
		TAILQ_REMOVE(&queue, point, entries);

		x = point->x;
		y = point->y;
		free(point);

		pixel *cur = img->pixels[x + img.w * y];

		if (cur->group_color || !neighbors(orig, *cur)) {
			cur->edge_flag = 1;
			continue;
		}

		cur->group_color = orig.group_color;

		int min_x = MIN(x - 1, 0), max_x = MAX(x + 1, img.max_x);
		int min_y = MIN(y - 1, 0), max_y = MAX(y + 1, img.max_y);
		for (int j = min_y; j < max_y; j++) {
			for (int i = min_x; i < max_x; i++) {
				if (i != x || j != y) {
					pixel *new_index = malloc(sizeof(struct _index_entry));
					new_index->x = i;
					new_index->y = j;
					TAILQ_INSERT_TAIL(&queue, new_index, entries);
				}
			}
		}
	}
}

int flood_fill(image img) {
	int color = 0;

	for (int y=0; y<img.h; y++) {
		for (int x=0; x<img.w; x++) {
			index = x + img.w * y;

			if (!img.pixels[index].group_color) {
				img.pixels[index].group_color = ++color;
				flood_fill_from_point(img, x, y);
			}
		}
	}

	return color;
}

image read_bitmap(FILE *fp) {
	// read header size
	fgetc(fp);
	fgetc(fp);
	size_t header_size = fgetc(fp);
	fseek(SEEK_SET);
	
	// read header
	char *header = malloc(header_size);
	fread(fp, 1, header_size, header);

	// read image size
	image img;
	img.w = 16 * (int) header[19] + (int) header[18];
	img.h = 16 * (int) header[23] + (int) header[22];
	img.max_x = img.w - 1;
	img.max_y = img.h - 1;
	int image_size = img.w * img.h;
	free(header);

	img.pixels = calloc(image_size, sizeof(struct _pixel));

	// read image
	char *raw = malloc(image_size * 4);
	for (int i = 0; i < image_size; i += 4) {
		img.pixels[i].color = {raw[i], raw[i + 1], raw[i + 2]};
		img.pixels[i].intensity = raw[i + 3];
	}

	free(raw);
	return img;
}

int main(int argc, char **argv) {
	char *filename = argv[1];
	FILE *in = fopen(filename, "r");
	image img = read_bitmap(in);

	int color_groups = flood_fill(img);
}
