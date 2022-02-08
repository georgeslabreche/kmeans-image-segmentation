#include <iostream>
#include <vector>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>

#include <dkm.hpp>
#include <dkm_utils.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "palette.h"

/* define the name of this program for convenience purposes when printing instructions */
#define PROGRAM_NAME                                                               "image_segmentation"

/* indicate if we are building for the OPS-SAT spacecraft or not */
#define TARGET_BUILD_OPSSAT                                                                           1

/* flag indicating whether or not the pixel values should be normalized or not */
#define NORMALIZE                                                                                     1

/* jpeg write quality */
#define JPEG_WRITE_QUALITY                                                                           90

/* define convenience macros */
#define streq(s1,s2)    (!strcmp ((s1), (s2)))

    

// --------------------------------------------------------------------------
// parse the program options

int parse_options(int argc, char **argv,
    int *argv_index_input, int *argv_index_metadata,
    int *argv_index_write_mode, int *argv_index_k,
    int *argv_index_palette)
{
    int argn;
    for (argn = 1; argn < argc; argn++)
    {
        if (streq (argv [argn], "--help")
        ||  streq (argv [argn], "-?"))
        {
            printf("%s [options] ...", PROGRAM_NAME);
            printf("\n  --input    / -i       the file path of the input image");
            printf("\n  --metadata / -m       the file path of the metadata csv file (optional)");
            printf("\n  --write    / -w       the write mode of the output image (optional)"
                   "\n\t0 - do not write a new image (equivalent to not specifying --write)"
                   "\n\t1 - write a new image as a new file"
                   "\n\t2 - write a new image that overwrites the input image file"
                   "\n\t3 - same as option 2 but backs up the original input image"
                  );
            printf("\n  --kvalue   / -k       the k-means k value (2 to 11)");
            printf("\n  --palette  / -p       the palette to use for segmentation (optional)");
            printf("\n  --help     / -?       this information\n");
            
            /* program error exit code */
            /* 11 	EAGAIN 	Try again */
            return EAGAIN;
        }
        else
        if (streq (argv [argn], "--input")
        ||  streq (argv [argn], "-i"))
            *argv_index_input = ++argn;
        else
        if (streq (argv [argn], "--metadata")
        ||  streq (argv [argn], "-m"))
            *argv_index_metadata = ++argn;
        else
        if (streq (argv [argn], "--write")
        ||  streq (argv [argn], "-w"))
            *argv_index_write_mode = ++argn;
        else
        if (streq (argv [argn], "--kvalue")
        ||  streq (argv [argn], "-k"))
            *argv_index_k = ++argn;
        else
        if (streq (argv [argn], "--palette")
        ||  streq (argv [argn], "-p"))
            *argv_index_palette = ++argn;
        else
        {
            /* print error message */
            printf("Unknown option. Get help: ./%s -?\n", PROGRAM_NAME);

            /* program error exit code */
            /* 22 	EINVAL 	Invalid argument */
            return EINVAL;
        }
    }


    // --------------------------------------------------------------------------
    // check that image input was given

    if(*argv_index_input == -1)
    {
        /* print error message */
        printf("No image input path specified. Get help: ./%s -?\n", PROGRAM_NAME);

        /* program error exit code */
        /* 22 	EINVAL 	Invalid argument */
        return EINVAL;
    }


#if 0
    // --------------------------------------------------------------------------
    // check that metadata was given (even though we don't use it for v1... for now)

    if(*argv_index_metadata == -1)
    {
        /* print error message */
        printf("No metadata file path specified. Get help: ./%s -?\n", PROGRAM_NAME);

        /* program error exit code */
        /* 22 	EINVAL 	Invalid argument */
        return EINVAL;
    }
#endif

    // --------------------------------------------------------------------------
    // check that the k-means k value was given

    if(*argv_index_k == -1)
    {
        /* print error message */
        printf("No k-means k value specified. Get help: ./%s -?\n", PROGRAM_NAME);

        /* program error exit code */
        /* 22 	EINVAL 	Invalid argument */
        return EINVAL;
    }


    /* success */
    return 0;
}


