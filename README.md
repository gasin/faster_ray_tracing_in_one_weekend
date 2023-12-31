# Faster Ray Tracing in One Weekend

make Ray Tracing in One Weekend Faster

## Result

### Parameter
```cpp
    cam.image_width       = 1200;
    cam.samples_per_pixel = 10;
    cam.max_depth         = 10;
```

| version | running time (sec) |
| ---- | ---- |
| origin | 2m19.456s |
| add compile option | 0m24.352s |
| fetch bvh | 0m5.794s |


```cpp
    cam.image_width       = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 10;
```
| version | running time (sec) |
| ---- | ---- |
| fetch bvh | 4m22.333s |
| use volume to construct bvh | 3m29.432s |
| precalculate ray_inv | 3m16.294s |
| separate groud | 3m3.673s |
| stop early return in aabb hit | 2m55.751s |
| remove swap in aabb hit | 2m50.553s |
| use precise surface area heuristic | 2m39.093s |
| consider bvh traversal order | 2m27.991s |
| implement simple qbvh without simd | 2m10.904s |


```cpp
# Same settings as RayTracingInOneWeekend
    cam.image_width       = 1200;
    cam.samples_per_pixel = 500;
    cam.max_depth         = 50;
```
| version | running time (sec) |
| ---- | ---- |
| implement simple qbvh without simd | 2m14.838s |
| implement parallel aabb hit detection | 1m54.029s |
| precalculate parallel bbox | 1m47.622s |
| use xorshift instead of rand | 1m17.546s |

## Setting
### Constraint
- no GPU
- no multithreading
- no excessive exploit of input data
  - no compile-time calculation
  - no embedding

### Environment
- CPU
  - Intel Core i7-13700KF
- OS
  - Windows 11
- compiler
  - `g++.exe (Rev2, Built by MSYS2 project) 13.2.0`

## Usage
```bash
# build and run
# output to img.ppm
bash ./run.sh
```

### Profiling
```bash
cmake -B build
cmake --build build
./build/inOneWeekend_prof.exe > img.ppm
gprof ./build/inOneWeekend_prof.exe > prof.txt
```
