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

