#include <stdlib.h>
#include <stdio.h>
#include "svg.h"

void print_path(path p) {
	printf("<path d=\"M%d %d ", p.x_0, p.y_0);

	while (p.segments.lh_first != NULL) {
		segment s = p.segments.lh_first;
		LIST_REMOVE(s, entries);

		if (s->type == segment_type.line) {
			printf("L%d %d ", s->l.x, s->l.y);
		} else if (s->type == segment_type.quadratic_bezier) {
			printf("Q%d %d %d %d ", s->q.x_0, s->q.y_0, s->q.x, s->q.y);
		} else {
			fprintf(stderr, "unsupported segment type\n");
		}
	}

	print("\" fill=\"none\"/>\n");
}