// --------------------------------------------------------------------------
// build file name output string (the file name of the output image that will be written)

int build_image_output_filename(int write_mode, char* inimg_filename, char *outimg_filename, char *image_file_ext)
{
    switch(write_mode)
    {
        case 1: /* write a new image as a new file */
            
            /* create new file name for the output image file */
            strncpy(outimg_filename, inimg_filename, strcspn(inimg_filename, "."));
            strcat(outimg_filename, ".segmented.");
            strcat(outimg_filename, image_file_ext);

            break;

        case 2: /* write a new image that overwrites the input image file */
            
            /* use existing input image file name as the the output image file name */
            strcpy(outimg_filename, inimg_filename);
            
            break;

        case 3: /*  write a new image that overwrites the input image file but back up the original input image */

            
            char inimg_filename_new[100] = {0};
            strncpy(inimg_filename_new, inimg_filename, strcspn(inimg_filename, "."));
            strcat(inimg_filename_new, ".original.");
            strcat(inimg_filename_new, image_file_ext);
            rename(inimg_filename, inimg_filename_new);

            /* use existing input image file name as the output image file name */
            strcpy(outimg_filename, inimg_filename);

            break;
    }

    /* success */
    return 0;
}


// --------------------------------------------------------------------------
// the main function

int main(int argc, char **argv)
{
    /* the return code */
    int rc = 0;

    /* get provider host and port from command arguments */
    int argv_index_input = -1;
    int argv_index_metadata = -1;
    int argv_index_write_mode = -1;
    int argv_index_k = -1;
    int argv_index_palette = -1;

    /* parse the program options */
    rc = parse_options(argc, argv,
        &argv_index_input, &argv_index_metadata,
        &argv_index_write_mode, &argv_index_k,
        &argv_index_palette);

    /* error check */
    if(rc != 0)
    {
        /* there was an error */
        /* end of program */
        return rc;
    }


    /* the filename of the input image */
    char *inimg_filename = argv[argv_index_input];

    /* get the extension of the image file */
    char image_file_ext[10] = {0};
    strncpy(image_file_ext,
        inimg_filename + strcspn(inimg_filename, ".") + 1,
        strlen(inimg_filename) - strcspn(inimg_filename, "."));


    /* check that write mode was given and if not set it to default value */
    /* the write mode variable */
    int write_mode;

    /* check that write mode was given and if not set it to default value */
    if(argv_index_write_mode == -1)
    {
        /* default write mode is 0 */
        write_mode = 0;
    }
    else
    {
        /* cast given write mode option to integer */
        write_mode = atoi(argv[argv_index_write_mode]);

        if(write_mode < 0 || write_mode > 3)
        {
            /* print error message */
            printf("Invalid write mode option. Get help: ./%s -?\n", PROGRAM_NAME);

            /* program error exit code */
            /* 22 	EINVAL 	Invalid argument */
            return EINVAL;
        }
    }

    /* check that the k-means k value is valid */
    int k = atoi(argv[argv_index_k]);
    if(k < 2)
    {
        /* print error message */
        printf("Invalid k-means k value option. Get help: ./%s -?\n", PROGRAM_NAME);

        /* program error exit code */
        /* 22 	EINVAL 	Invalid argument */
        return EINVAL;
    }

    /* check that a palette was given and if not then get a random one */
    char palette[10] = {0};
    if(argv_index_palette == -1)
    {
        srand(time(NULL));
        int random_palette_index = rand() % (PALETTE_COUNT - 1); /* -1 because we don't want the black and white palette */
        strcpy(palette, PALETTES[random_palette_index]);
    }
    else
    {
        strcpy(palette, argv[argv_index_palette]);
    }

    /* these properties from the input image will be read from the file when invoking stbi_load */
    int img_xsize;
    int img_ysize;
    int img_channels_infile;

    /* decode the image file */
    uint8_t *img_buffer = (uint8_t*)stbi_load(inimg_filename, &img_xsize, &img_ysize, &img_channels_infile, STBI_rgb);

    /* build file name output string (the file name of the output image that will be written) */
    char outimg_filename[100] = {0};
    rc = build_image_output_filename(write_mode, inimg_filename, outimg_filename, image_file_ext);

    /* error check */
    if(rc != 0)
    {
        /* there was an error */
       
        /* free the input image data buffer */
        stbi_image_free(img_buffer);

        /* end of program */
        return rc;
    }

    /* the size of the image buffer data */
    int img_buffer_size = img_xsize * img_ysize * STBI_rgb;

    /* declare the vectir that will contain the image RGB data to use as a training data */
    std::vector<std::array<float, STBI_rgb>> training_data_vector;

    /* populate the training data vector with RGB arrays */
    for(int i = 0; i < img_buffer_size;)
    {
        /* The array that will contain RGB data for each pixel of the image */
        std::array<float, STBI_rgb> pixel_rgb_array;

        /* build the pixel array */
        /* three iterations of this loop: Red, Green, and Blue pixel values */
        for(int p = 0; p < STBI_rgb; p++)
        {
            /* populate pixel RGB array with pixel color values */
            pixel_rgb_array.at(p) = (NORMALIZE == 1) ? ((int)img_buffer[i]) / 255.0 : (float)img_buffer[i];
            
            /* increment the img buffer dat index */
            i++;
        }
        
        /* push the pixel RGB array into the training data vector */
        training_data_vector.push_back(pixel_rgb_array);
    }
    
       
    /* this will contain the cluster data */
    std::tuple<std::vector<std::array<float, STBI_rgb>>, std::vector<uint32_t>> cluster_data;

    /* use K-Means Lloyd algorithm to build clusters */ 
    cluster_data = dkm::kmeans_lloyd<float, STBI_rgb>(training_data_vector, k);

    /* cloud pixel count if we are doing cloud coverage image segmentation */
    uint32_t cloud_pixel_count = 0;

    /* write the output image file */
    if(write_mode != 0)
    {
        /* image buffer index */
        int seg_index = 0;  

        /* if k=2 and palette is Black & White (BW) then make sure that pixels close to black are clustered in black and the rest in white */
        /* this is necessary because there is no guarantee which label the training algorithm will end up applying to white leaning vs black leaning pixels */
        uint32_t black_label = 0;

        /* if random palette was selected then use this paletter vector */
        std::vector<std::array<uint8_t, STBI_rgb>> PALETTE_Random;

        if(streq(palette, PALETTES[0])) /* random palette */
        {
            generate_random_rgb_palette(k, PALETTE_Random);
        }
        else if(streq(palette, PALETTES[5])) /* Black & White palette */
        {
            /* make a mock prediction to determine which label is associated with black pixels */
            std::vector<std::array<float, STBI_rgb>> centroids = std::get<0>(cluster_data);
            std::array<float, STBI_rgb> query {0, 0, 0};

            /* the label that is associated with black pixels */
            /* use this later when  determining if a black or white color should be applied to a given label */
            black_label = dkm::predict(centroids, query);
        }

        /* for each label in the clustered data */
        for (uint32_t label : std::get<1>(cluster_data))
        {

            /* get the pixel rbg for the label from the selected palette */
            std::array<uint8_t, STBI_rgb> rgb;
            
            /* TODO: make this index based */
            if(streq(palette, PALETTES[0]))
            {
                rgb = PALETTE_Random.at(label);
            }
            else if(streq(palette, PALETTES[1]))
            {
                rgb = PALETTE_RdYlBu.at(label);
            }
            else if(streq(palette, PALETTES[2]))
            {
                rgb = PALETTE_PiYG.at(label);
            }
            else if(streq(palette, PALETTES[3]))
            {
                rgb = PALETTE_Paired.at(label);
            }
            else if(streq(palette, PALETTES[4]))
            {
                rgb = PALETTE_Set3.at(label);
            }
            else if(streq(palette, PALETTES[5])) /* Black & White palette */
            {
                /* if k=2 and palette is Black & White then make sure that pixels close to black are clustered in black and the rest in white */
                /* this is necessary because there is no guarantee which label the training algorithm will end up applying to white leaning vs black leaning pixels */
                if(label == black_label)
                {
                    rgb = PALETTE_BW.at(0);
                }
                else
                {
                    rgb = PALETTE_BW.at(1);
                }
            }
            else
            {
                rgb = PALETTE_Set3.at(label);
            }

            /* set palette RGB as pixel data in the segmented image data buffer */
            /* reuse the same image buffer that was used to read the input image */
            img_buffer[seg_index] = std::get<0>(rgb);
            seg_index++;

            img_buffer[seg_index] = std::get<1>(rgb);
            seg_index++;

            img_buffer[seg_index] = std::get<2>(rgb);
            seg_index++;

            /* count total cloud pixels if we are doing cloud coverage */
            /* will use this later to calculate cloud coverage */
            if(streq(palette, PALETTES[5]) && std::get<0>(rgb) == 255)
            {
                cloud_pixel_count++;
            }
        }

        if(streq(image_file_ext, "png"))
        {
            stbi_write_png(outimg_filename, img_xsize, img_ysize, STBI_rgb, (void*)img_buffer, 0);
        }
        else if(streq(image_file_ext, "jpeg"))
        {
            stbi_write_jpg(outimg_filename, img_xsize, img_ysize, STBI_rgb, (void*)img_buffer, JPEG_WRITE_QUALITY);
        }
        else
        {
            /* print error message */
            printf("Image format not supported: %s.", image_file_ext);

            /* program error exit code */
            /* 22 	EINVAL 	Invalid argument */
            rc = EINVAL;
        }
    }

    /* free the input image data buffer */
    stbi_image_free(img_buffer);

#if TARGET_BUILD_OPSSAT /* this logic is specific to the OPS-SAT spacecraft */

    /* return the classification json object string */

    if(streq(palette, PALETTES[5])) /* if we are doing cloud coverage */
    {
        /* calculate code coverage */
        float cloud_coverage = (float)cloud_pixel_count / (float)(img_xsize * img_ysize);

        /* mark which 100% confidence which label to apply to the image */
        uint8_t cloudy_0_25 = cloud_coverage >= 0 && cloud_coverage <= 0.25 ? 1 : 0;
        uint8_t cloudy_26_50 = cloud_coverage > 0.25 && cloud_coverage <= 0.50 ? 1 : 0;
        uint8_t cloudy_51_75 = cloud_coverage > 0.50 && cloud_coverage <= 0.75 ? 1 : 0;
        uint8_t cloudy_76_100 = cloud_coverage > 0.75 ? 1 : 0;

        /* create classification result json object */
        printf("{");
        printf("\"cloudy_0_25\": %d, ", cloudy_0_25);
        printf("\"cloudy_26_50\": %d, ", cloudy_26_50);
        printf("\"cloudy_51_75\": %d, ", cloudy_51_75);
        printf("\"cloudy_76_100\": %d, ", cloudy_76_100);
        printf("\"features\": 0, ");
        printf("\"_cloud_coverage\": %f", cloud_coverage);  /* prefixed by an underscore means it's metadata, not a label */
        printf("}");
    }
    else /* if we are doing feature extraction */
    {
        printf("{"
            "\"cloudy_0_25\": 0, "
            "\"cloudy_26_50\": 0, "
            "\"cloudy_51_75\": 0, "
            "\"cloudy_76_100\": 0, "
            "\"features\": 1"
        "}");
    }
#endif

    /* end program */
    return rc;
}