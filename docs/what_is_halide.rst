What is Halide? Why should I use it?
================================================================

*Halide* is a compiler focuses on efficiently expressing and compiling array computation, for image processing, computer vision, scientific computation, and machine learning. It provides concise syntax, along with powerful code optimization functionality to achieve both ease of programming and high performance. It also provides automatic gradient generation feature for machine learning and optimization applications.

Modern tensor manipulation frameworks such as Tensorflow, PyTorch, Numpy, or Matlab provides efficient linear algebra building blocks, such as convolution or matrix multiplication, for the users to build their applications. When limited to their walled-gardens of pre-made, coarse-grained operations, these frameworks provide high-performance kernel implementations and automatic differentiation through chains of operations. As general programming languages, however, they are a poor fit for many array programs. Building new algorithms requires contorting a problem into existing building blocks. Even when done successfully, the resulting implementation is often both slow and memory-inefficient, saving and reloading entire arrays of intermediate results between each step, causing costly cache misses. As a result, practitioners often have to write low-level C or CUDA code as custom operators. Furthermore, it becomes even more tedious when the code needs to map to different platforms, such as non-NVIDIA GPUs, mobile CPUs, DSPs, etc.

Halide resolves this by decoupling the computation into *algorithm* and *schedule*, where the algorithm represents the high-level intention, and the schedule represents the low-level code optimization. This allows the compiler to build explicit *loops* and transform them to generate general and high efficiency code, while the user can focus on the high-level algorithm. Halide provides automatic scheduler that can generate schedules for the users, or the user can choose to write their own schedules. The compiler takes the algorithm and schedule, generate code for different backends.

Currently, Halide is embedded in both C++ and Python, and targets:

- CPU architectures: X86, ARM, MIPS, Hexagon, PowerPC

- Operating systems: Linux, Windows, Mac OS X, Android, iOS, Qualcomm QuRT

- GPU Compute APIs: CUDA, OpenCL, OpenGL, OpenGL Compute Shaders, Apple Metal, Microsoft Direct X 12

To give you a quick taste of what Halide looks like, here is what a 2D box filter looks like in Halide:

.. tabs::

   .. code-tab:: c++

    Func blur_3x3(Func input) {
      Func blur_x, blur_y;
      Var x, y, xi, yi;

      // The algorithm - no storage or order
      blur_x(x, y) = (input(x-1, y) + input(x, y) + input(x+1, y))/3;
      blur_y(x, y) = (blur_x(x, y-1) + blur_x(x, y) + blur_x(x, y+1))/3;

      // The schedule - defines order, locality; implies storage
      blur_y.tile(x, y, xi, yi, 256, 32)
            .vectorize(xi, 8).parallel(y);
      blur_x.compute_at(blur_y, x).vectorize(x, 8);

      return blur_y;
    }

   .. code-tab:: py

    def blur_3x3(input) {
      blur_x, blur_y = hl.Func(), hl.Func()
      x, y, xi, yi = hl.Var(), hl.Var(), hl.Var(), hl.Var()

      # The algorithm - no storage or order
      blur_x[x, y] = (input[x-1, y] + input[x, y] + input[x+1, y])/3
      blur_y[x, y] = (blur_x[x, y-1] + blur_x[x, y] + blur_x[x, y+1])/3

      # The schedule - defines order, locality; implies storage
      blur_y.tile(x, y, xi, yi, 256, 32)
            .vectorize(xi, 8).parallel(y)
      blur_x.compute_at(blur_y, x).vectorize(x, 8)

      return blur_y
    }
