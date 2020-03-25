import halide as hl
import imageio
import numpy as np

# Constructing Halide functions statically.
input = hl.ImageParam(hl.Float(32), 3)
f = hl.Func('f')
x, y, c = hl.Var('x'), hl.Var('y'), hl.Var('c')
# Double the values and clamp them by 1.
f[x, y, c] = hl.min(2 * input[x, y, c], 1.0)


# Actually compiling/executing the Halide functions.
#
# Setup the input by loading an image (Halide assumes Fortran ordering).
input_buffer = hl.Buffer(np.asfortranarray(imageio.imread('images/rgb.png').astype(np.float32) / 255.0))
input.set(input_buffer)
# Process the input by calling f.realize
output = f.realize(input_buffer.width(), input_buffer.height(), input_buffer.channels())
# Save the image to a file by converting to a numpy array.
output = np.array(output)
imageio.imsave('output.png', (output * 255.0).astype(np.uint8))
