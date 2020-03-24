import halide as hl
import imageio
import numpy as np

input = hl.ImageParam(hl.Float(32), 3)
f = hl.Func('f')
x, y, c = hl.Var('x'), hl.Var('y'), hl.Var('c')
f[x, y, c] = hl.min(2 * input[x, y, c], 1.0)

img = hl.Buffer(np.asfortranarray(imageio.imread('images/rgb.png').astype(np.float32) / 255.0))
input.set(img)
output = f.realize(img.width(), img.height(), img.channels())
output = np.array(output)
imageio.imsave('output.png', (output * 255.0).astype(np.uint8))
