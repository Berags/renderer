#include <iostream>

#include "include/Image.h"
#include "include/Renderer.h"
#include "include/shape/Circle.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);
    renderer.render();
    if (const Image image(800, 600, "output.png"); image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }
    return 0;
}
