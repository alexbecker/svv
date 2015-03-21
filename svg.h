#ifndef _SVG_H
#define _SVG_H

#include <sys/queue.h>
#include "image.h"

typedef enum _segment_type {
	line,
	quadratic_bezier
} segment_type;

typedef struct _line {
	int x, y;
} line;

typedef struct _quadratic_bezier {
	int x_1, y_1;	// control points
	int x, y;
} quadratic_bezier;

typedef struct _segment {
	segment_type type;
	union {
		line l;
		quadratic_bezier q;
	}
	LIST_ENTRY(_segment) entries;
} segment;

typedef struct _path {
	int x_0, y_0;
	pixel start;
	LIST_HEAD(segments, segment) segments;
} path;

void print_path(path p);

#endif
