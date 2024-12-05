# Efficient Low-Storage 3D Seismic Simulation Using Curved Grid Finite-Difference Method (CGFDM)

## Overview

High-resolution three-dimensional (3D) seismic simulation imposes severe demands for computational memory, making low-storage seismic simulation particularly important. Due to its high efficiency and low storage, the half-precision floating-point 16-bit format (FP16) is widely used in heterogeneous computing platforms, such as Sunway series supercomputers and graphics processing unit (GPU) computing platforms.

This project introduces FP16 and the Low-Storage Runge-Kutta (LSRK) technique to reduce memory usage. It also derives new elastic wave equations by introducing three dimensionless constants, \(C_v\), \(C_s\), and \(C_p\), to normalize the magnitude of velocity and stress variables, ensuring they remain within FP16's representable range. These techniques enable the development of optimized multi-GPU solvers for seismic simulation using the curved grid finite-difference method (CGFDM).

## Key Features

- **FP16 for Low Storage**:
  - Reduces memory usage while maintaining computational efficiency.
- **Low-Storage Runge-Kutta (LSRK)**:
  - Further reduces memory demand compared to classical Runge-Kutta methods.
- **Magnitude Normalization**:
  - Keeps physical quantities within FP16 range using \(C_v\), \(C_s\), and \(C_p\).
- **Optimized Multi-GPU Solvers**:
  - Improves performance for large-scale seismic simulations.
- **Memory Usage Reduction**:
  - Reduces memory usage by up to 1/3 compared to the original CGFDM solver.

## Validation

The optimized solver has been verified through a series of seismic simulations, demonstrating:

- High computational accuracy and stability.
- Significant optimization of computational efficiency.
- Effective memory usage reduction.

## Citation

Wenqiang Wang, Zhenguo Zhang, Wenqiang Zhang, and Qi Liu. "Implementation of efficient low-storage techniques for 3-D seismic simulation using the curved grid finite-difference method," _Geophysical Journal International_, 2023. [DOI:10.1093/gji/ggad198](https://doi.org/10.1093/gji/ggad198)
