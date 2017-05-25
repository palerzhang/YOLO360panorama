#ifndef REGION_WRITER_H
#define REGION_WRITER_H

// create for 360 panorama by paler

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "box.h"

#define VALMAX(x,y) (x>y?x:y)
#define VALMIN(x,y) (x<y?x:y)
#define VALNONNEG(x) (x<0?0:x) 
#define SWAPVAL(x,y) (x ^= y ^= x ^= y)
#define GAP 6

typedef struct 
{
	int c; // class
	int u; // is up
	float l; // left
	float r; // right
	float t; // top
	float b; // bottom
	float p; // probability
} box4panorama;

// swap two float variables
void swap_float(float * v1, float * v2);
// swap two box4panorama variables
void swap_b4p(box4panorama * b1, box4panorama * b2);
// calculate the value of l, r, t, b for a box4panorama
void cal_lrtb(box4panorama * b, float x, float y, float w, float h);
// calculate the iou of two box4panoramas
float b4p_iou(box4panorama * b1, box4panorama * b2);
// convert boxes into box4panoramas
void box_2_box4panorama(box4panorama * tb4ps, int *tnum, box4panorama * bb4ps, int * bnum, box *boxes, int num, float thresh, float **probs, int classes, int gap, int size);
// handle clip
void handle_clip(box4panorama * r1, int * size1, box4panorama * r2, int * size2, float threshold);
// finetune the boxes
void finetune_both(box4panorama * r1, int * size1, box4panorama * r2, int * size2, float threshold, box4panorama * merged, int * size);
// finetune self across clip line
void finetune_self(box4panorama * rg, int * size, float threshold);
// copy box4panorama to another
void b4p_copy(box4panorama * dst, const box4panorama * src);
// every char of a string match c
int strequal_char(const char * str, char c);

void write_regions_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes);

void write_normalized_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes, int gap, int size);

#endif