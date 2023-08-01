#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"
#include <math.h>

void filter(int height, int width, RGBTRIPLE image[height][width], int newHeight, int ratio, int newWidth, int newImage[newHeight][newWidth]);
int average(RGBTRIPLE value);
void sobel(int newHeight, int ratio, int newWidth, int newImage[newHeight][newWidth]);

int main(int argc, char *argv[])
{
    // USING DAVID MALAN'S CODE FOR: PROCESSING BMP FILE (PROCESSING FILE HEADER + FILE INFO, SKIPPING PADDING - READING RGB VALUES INTO PIXEL ARRAY) ↓↓↓
    if (argc != optind + 1)
    {
        printf("Usage: ./filter infile\n");
        return 1;
    }

    char *infile = argv[optind];

    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        printf("Unsupported file format.\n");
        return 4;
    }

    int height = abs(bi.biHeight);
    int width = bi.biWidth;

    RGBTRIPLE(*image)
    [width] = calloc(height, width * sizeof(RGBTRIPLE));
    if (image == NULL)
    {
        printf("Not enough memory to store image.\n");
        fclose(inptr);
        return 5;
    }

    int padding = (4 - (width * sizeof(RGBTRIPLE)) % 4) % 4;

    for (int i = 0; i < height; i++)
    {
        fread(image[i], sizeof(RGBTRIPLE), width, inptr);
        fseek(inptr, padding, SEEK_CUR);
    }
    
    // END OF DAVID MALAN'S CODE ↑↑↑

    // ORIGINAL CODE BY MYSELF ↓↓↓

    // determining parameters for shrinking new image to a height of 32 (and shrinking width proportionally)
    int newHeight = 32;
    int ratio = floor(height / newHeight);
    if ((float)height / ratio > 32) // as we're incrementing by ratio in a later function, it's important that the ratio actually fits into 32 height times to avoid a memory issue)
    {
        ratio++;
    }
    int newWidth = floor(width / ratio);

    // creating new array for shrunken image
    int(*newImage)[newWidth] = calloc(newHeight, newWidth * sizeof(int));
    if (newImage == NULL)
    {
        printf("Not enough memory to store 36 x 36 array.\n");
        fclose(inptr);
        return 6;
    }

    // function that pixelates/shrinks original image into new array
    filter(height, width, image, newHeight, ratio, newWidth, newImage);
    free(image);

    // function that iterates through new array - calculates sobel value and prints respective character
    sobel(newHeight, ratio, newWidth, newImage);

    // closing it out!
    free(newImage);
    fclose(inptr);
    return 0;
}

void filter(int height, int width, RGBTRIPLE image[height][width], int newHeight, int ratio, int newWidth, int newImage[newHeight][newWidth])
{
    // PIXELATION
    int pixel_added = 0;

    // configuring parameters
    printf("height is %i\n", height);
    printf("width is %i\n", width);
    printf("ratio is %i\n", ratio);

    // pixel and grayscale
    int counterx = 0;
    int countery = 0;
    // averaging size (ratio x ratio) blocks of pixels into a single pixel in the new array
    for (int i = 0; i < height; i += ratio)
    {
        counterx = 0;
        for (int j = 0; j < width; j += ratio)
        {
            int score = 0;
            for (int x = 0; x < ratio; x++)
            {
                for (int z = 0; z < ratio; z++)
                {
                    score += average(image[i + x][j + z]); // averages the rgb values of the image (greyscaling) so we can work with a single range of ints instead of RGBTRIPLES.
                }
            }
            newImage[(newHeight - 1) - countery][counterx] = score / (ratio * ratio); //<-- BMP files are stored upside down, this flips the image right-side up as its read into the new array.
            pixel_added++;
            counterx++;
        }
        countery++;
    }
    printf("pixel added = %i\n", pixel_added);
    return;
}

int average(RGBTRIPLE value)
{
    // GREYSCALE
    int average = (value.rgbtBlue + value.rgbtGreen + value.rgbtRed) / 3;
    return average;
}

void sobel(int newHeight, int ratio, int newWidth, int newImage[newHeight][newWidth])
{
    // TEMP ARRAY
    int temp_array[newHeight][newWidth]; // <-- temp array ensures that we our calculations are based on original values rather than changed values, which would be included in the 9x9 square for sobel operation.

    for (int a = 0; a < newHeight; a++)
    {
        for (int b = 0; b < newWidth; b++)
        {
            temp_array[a][b] = newImage[a][b];
        }
    }
    // SOBEL & PRINT
    for (int current_height = 0; current_height < newHeight - 1; current_height++)
    {
        for (int current_width = 0; current_width < newWidth - 1; current_width++)
        {

            int ARRAY[9];
            for (int m = 0; m < 9; m++)
            {
                ARRAY[m] = 0;
            }
            int k = 0;

            int Gx = 0;

            int Gy = 0;

            int Final = 0;
            // creates 9 x 9 square while avoiding searching past the beginning or end of array (these values are zeroed and therefore imply a black border around the image)
            for (int i = -1; i < 2; i++)
            {
                if (i + current_width < 0 || i + current_width == newWidth)
                {
                    k += 3;
                    continue;
                }
                for (int j = -1; j < 2; j++)
                {
                    if (j + current_height < 0 || j + current_height == newHeight)
                    {
                        k++;
                        continue;
                    }

                    ARRAY[k] = temp_array[current_height + i][current_width + j];
                    k++;
                }
            }
            // sobel operation
            Gx = (ARRAY[0] * -1) + (ARRAY[2] * 1) + (ARRAY[3] * -2) + (ARRAY[5] * 2) + (ARRAY[6] * -1) + (ARRAY[8] * 1);
            Gy = (ARRAY[0] * -1) + (ARRAY[1] * -2) + (ARRAY[2] * -1) + (ARRAY[6] * 1) + (ARRAY[7] * 2) + (ARRAY[8] * 1);

            Final = (int)round(sqrt(Gx * Gx + Gy * Gy));

            // printing time
            if (Final >= 200) // <-- CHANGE VALUE FOR MORE OR LESS DETAIL! GREATER VALUE = LESS DETAIL.
            {
                // CHECK FOR SIGNIFICANT STRENGTH IN EITHER DIRECTION
                if ((Gx * Gx) < ((Gy * Gy) / 2)) // <-- if exceptional polarity in Y direction, print horizontal line
                {
                    if (Gy < 0) //<-- check direction of polarity (see sobel operation matrix)
                    {
                        printf("¯¯");
                    }
                    else
                    {
                        printf("__");
                    }
                }
                else if ((Gy * Gy) < ((Gx * Gx) / 2))
                {
                    if (Gx < 0)
                    {
                        printf("| ");
                    }
                    else
                    {
                        printf(" |");
                    }
                }
                // IF NO SIGNIFICANT STRENGTH:
                else
                {
                    if (Gx < 0)
                    {
                        if (Gy < 0)
                        {
                            printf("/¯");
                        }
                        else
                        {
                            printf("\\_");
                        }
                    }
                    else
                    {
                        if (Gy < 0)
                        {
                            printf("¯\\");
                        }
                        else
                        {
                            printf("_/");
                        }
                    }
                }
            }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }

    return;
}