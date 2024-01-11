## Clone and build

GLFW library is required

```
git clone --recurse-submodules https://github.com/Idutvuker/voxture.git voxture
cd voxture
mkdir build
cd build
cmake ..
make
make link_res
```

## Running the app
`./main builder` - run builder

`./main viewer` - run viewer

`./main benchmark` - run benchmark

## Options
`resources/config.txt` - paths to read/write data  

`src/common/constants.hpp` - global constants

## Images
![Saharov](https://github.com/Idutvuker/voxture/assets/6796129/b5741817-a3b3-4dd6-ba25-16c8ecd0d83a)

![Citywall](https://github.com/Idutvuker/voxture/assets/6796129/b1b0d043-c4ff-440f-831e-ffb008a72a0b)
