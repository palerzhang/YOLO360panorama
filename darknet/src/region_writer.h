#ifndef REGION_WRITER_H
#define REGION_WRITER_H

#include <stdio.h>
#include <stdlib.h>
#include "box.h"

void write_regions_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes);

void write_normalized_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes, int gap, int size);

#endif