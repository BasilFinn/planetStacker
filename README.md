# PlanetStacker
The PlanetStacker software reads videos of planets passing a CCD sensor connected to a telescope. All the images get registered
and stacked which results into a better signal to noise ration. After the stacking a simple sharpening filter is applied
to the image.

# Results
<img src="https://github.com/BasilFinn/planetStacker/blob/master/img/saturn_before.png" width="400" height="300">
Raw image

<img src="https://github.com/BasilFinn/planetStacker/blob/master/img/saturn_stack.png" width="400" height="300">
stacked image

<img src="https://github.com/BasilFinn/planetStacker/blob/master/img/saturn_filter.png" width="400" height="300">
stacked and sharpend

# GUI Parameters
- Stack Threshold:
  Only images above a certain threshold get stacked on each other. This step is sorting out bad images.
- Sharpening Kernel Size:
- Sharpening Weight Org:
- Sharpening Weight Blurr:

# Tested With
> QT Creator 5.12.2 (Community) with gcc
> CMake 3.16.0
> OpenCV 4.1.2
> Linux 64bit
