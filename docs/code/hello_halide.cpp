// g++ hello_halide.cpp -g -I ~/Programs/Halide/distrib/include -I ~/Programs/Halide/distrib/tools -L ~/Programs/Halide/distrib/bin -lHalide `libpng-config --cflags --ldflags` -ljpeg -o hello_halide -std=c++11

#include <Halide.h>
#include <halide_image_io.h>

using namespace Halide;
using namespace Halide::Tools;

int main() {
	// Constructing Halide functions statically.
    
    ImageParam input(Float(32), 3);
    Func f("f");
    Var x("x"), y("y"), c("c");
    f(x, y, c) = min(2 * input(x, y, c), 1.f);

    
    // Actually compiling/executing the Halide functions.
    
    // Setup the input by loading an image.
    Buffer<float> input_buffer = load_and_convert_image("images/rgb.png");
    input.set(input_buffer);
    // Process the input by calling f.realize
    Buffer<float> output_buffer = f.realize(640, 480, 3);
    // Save the image to a file.
    convert_and_save_image(output_buffer, "output.png");
}
