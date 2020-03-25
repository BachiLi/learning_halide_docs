Learning to Write Halide Algorithms by Examples
================================================================

In the last tutorial we learned how to construct Halide functions,
which describe computation on infinite multi-dimensional grids. In
this tutorial, we will learn how to use these Halide functions to
construct more complex Halide algorithms, including how to connect
different Halide functions and how to write loops and conditions
inside a Halide function. 

.. note::
    Remember that Halide separates code into **algorithms** and **schedules**.
    In this tutorial we focus on the algorithms and use the default
    schedules. This means that if you execute the programs listed here
    they will be slow. Read the subsequent tutorials on how to make them fast.

Matrix multiplication
---------------------

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            ImageParam A(Float(32), 2, "A"), B(Float(32), 2, "B");
            Func C("C");
            Var i("i"), j("j");
            C(i, j) = 0.f;
            RDom k(0, A.dim(1).extent(), "k"); // inner loop
            C(i, j) += A(i, k) * B(k, j);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            A, B = hl.ImageParam(hl.Float(32), 2, "A"), hl.ImageParam(hl.Float(32), 2, "B")
            C = hl.Func("C")
            i, j = hl.Var("i"), hl.Var("j")
            C[i, j] = 0.0
            k = hl.RDom(0, A.dim(1).extent(), "k") # inner loop
            C[i, j] += A[i, k] * B[k, j]

The first example we will look at is a matrix multiplication Halide program:
given two matrices ``A`` and ``B`` we want to compute ``C = A * B``.

The Halide code above roughly translates to the following pseudo code:

.. code-block:: c++

    for i = 0 to A.rows()
        for j = 0 to B.cols()
            C(i, j) = 0.f
            for k = 0 to A.cols()
                C(i, j) += A(i, k) * B(k, j)

We have previously introduced Halide variables ``Var``. When we write ``C(i, j) = 0.f``
we can think of it as Halide implicitly constructing loops that traverse over ``C`` 's
elements. How do we introduce the inner loop that sum over entries from ``A`` and ``B``
then? We declare a ``RDom`` (which stands for *reduction domain*) construct ``k`` to loop
over ``A`` 's columns. Also note that we defined ``C`` twice. In the first time we initialize
it to zero over the whole domain, and the second time we update ``C`` to be the sum over
``A`` 's rows and ``B`` 's columns.

Image convolution
---------------------

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            ImageParam input(Float(32), 2, "in"), kernel(Float(32), 2, "k");
            Var x("x"), y("y");
            Func bounded_input("input");
            bounded_input(x, y) = input(clamp(x, 0, input.dim(0).extent()),
                                        clamp(y, 0, input.dim(1).extent()));
            Func output("output");
            RDom r(0, kernel.dim(0).extent(), 0, kernel.dim(1).extent());
            output(x, y) += bounded_input(x - r.x + kernel.dim(0).extent() / 2,
                                          y - r.y + kernel.dim(1).extent() / 2) *
                            kernel(r.x, r.y);

   .. tab:: Halide (Python frontend)

        .. code-block:: py

            input = hl.ImageParam(hl.Float(32), 2, 'in')
            kernel = hl.ImageParam(hl.Float(32), 2, 'k')
            x, y = hl.Var('x'), hl.Var('y')
            bounded_input = hl.Func('input')
            bounded_input[x, y] = input[hl.clamp(x, 0, input.dim(0).extent()),
                                        hl.clamp(y, 0, input.dim(1).extent())];
            output = hl.Func('output');
            r = hl.RDom(0, kernel.dim(0).extent(), 0, kernel.dim(1).extent());
            output[x, y] += bounded_input[x - r.x + kernel.dim(0).extent() / 2,
                                          y - r.y + kernel.dim(1).extent() / 2] *
                            kernel[r.x, r.y];

Image convolution can also be described using reduction domains. There are two crucial differences
compared to the matrix multiplication.

Firstly, the reduction domain becomes 2D. We can use ``r.x`` and ``r.y`` to access different
dimensions of the reduction domains. Alternatively we can use ``r[0]`` or ``r[1]``.

Secondly, the convolution filter can read from outside the bounds of the input. Therefore we need
to define a *boundary condition* for our input. This is done through the ``bounded_input`` function.
We define the out-of-bound access to the closest pixel based on the coordinate, and this is achieved
through clamping the coordinates. Since this pattern is very common, Halide provides a syntatic sugar
for this:

.. tabs::

   .. tab:: Halide (C++ frontend)

       .. code-block:: c++

            Func bounded_input = BoundaryConditions::repeat_edge(input);


   .. tab:: Halide (Python frontend)

        .. code-block:: py

            bounded_input = hl.BoundaryConditions.repeat_edge(input)

Check out Halide's documentation for different kinds of boundary conditions.