# K-Means Image Segmentation
K-Means image segmentation (feature extraction) that just works. Lightweight and low footprint C++ implementation. 

## Build
1. Initialize and update the Git submodules: `git submodule init && git submodule update`.
2. Compile with `make`. Can also compile for ARM architecture with `make TARGET=arm`.

## Example Usage
`./image_segmentation -i samples/img_msec_1607377664281_2_thumbnail.jpeg -w 1 -k 7 -p Set3`