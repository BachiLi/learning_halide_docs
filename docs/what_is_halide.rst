What is Halide? Why should I use it?
================================================================

*Halide* is a language and compiler focused on efficiently expressing and
compiling array computation for image processing, computer vision, scientific
computation, and machine learning. It provides a concise syntax and powerful
code optimizations to achieve both ease of use and high performance. It also
provides automatic gradient generation features for machine learning and
optimization applications.

Modern tensor frameworks such as Tensorflow, PyTorch, Numpy, or Matlab provide
linear algebra building blocks, such as convolution or matrix multiplication,
for users to build their applications. These frameworks provide
high-performance kernel implementations and automatic differentiation.
Unfortunately, building new algorithms requires contorting a problem into
the existing building blocks. Even when done successfully, the resulting
implementation is often both slow and memory-inefficient, saving and
reloading entire arrays of intermediate results between each step, thus
causing costly cache misses. As a result, practitioners often write
low-level C or CUDA code as custom operators. This process becomes even
more tedious when the code needs to port to different platforms, such as
non-NVIDIA GPUs, mobile CPUs, DSPs, etc.

Halide resolves this by decoupling the computation into the *algorithm* and
the *schedule*, where the algorithm represents the high-level intention, and the
schedule represents the low-level code optimization. This allows the compiler
to build explicit loops and transform them to generate general and high
efficiency code, while the user can focus on the high-level algorithm. Halide
provides automatic schedule generation, but the user can also choose to
write their own schedules. The compiler takes the algorithm and schedule,
and generates efficient code for different backends.

Currently, the Halide DSL is embedded in both C++ and Python, and targets:

- CPU architectures: X86, ARM, MIPS, Hexagon, PowerPC

- Operating systems: Linux, Windows, Mac OS X, Android, iOS, Qualcomm QuRT

- GPU Compute APIs: CUDA, OpenCL, OpenGL, OpenGL Compute Shaders, Apple Metal, Microsoft Direct X 12

To give you a quick taste of what Halide looks like, here is a 3x3 2D box
filter algorithm:

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

            def blur_3x3(input):
              blur_x, blur_y = hl.Func(), hl.Func()
              x, y, c = hl.Var(), hl.Var(), hl.Var()

              blur_x[x, y, c] = (input[x-1, y, c] + input[x, y, c] + input[x+1, y, c])/3
              blur_y[x, y, c] = (blur_x[x, y-1, c] + blur_x[x, y, c] + blur_x[x, y+1, c])/3

              return blur_y

Halide can either automatically generate the schedule for the code above, or
the user can choose to specify the schedule manually, such as in the example
below:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Var xi, yi;
            blur_y.split(y, y, yi, 8).parallel(y).vectorize(x, 8);
            blur_x.store_at(blur_y, y).compute_at(blur_y, yi).vectorize(x, 8);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            xi, yi = hl.Var(), hl.Var()
            blur_y.split(y, y, yi, 8).parallel(y).vectorize(x, 8)
            blur_x.store_at(blur_y, y).compute_at(blur_y, yi).vectorize(x, 8)

The schedule optimizes the storage, order, and paralellism of the computation.
Different backends would require different schedules to fully account for the
differences in hardware.

Compare this to the following PyTorch implementation of the same 3x3 box filter:

.. tabs::

   .. tab:: PyTorch

        .. code-block:: py

            def blur_3x3(input):
              input = input.unsqueeze(3)
              kernel = torch.ones(3, 1, 1, 3) / 3
              blur_x = torch.nn.functional.conv2d(input, kernel, groups=3)
              kernel = kernel.permute(0, 1, 3, 2)
              blur_y = torch.nn.functional.conv2d(blur_x, kernel, groups=3)
              return blur_y

While the line counts are similar, the PyTorch code reveals several issues of
popular tensor frameworks:

- It assumes the images always come with the batch dimension, so we have to
  unsqueeze it.

- We have to create a kernel with size of 9 for group convolution, and we have
  to be familiar with the concept of group convolution to use it.

- Because there is no specialized version of conv2d, PyTorch is not able to
  optimize out the constant kernel.

- We need to allocate several intermediate buffers for the computation, making
  the computation slower than necessary.

Benchmarking on an Intel i7-6900K CPU and a NVIDIA Titan Xp GPU, we get the
following running time on a 2560 x 1536 x 3 image:

===========  =========
Halide CPU   3.426 ms
PyTorch CPU  95.240 ms
Halide GPU   0.251 ms
PyTorch GPU  1.894 ms
===========  =========

Halide is **27.8x** faster on CPU and **7.5x** faster on GPU, thanks to the
better scheduling.

However, Halide is not intended to be a replacement to PyTorch or Tensorflow.
It is a complement to them: when you find a case where efficient implementation
in PyTorch or Tensorflow is necessary but difficult, you should first consider
using Halide, instead of diving in to the low-level C++/CUDA implementation.

Another use case of Halide is achieving **performance portability**. You can
write the algorithm once and then compile and optimize it to different hardware
platforms by supplying specialized schedules. This allows you to, for example,
write your algorithm in Halide, train it on a GPU, then retarget it to mobile
systems.

