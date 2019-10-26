Installing Halide
================================================================

For now, we recommend building Halide from source for the full functionality. Clone the repository to your local directory first:

.. code-block:: bash

	git clone https://github.com/halide/Halide#building-halide


Then follow the `building instruction <https://github.com/halide/Halide#building-halide>`_ in the official readme.

This tutorial also uses the Python bindings. To build the Python bindings, first build main Halide code. Then go to the ``python_bindings`` directory, and install the following requirements:

.. code-block:: bash

	$ pip install numpy scipy pillow imageio pybind11

Next do

.. code-block:: bash

	$ make

After building the Python bindings, we need to point Python to it so that we can ``import halide``. Add ``/path/to/halide/python_bindings/bin`` to your ``PYTHONPATH`` environment variable (e.g. ``export PYTHONPATH=/path/to/halide/python_bindings/bin:$PYTHONPATH``).

There is one more step. You need to install the autoscheduler. There are several autoschedulers in Halide but we will use the ``gradient_autoscheduler`` as it is currently the only autoscheduler that supports GPU scheduling. At the main directory of Halide:

.. code-block:: bash

	$ cd apps/gradient_autoscheduler
	$ make

And again add ``/path/to/halide/apps/gradient_autoscheduler/bin`` to ``PYTHONPATH``.


After this, go to the Python console and try

.. code-block:: py

	import halide
	import libgradient_autoscheduler

If the imports are successful, you are ready!
