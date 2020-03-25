// To compile & run on Linux
// g++ hello_halide.cpp -g -I /path/to/halide/distrib/include -I /path/to/halide/distrib/tools -L /path/to/halide/distrib/bin -lHalide `libpng-config --cflags --ldflags` -ljpeg -lpthread -ldl -o hello_halide -std=c++11
// LD_LIBRARY_PATH=/path/to/halide/distrib/bin ./hello_halide
// MacOS
// g++ hello_halide.cpp -g -I /path/to/halide/distrib/include -I /path/to/halide/distrib/tools -L /path/to/halide/distrib/bin -lHalide `libpng-config --cflags --ldflags` -ljpeg -o hello_halide -std=c++11
// DYLD_LIBRARY_PATH=/path/to/halide/distrib/bin ./hello_halide

#include <Halide.h>
#include <halide_image_io.h>

using namespace Halide;
using namespace Halide::Tools;

int main() {
    // Constructing Halide functions statically.
    ImageParam input(Float(32), 3);
    Func f("f");
    Var x("x"), y("y"), c("c");
    // Double the values and clamp them by 1.
    f(x, y, c) = min(2 * input(x, y, c), 1.f);
    
    // Actually compiling/executing the Halide functions.
    //
    // Setup the input by loading an image.
    Buffer<float> input_buffer = load_and_convert_image("images/rgb.png");
    input.set(input_buffer);
    // Process the input by calling f.realize
    Buffer<float> output_buffer = f.realize(
        input_buffer.width(), input_buffer.height(), input_buffer.channels());
    // Save the image to a file.
    convert_and_save_image(output_buffer, "output.png");
}
