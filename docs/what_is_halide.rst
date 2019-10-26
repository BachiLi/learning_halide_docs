What is Halide? Why should I use it?
================================================================

*Halide* is a compiler focuses on efficiently expressing and compiling array computation, for image processing, computer vision, scientific computation, and machine learning. It provides concise syntax, along with powerful code optimization functionality to achieve both ease of programming and high performance. It also provides automatic gradient generation feature for machine learning and optimization applications.

Modern tensor manipulation frameworks such as Tensorflow, PyTorch, Numpy, or Matlab provides efficient linear algebra building blocks, such as convolution or matrix multiplication, for the users to build their applications. When limited to their walled-gardens of pre-made, coarse-grained operations, these frameworks provide high-performance kernel implementations and automatic differentiation through chains of operations. As general programming languages, however, they are a poor fit for many array programs. Building new algorithms requires contorting a problem into existing building blocks. Even when done successfully, the resulting implementation is often both slow and memory-inefficient, saving and reloading entire arrays of intermediate results between each step, causing costly cache misses. As a result, practitioners often have to write low-level C or CUDA code as custom operators. Furthermore, it becomes even more tedious when the code needs to map to different platforms, such as non-NVIDIA GPUs, mobile CPUs, DSPs, etc.

Halide resolves this by decoupling the computation into *algorithm* and *schedule*, where the algorithm represents the high-level intention, and the schedule represents the low-level code optimization. This allows the compiler to build explicit loops and transform them to generate general and high efficiency code, while the user can focus on the high-level algorithm. Halide provides automatic scheduler that can generate schedules for the users, or the user can choose to write their own schedules. The compiler takes the algorithm and schedule, generate code for different backends.

Currently, Halide is embedded in both C++ and Python, and targets:

- CPU architectures: X86, ARM, MIPS, Hexagon, PowerPC

- Operating systems: Linux, Windows, Mac OS X, Android, iOS, Qualcomm QuRT

- GPU Compute APIs: CUDA, OpenCL, OpenGL, OpenGL Compute Shaders, Apple Metal, Microsoft Direct X 12

To give you a quick taste of what Halide looks like, here is what a 3x3 2D box filter looks like in Halide algorithm:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Func blur_3x3(Func input) {
              Func blur_x, blur_y;
              Var x, y, c;

              blur_x(x, y, c) = (input(x-1, y, c) + input(x, y, c) + input(x+1, y, c))/3;
              blur_y(x, y, c) = (blur_x(x, y-1, c) + blur_x(x, y, c) + blur_x(x, y+1, c))/3;

              return blur_y;
            }

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            def blur_3x3(input) {
              blur_x, blur_y = hl.Func(), hl.Func()
              x, y, xi, yi = hl.Var(), hl.Var(), hl.Var(), hl.Var()

              blur_x[x, y] = (input[x-1, y] + input[x, y] + input[x+1, y])/3
              blur_y[x, y] = (blur_x[x, y-1] + blur_x[x, y] + blur_x[x, y+1])/3

              return blur_y
            }

Halide can either automatically generates the schedule for the code above, or the user can choose to specify the schedule manually, which looks like below:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Var xi, yi
            blur_y.tile(x, y, xi, yi, 256, 32)
                .vectorize(xi, 8).parallel(y);
            blur_x.compute_at(blur_y, x).vectorize(x, 8);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            xi, yi = hl.Var(), hl.Var()
            blur_y.tile(x, y, xi, yi, 256, 32)
                .vectorize(xi, 8).parallel(y)
            blur_x.compute_at(blur_y, x).vectorize(x, 8)

The schedule optimizes the storage, order, and paralellism of the computation.

Comparing to the following PyTorch implementation of the same 3x3 box filter:

.. tabs::

   .. tab:: PyTorch

        .. code-block:: py

            def blur_3x3(input) {
              input = input.unsqueeze(3)
              kernel = torch.ones(3, 1, 1, 3)
              blur_x = torch.nn.functional.conv2d(input, kernel, groups=3)
              kernel = kernel.permute(0, 1, 3, 2)
              blur_y = torch.nn.functional.conv2d(blur_x, kernel, groups=3)
              return blur_y
            }

While the line counts are similar, the PyTorch code reveals several issues of popular tensor frameworks. First, it assumes the images always come with the batch dimension, so we have to unsqueeze it. Second, we have to create a kernel with size of 9 for group convolution. Third, because there is no specialized version of conv2d, PyTorch is not able to optimize out the constant kernel. Finally, we need to allocate several intermediate buffers for the computation, making the computation slower than necessary.

Benchmarking on an Intel i7-6900K CPU and a NVIDIA Titan Xp GPU, we get the following running time on a 2560x1536x3 image:

===========  =========
Halide CPU   3.426 ms
PyTorch CPU  95.240 ms
Halide GPU   0.264 ms
PyTorch GPU  1.894 ms
===========  =========

Halide is **27.8x** faster on CPU and **7x** faster on GPU, thanks to the better scheduling.

Halide is not intended to replace PyTorch or Tensorflow though. It is a complement to them: when you found a case where efficient implementation in PyTorch or Tensorflow is difficult, you should consider using Halide, instead of diving in to the low-level C++/CUDA implementation. Another use case of Halide is when you want to write the same algorithm, but compile it to different hardwares. For example, you can write the code in Halide, train your algorithm on GPU, then retarget your code on to mobile systems.
