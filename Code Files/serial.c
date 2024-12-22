#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h> 

typedef enum {RGB, GREY} color_t;

void convolute(uint8_t *src, uint8_t *dst, int row_from, int row_to, int col_from, int col_to, int width, int height, float **h, color_t imageType);
void convolute_grey(uint8_t *src, uint8_t *dst, int x, int y, int width, int height, float **h);
void convolute_rgb(uint8_t *src, uint8_t *dst, int x, int y, int width, int height, float **h);
void Usage(int argc, char **argv, char **image, int *width, int *height, int *loops, color_t *imageType);
uint8_t *offset(uint8_t *array, int i, int j, int width);

int main(int argc, char** argv) {
    int i, j, t, width, height, loops;
    char *image;
    color_t imageType;
    
    clock_t start_time, end_time;
    double cpu_time_used;
    
    // Start timing
    start_time = clock();
    
    // Check arguments
    Usage(argc, argv, &image, &width, &height, &loops, &imageType);
    
    // Init filters
    int gaussian_blur[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    float **h = malloc(3 * sizeof(float *));
    for (i = 0; i < 3; i++)
        h[i] = malloc(3 * sizeof(float));
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            h[i][j] = gaussian_blur[i][j] / 16.0;
        }
    }
    
    // Init arrays with padding
    uint8_t *src = NULL, *dst = NULL, *tmp = NULL;
    if (imageType == GREY) {
        src = calloc((height+2) * (width+2), sizeof(uint8_t));
        dst = calloc((height+2) * (width+2), sizeof(uint8_t));
    } else if (imageType == RGB) {
        src = calloc((height+2) * (width*3+6), sizeof(uint8_t));
        dst = calloc((height+2) * (width*3+6), sizeof(uint8_t));
    }
    
    if (src == NULL || dst == NULL) {
        fprintf(stderr, "%s: Not enough memory\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    // Read input file
    FILE *fp = fopen(image, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open input file\n");
        return EXIT_FAILURE;
    }
    
    if (imageType == GREY) {
        for (i = 1; i <= height; i++) {
            uint8_t *tmpbuf = offset(src, i, 1, width+2);
            fread(tmpbuf, sizeof(uint8_t), width, fp);
        }
    } else if (imageType == RGB) {
        for (i = 1; i <= height; i++) {
            uint8_t *tmpbuf = offset(src, i, 3, width*3+6);
            fread(tmpbuf, sizeof(uint8_t), width*3, fp);
        }
    }
    fclose(fp);
    
    // Record time before convolution
    clock_t conv_start = clock();
    
    // Convolute "loops" times
    for (t = 0; t < loops; t++) {
        // Process entire image
        convolute(src, dst, 1, height, 1, width, width, height, h, imageType);
        
        // Swap arrays
        tmp = src;
        src = dst;
        dst = tmp;
    }
    
    // Record time after convolution
    clock_t conv_end = clock();
    
    // Write output file
    char *outImage = malloc((strlen(image) + 9) * sizeof(char));
    strcpy(outImage, "blur_");
    strcat(outImage, image);
    
    FILE *outFile = fopen(outImage, "wb");
    if (outFile == NULL) {
        fprintf(stderr, "Cannot open output file\n");
        return EXIT_FAILURE;
    }
    
    if (imageType == GREY) {
        for (i = 1; i <= height; i++) {
            uint8_t *tmpbuf = offset(src, i, 1, width+2);
            fwrite(tmpbuf, sizeof(uint8_t), width, outFile);
        }
    } else if (imageType == RGB) {
        for (i = 1; i <= height; i++) {
            uint8_t *tmpbuf = offset(src, i, 3, width*3+6);
            fwrite(tmpbuf, sizeof(uint8_t), width*3, outFile);
        }
    }
    fclose(outFile);
    
    // End timing
    end_time = clock();
    
    // Calculate and display timing information
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    double conv_time = ((double) (conv_end - conv_start)) / CLOCKS_PER_SEC;
    
    printf("\nPerformance Metrics:\n");
    printf("Total execution time: %.3f seconds\n", cpu_time_used);
    printf("Convolution time only: %.3f seconds\n", conv_time);
    printf("Image type: %s\n", imageType == GREY ? "Greyscale" : "RGB");
    printf("Image dimensions: %dx%d\n", width, height);
    printf("Number of iterations: %d\n", loops);
    
    // Free memory
    free(src);
    free(dst);
    for (i = 0; i < 3; i++)
        free(h[i]);
    free(h);
    free(image);
    free(outImage);
    
    return EXIT_SUCCESS;
}

void convolute(uint8_t *src, uint8_t *dst, int row_from, int row_to, int col_from, int col_to, int width, int height, float **h, color_t imageType) {
    int i, j;
    if (imageType == GREY) {
        for (i = row_from; i <= row_to; i++)
            for (j = col_from; j <= col_to; j++)
                convolute_grey(src, dst, i, j, width+2, height, h);
    } else if (imageType == RGB) {
        for (i = row_from; i <= row_to; i++)
            for (j = col_from; j <= col_to; j++)
                convolute_rgb(src, dst, i, j*3, width*3+6, height, h);
    }
}

void convolute_grey(uint8_t *src, uint8_t *dst, int x, int y, int width, int height, float **h) {
    int i, j, k, l;
    float val = 0;
    for (i = x-1, k = 0; i <= x+1; i++, k++)
        for (j = y-1, l = 0; j <= y+1; j++, l++)
            val += src[width * i + j] * h[k][l];
    dst[width * x + y] = val;
}

void convolute_rgb(uint8_t *src, uint8_t *dst, int x, int y, int width, int height, float **h) {
    int i, j, k, l;
    float redval = 0, greenval = 0, blueval = 0;
    for (i = x-1, k = 0; i <= x+1; i++, k++)
        for (j = y-3, l = 0; j <= y+3; j+=3, l++) {
            redval += src[width * i + j] * h[k][l];
            greenval += src[width * i + j+1] * h[k][l];
            blueval += src[width * i + j+2] * h[k][l];
        }
    dst[width * x + y] = redval;
    dst[width * x + y+1] = greenval;
    dst[width * x + y+2] = blueval;
}

uint8_t *offset(uint8_t *array, int i, int j, int width) {
    return &array[width * i + j];
}

void Usage(int argc, char **argv, char **image, int *width, int *height, int *loops, color_t *imageType) {
    if (argc == 6 && !strcmp(argv[5], "grey")) {
        *image = malloc((strlen(argv[1])+1) * sizeof(char));
        strcpy(*image, argv[1]); 
        *width = atoi(argv[2]);
        *height = atoi(argv[3]);
        *loops = atoi(argv[4]);
        *imageType = GREY;
    } else if (argc == 6 && !strcmp(argv[5], "rgb")) {
        *image = malloc((strlen(argv[1])+1) * sizeof(char));
        strcpy(*image, argv[1]); 
        *width = atoi(argv[2]);
        *height = atoi(argv[3]);
        *loops = atoi(argv[4]);
        *imageType = RGB;
    } else {
        fprintf(stderr, "\nError Input!\n%s image_name width height loops [rgb/grey].\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }
}