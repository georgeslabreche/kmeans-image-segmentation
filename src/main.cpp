#include <iostream>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>

#include <dkm.hpp>
#include <dkm_utils.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#include "stb_image_resize.h"


using namespace std;

/* define the name of this program for convenience purposes when printing instructions */
#define PROGRAM_NAME                                                               "image_segmentation"

/* flag indicating whether or not the pixel values should be normalized or not. */
#define NORMALIZE                                                                                     1

/* define convenience macros */
#define streq(s1,s2)    (!strcmp ((s1), (s2)))


/* the main function */
int main(int argc, char **argv)
{
    printf("Hello Space!\n");

    /* get provider host and port from command arguments */
    int8_t argv_index_input = -1;
    int8_t argv_index_metadata = -1;
    int8_t argv_index_write = -1;
    int8_t argv_index_args = -1;

    // --------------------------------------------------------------------------
    // parse the command arguments

    int8_t argn;
    for (argn = 1; argn < argc; argn++)
    {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-?"))
        {
            printf("%s [options] ...", PROGRAM_NAME);
            printf("\n  --input    / -i       the file path of the input image");
            printf("\n  --metadata / -m       the file path of the metadata csv file");
            printf("\n  --write    / -w       the write mode of the output image (optional)"
                   "\n\t0 - do not write a new image (equivalent to not specifying the --write option)"
                   "\n\t1 - write a new image as a new file"
                   "\n\t2 - write a new image that overwrites the input image file"
                   "\n\t3 - same as option 2 but backs up the original input image"
                  );
            printf("\n  --args     / -a       additional arguments specific to this program (optional)");
            printf("\n  --help     / -?       this information\n");
            
            /* program error exit code */
            /* 11 	EAGAIN 	Try again */
            return EAGAIN;
        }
        else
        if (streq (argv [argn], "--input")
        ||  streq (argv [argn], "-i"))
            argv_index_input = ++argn;
        else
        if (streq (argv [argn], "--metadata")
        ||  streq (argv [argn], "-m"))
            argv_index_metadata = ++argn;
        else
        if (streq (argv [argn], "--write")
        ||  streq (argv [argn], "-w"))
            argv_index_write = ++argn;
        else
        if (streq (argv [argn], "--args")
        ||  streq (argv [argn], "-a"))
            argv_index_args = ++argn;
        else
        {
            /* print error message */
            printf("Unknown option. Get help: ./%s -?", PROGRAM_NAME);

            /* program error exit code */
            /* 22 	EINVAL 	Invalid argument */
            return EINVAL;
        }
    }


    // --------------------------------------------------------------------------
    // parse the input image file path

    if(argv_index_input == -1)
    {
        /* print error message */
        printf("No image input path specified. Get help: ./%s -?\n", PROGRAM_NAME);

        /* program error exit code */
        /* 22 	EINVAL 	Invalid argument */
        return EINVAL;
    }


    // --------------------------------------------------------------------------
    // parse the input image file path

    if(argv_index_metadata == -1)
    {
        /* printf for documentation purposes only */
        printf("No metadata file path specified. Get help: ./%s -?\n", PROGRAM_NAME);
    }


    /**
     * TODO:
     *  1. read image to buffer
     *  2. process
     *  3. write image output
     * 
     */



    return 0;
}