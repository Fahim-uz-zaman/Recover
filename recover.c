/*

Debugged.
Recovered images look good!

*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint8_t BYTE;

BYTE buffer[512]; // stores the individual memory chunks of 512 bytes each that jpeg files contain

int isNewJpeg()
{
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
    {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        // That's invalid usage
        printf("Usage: ./recover image\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        printf("Couldn't open the file.\n");
        return 2;
    }


    FILE *img = NULL; // this will store pointers to new jpeg files later in my program
    int i = -1;

    while (fread(buffer, sizeof(BYTE), 512, file) == 512)
    {
        // Whenever I have a full 512 chunk of bytes available in the file, I'm in here

        if (isNewJpeg())
        {
            // currently buffer stores a memory chunk which marks the beginning of a new jpeg

            i++;

            char *filename = malloc((sizeof(char)) * 8);
            if (filename == NULL) // making sure if memory was allocated succesfully
            {
                printf("Error occured while allocating memory. \n");
                return 3;
            }
            sprintf(filename, "%03i.jpg", i);
            // this gives me each new jpeg's file name

            if (i == 0) // if it's the first time I found a jpeg
            {
                img = fopen(filename, "w");
                if (img == NULL)
                {
                    printf("Failed to open file.\n");
                    return 2;
                }
            }
            else // if I have already been writing to a jpeg and just found a new one
            {
                fclose(img);
                img = fopen(filename, "w");
                if (img == NULL)
                {
                    printf("Failed to open file.\n");
                    return 2;
                }
            }
            fwrite(buffer, sizeof(BYTE), 512, img); // copying over this memory chunk into current jpeg file
            free(filename);
        }
        else if (i >= 0) // if the memory chunk we're looking at is not the beginning of a jpeg file
        {
            fwrite(buffer, sizeof(BYTE), 512, img); // copying over this memory chunk into current jpeg file
        }
    }
    if (img != NULL) // let's close any remaining open file(s)
    {
        fclose(img);
    }
    return 0;
}
