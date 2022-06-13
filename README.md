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
