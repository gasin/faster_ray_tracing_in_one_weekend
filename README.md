# Faster Ray Tracing in One Weekend

make Ray Tracing in One Weekend Faster

## Setting
### Constraint
- no GPU
- no multithreading
- no specification changes
  - importance sampling, etc

### Environment
- CPU
  - Intel Core i7-13700KF
- compiler
  - `g++.exe (Rev2, Built by MSYS2 project) 13.2.0`

## Log

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
