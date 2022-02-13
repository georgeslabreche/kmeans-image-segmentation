# K-Means Image Segmentation
K-Means image segmentation (feature extraction) that just works. Lightweight and low footprint C++ implementation. 

## Build
1. Initialize and update the Git submodules: `git submodule init && git submodule update`.
2. Compile with `make`. Can also compile for ARM architecture with `make TARGET=arm`.

This program was specifically created for the European Space Agency's [OPS-SAT Spacecraft](https://opssat1.esoc.esa.int/) to enable in-orbit cloud detection and feature extraction with pictures acquired by the on-board camera. As such, the program's stdout is specific to OPS-SAT's needs. However, this specificity can be disabled by setting `TARGET_BUILD_OPSSAT` to 0 in **main.cpp**:

```cpp
/* indicate if we are building for the OPS-SAT spacecraft or not */
#define TARGET_BUILD_OPSSAT         0
```

Alternatively, developers are encouraged to fork this repo and remove OPS-SAT related logic.

## Usage
Easy to use!

### Help
```bash
$ ./image_segmentation -?
image_segmentation [options] ...
  --input    / -i       the file path of the input image
  --write    / -w       the write mode of the output image (optional)
        0 - do not write a new image (equivalent to not specifying --write)
        1 - write a new image as a new file
        2 - write a new image that overwrites the input image file
        3 - same as option 2 but backs up the original input image
  --kvalue   / -k       the k-means k value (2 to 11)
  --palette  / -p       the palette to use for segmentation (optional)
  --help     / -?       this information
```

Palette options (case sensitive):
- Random
- RdYlBu
- PiYG
- Paired
- Set3
- BW

Refer to [colorbrewer2.org](https://colorbrewer2.org/) for palette colors. Supported range for k values are 2 to 11 for palettes except BW (k=2) and Random (k >= 2 with no ceiling).

### Example
`$./image_segmentation -i samples/img_msec_1607377664281_2_thumbnail.jpeg -w 1 -k 7 -p Set3`