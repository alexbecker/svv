#include <stdlib.h>
#include <stdio.h>
#include "svg.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

typedef struct _point_entry {
	int x, y;
	LIST_ENTRY(_point_entry) entries;
} point;

typedef LIST_HEAD(point_list_head, _point_entry) point_list;

path vectorize_edge(image img, int x, int y) {
	point_list points;
	LIST_INIT(&points);

	point *p = malloc(sizeof(struct _point_entry));
	p->x = x;
	p->y = y;
	LIST_INSERT_HEAD(&points, p, entries);

	int min_x = MAX(x - 1, 0), max_x = MIN(x + 1, img.max_x);
	int min_y = MAX(y - 1, 0), max_y = MIN(y + 1, img.max_y);
	for (int j = min_y; j <= max_y; j++) {
		for (int i = min_x; i <= max_x; i++) {
			if (i != x || j != y) {
				if () {
				}
			}
		}
	}
}

int vectorize_edges(image img) {
	int color = 1;

	for (int y=0; y<img.max_y; y++) {
		for (int x=0; x<img.max_x; x++) {
			if (img.pixels[x + img.w * y].edge_flag == 1) {
				classify_edge_from_point(img, ++color, x, y);
			}
		}
	}

	return color - 1;
}
