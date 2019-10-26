Hello, Halide!
================================================================

Halide employs a static metaprogramming model similar to Tensorflow or `Terra <http://terralang.org/>`_. You write C++ or Python programs that construct Halide *functions* that describe the algorithm. Halide then transforms these functions into lower-level code based on the schedules. This has two benefits. First, it allows the compiler frontend to be embedded in a popular language. This makes interaction of Halide with these language easier, and allows metaprogramming Halide functions using the host language. Second, in contrast to *interpreting* a dynamic computation graph, the static model enables Halide to generate more efficient code by employing compiler optimizations such as constant propagation and common subexpression elimination ahead of the time.

To use Halide in your program, you would need to include the Halide header in C++ or import the Halide module in Python:

.. tabs::

   .. tab:: Halide (C++ frontend)

        .. code-block:: c++

            #include <Halide.h>

            using namespace halide;

   .. tab:: Halide (Python frontend)

        .. code-block:: py
        
            import halide as hl

The following code *declares* a Halide function ``f``, that does not has a *definition* yet:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Func f("f");

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            f = hl.Func('f')

The constructor of ``Func`` takes an optional name argument, that is useful for debugging and pretty printing.

Each Halide function describes an *infinite* multi-dimensional domain of values. This means that, when accessing a Halide function, it always returns some value. This has the benefit of memory safety -- Halide guarantees that you can never have buffer overrun issues that cause segmentation faults. This relieves the programmers from having to worry about the boundaries of a computation.

To describe the multi-dimensional domain, we need to name the coordinates. ``Var`` is designed for this:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Var x("x"), y("y");
            // Initializes f to 5 everywhere
            f(x, y) = 5.0f;

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            x, y = hl.Var('x'), hl.Var('y')
            # Initializes f to 5 everywhere
            # Python has no single-precision floating point,
            # we use hl.f32 for the conversion.
            f[x, y] = hl.f32(5.0)

You can also define a *0-dimensional* Halide function for representing a scalar:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Func g("g");
            g() = 5.0f;

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            g = hl.Func('g');
            g[()] = hl.f32(5.0)

Keep in mind that you are metaprogramming Halide functions -- at this point there is no ``f`` or ``g`` actually allocated and store 5 everywhere.

To actually realize ``f``, you need to call ``f.realize``. This generates a ``Buffer`` that has a finite extent and actual values inside:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            // Generate a buffer with width 4 and height 6.
            Buffer<float> b = f.realize(4, 6);
            for (int j = 0; j < b.dim(1).extent(); j++) {
                for (int i = 0; i < b.dim(0).extent(); i++) {
                    // Should be 5 everywhere
                    std::cout << "b(" << i << ", " << j << "):" << b(i, j) << std::endl;
                }
            }

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            # Generate a buffer with width 4 and height 6
            b = f.realize(4, 6)
            for j in range(b.dim(1).extent()):
                for i in range(b.dim(0).extent()):
                    print('b({}, {}) = {}'.format(i, j, b(i, j)))

To do interesting computation Halide needs to take some inputs. Inputs can also represented by ``Buffer``:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Buffer<float> input(640, 480, 3);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            input = hl.Buffer(type = hl.Float(32), sizes = (640, 480, 3))
            # hl.Buffer also plays well with numpy arrays
            import numpy as np
            input2 = hl.Buffer(np.zeros(640, 480, 3))

Alternatively they can be an ``ImageParam`` when you do not know the size of the input in advance:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            ImageParam input(Float(32), 3 /* num. of dimensions */);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            # Construct an ImageParam with 3 dimensions
            input = hl.ImageParam(hl.Float(32), 3)

``Buffer`` and ``ImageParam`` can be accessed by ``Func``. Here we define a Func that make the input two times brighter.

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Func f("f");
            Var x("x"), y("y"), c("c");
            f(x, y, c) = 2 * input(x, y, c);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            f = hl.Func("f")
            x, y, c = hl.Var('x'), hl.Var('y'), hl.Var('c')
            f[x, y, c] = 2 * input[x, y, c]

Now we want to evaluate the Func. If you are using ``ImageParam``, you need to setup the content using ``Buffer`` first:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Buffer<float> b(640, 480, 3);
            input.set(b)

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            b = hl.Buffer(hl.Float(32), (640, 480, 3));
            input.set(b)

And we can realize the function like before:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Buffer<float> b = f.realize(640, 480, 3);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            b = f.realize(640, 480, 3)
