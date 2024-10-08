# MCA

The official implementation of *Microimage Cropping and Aligning*.

## Usage

```shell
mca-preproc [param_file_path]
mca-postproc [param_file_path]
```

## Configuration File Format

### Options

+ `pipeline (int)` The image is captured by Raytrix (0) or TSPC (1).
+ `Calibration_xml (str)` The path of the calibration file.
+ `RawImage_Path (str)` The glob pattern of the input images in C-printf style, filled with the frame index.
+ `Output_Path (str)` The glob pattern of the output images in C-printf style, filled with the frame index.
+ `start_frame (int)` The index of the start frame, left contains.
+ `end_frame (int)` The index of the end frame, right contains.
+ `width (int)` The pixel width of input image.
+ `height (int)` The pixel height of input image.
+ `crop_ratio (float)` The ratio between cropping width and diameter of Micro Image.

### Example

```
pipeline           1
Calibration_xml    Boys.xml
RawImage_Path      Boys/src/image_%d.png
Output_Path        Boys/dst
start_frame        1
end_frame          300
height             3068
width              4080
crop_ratio         0.7071067811865475
```

### Charset

Must be utf-8!

## Compile Options

See `cmake/options.cmake`

## Related Document

[(m62932) Focused Plenoptic Image Analysis and Sub-aperture Image lossless Preprocessing Scheme](https://dms.mpeg.expert/doc_end_user/current_document.php?id=86981)
