// g++ hello_halide.cpp -g -I ~/Programs/Halide/distrib/include -I ~/Programs/Halide/distrib/tools -L ~/Programs/Halide/distrib/bin -lHalide `libpng-config --cflags --ldflags` -ljpeg -o hello_halide -std=c++11

#include <Halide.h>
#include <halide_image_io.h>

using namespace Halide;
using namespace Halide::Tools;

int main() {
    ImageParam input(Float(32), 3);
    Func f("f");
    Var x("x"), y("y"), c("c");
    f(x, y, c) = min(2 * input(x, y, c), 1.f);

    // Setup the input by loading an image
    Buffer<float> input_buffer = load_and_convert_image("images/rgb.png");
    input.set(input_buffer);
    Buffer<float> output_buffer = f.realize(640, 480, 3);
    convert_and_save_image(output_buffer, "output.png");
}
