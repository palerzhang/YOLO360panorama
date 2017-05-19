#include "region_writer.h"
#include <string.h>

void write_regions_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes)
{
	int i;

	FILE *fp = fopen(filename, "w+");
	if (fp == 0)
	{
		printf("cannot open file.\n");
		return 0;
	}

    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh){

            //int width = im.h * .012;

            //if(0){
            //    width = pow(prob, 1./2.)*10+1;
            //    alphabet = 0;
            //}

            //printf("%s: %.0f%%\n", names[class], prob*100);

            //int left  = (b.x-b.w/2.)*im.w;
            //int right = (b.x+b.w/2.)*im.w;
            //int top   = (b.y-b.h/2.)*im.h;
            //int bot   = (b.y+b.h/2.)*im.h;

            //if(left < 0) left = 0;
            //if(right > im.w-1) right = im.w-1;
            //if(top < 0) top = 0;
            //if(bot > im.h-1) bot = im.h-1;
            box b = boxes[i];
            char buf[64];
            sprintf(buf,"%d %.8f %.8f %.8f %.8f %.2f\n", class, b.x, b.y, b.w, b.h, prob);
            fwrite(buf, strlen(buf), 1, fp);
        }
    }
    fclose(fp);
    return;
}

void write_normalized_to_file(const char * filename, int num, float thresh, box *boxes, float **probs, int classes, int gap, int size)
{
	int i;

	FILE *fp = fopen(filename, "w+");
	if (fp == 0)
	{
		printf("cannot open file.\n");
		return 0;
	}

    for(i = 0; i < num; ++i){
        int class = max_index(probs[i], classes);
        float prob = probs[i][class];
        if(prob > thresh)
        {
            box b = boxes[i];

            float x,y,w,h;
            int orign = 1;
            if (b.y > 0.5)
       		{
            	x = b.x + 0.5;
            	x = x > 1 ? x - 1 : x;
            	y = (2.0*size*b.y-size-gap) / (size-gap);
            	w = b.w;
            	h = 2.0*size*b.h / (size-gap);
            	orign = 0;
            }
            else
            {
            	x = b.x;
            	y = 2.0*size*b.y / (size-gap);
            	w = b.w;
            	h = 2.0*size*b.h / (size-gap);
            }

            char buf[64];
            sprintf(buf,"%d %d %.8f %.8f %.8f %.8f %.2f\n", class, orign, x, y, w, h, prob);
            fwrite(buf, strlen(buf), 1, fp);
        }
    }
    fclose(fp);
    return;
}