/* the color palette that will be used to write the segmented imanges */
/* get some pretty color palettes from here: https://colorbrewer2.org */

#define PALETTE_COUNT   6

/* palette names */
char PALETTES[PALETTE_COUNT][10] = {
    "Random",
    "RdYlBu",   /* 11 colors */
    "PiYG",     /* 11 colors */
    "Paired",   /* 11 colors */
    "Set3",     /* 11 colors */
    "BW"        /*  2 colors */
};

/* BW */
std::vector<std::array<uint8_t, STBI_rgb>> PALETTE_BW
{
    {0,0,0},
    {255,255,255}
};


/* RdYlBu */
std::vector<std::array<uint8_t, STBI_rgb>> PALETTE_RdYlBu
{
    {165,0,38},
    {215,48,39},
    {244,109,67},
    {253,174,97},
    {254,224,144},
    {255,255,191},
    {224,243,248},
    {171,217,233},
    {116,173,209},
    {69,117,180},
    {49,54,149}
};


/* PiYG */
std::vector<std::array<uint8_t, STBI_rgb>> PALETTE_PiYG
{
    {142,1,82},
    {197,27,125},
    {222,119,174},
    {241,182,218},
    {253,224,239},
    {247,247,247},
    {230,245,208},
    {184,225,134},
    {127,188,65},
    {77,146,33},
    {39,100,25}
};


/* Paired */
std::vector<std::array<uint8_t, STBI_rgb>> PALETTE_Paired
{
    {166,206,227},
    {31,120,180},
    {178,223,138},
    {51,160,44},
    {251,154,153},
    {227,26,28},
    {253,191,111},
    {255,127,0},
    {202,178,214},
    {106,61,154},
    {255,255,153}
};


/* Set3 */
std::vector<std::array<uint8_t, STBI_rgb>> PALETTE_Set3
{
    {141,211,199},
    {255,255,179},
    {190,186,218},
    {251,128,114},
    {128,177,211},
    {253,180,98},
    {179,222,105},
    {252,205,229},
    {217,217,217},
    {188,128,189},
    {204,235,197}
};


int generate_random_rgb_palette(int k, std::vector<std::array<uint8_t, 3>> &palette)
{
    srand(time(NULL));
    for(int i = 0; i < k; i++)
    {
        std::array<uint8_t, 3> rgb{(uint8_t)(rand() % 255), (uint8_t)(rand() % 255), (uint8_t)(rand() % 255)};
        palette.push_back(rgb);
    }

    return 0;
}