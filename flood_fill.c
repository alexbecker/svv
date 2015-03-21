#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>
#include "flood_fill.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef TAILQ_HEAD(point_queue_head, _point_entry) point_queue_head;

typedef struct _point_entry {
	int x, y;
	TAILQ_ENTRY(_point_entry) entries;
} point_entry;

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

	img.pixels[x + img.w * y].group_color = group_color;

	while (queue.tqh_first != NULL) {
		point = queue.tqh_first;
		TAILQ_REMOVE(&queue, point, entries);

		x = point->x;
		y = point->y;
		free(point);

		pixel cur = img.pixels[x + img.w * y];

		int min_x = MAX(x - 1, 0), max_x = MIN(x + 1, img.max_x);
		int min_y = MAX(y - 1, 0), max_y = MIN(y + 1, img.max_y);
		for (int j = min_y; j <= max_y; j++) {
			for (int i = min_x; i <= max_x; i++) {
				if (i != x || j != y) {
					pixel *next = &img.pixels[i + img.w * j];

					if (next->group_color != group_color && !next->edge_flag && neighbors(cur, *next)) {
						next->group_color = group_color;

						point = malloc(sizeof(struct _point_entry));
						point->x = i;
						point->y = j;
						TAILQ_INSERT_TAIL(&queue, point, entries);
					} else if (next->group_color != group_color) {
						next->group_color = group_color;
						next->edge_flag = 1;
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

			if (!img.pixels[index].group_color && !img.pixels[index].edge_flag) {
				flood_fill_from_point(img, ++color, x, y);
			}
		}
	}

	return color;
}

#ifdef FLOOD_FILL_TEST
void test_flood_fill(image img) {
	int color_groups = flood_fill(img);
	printf("%d\n", color_groups);

	pixel *color_array = calloc(color_groups, sizeof(struct _pixel));
	for (int i=0; i<img.size; i++) {
		if (img.pixels[i].edge_flag) {
			continue;
		}

		int color = img.pixels[i].group_color;
		if (!color_array[color - 1].group_color) {
			color_array[color - 1] = img.pixels[i];
		}
	}

	for (int i=0; i<img.size; i++) {
		int color = img.pixels[i].group_color;
#ifdef EDGE_TEST
		img.pixels[i].color[0] = 0;
		img.pixels[i].color[1] = 0;
		img.pixels[i].color[2] = 0;
		img.pixels[i].intensity = 0xff;
#else
		img.pixels[i].color[0] = color_array[color - 1].color[0];
		img.pixels[i].color[1] = color_array[color - 1].color[1];
		img.pixels[i].color[2] = color_array[color - 1].color[2];
		img.pixels[i].intensity = color_array[color - 1].intensity;
#endif

		if (img.pixels[i].edge_flag) {
			img.pixels[i].color[0] = (55 * color) % 256;
			img.pixels[i].color[1] = (23 * color) % 256;
			img.pixels[i].color[2] = (101 * color) % 256;
		}
	}
}

int main(int argc, char **argv) {
	max_diff = atoi(argv[1]);

	FILE *in = fopen(argv[2], "rb");
	image img = read_bitmap(in);

	test_flood_fill(img);

	FILE *out = fopen(argv[3], "wb");
	write_bitmap(img, out);

	free(img.pixels);
	free(img.header);
	exit(0);
}
#endif
