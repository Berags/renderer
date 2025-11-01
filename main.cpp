#include <iostream>

#include "include/Image.h"
#include "include/Renderer.h"
#include "include/shape/Circle.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);
    renderer.render();
    auto circle = Shape::Circle::Builder()
            .x(1)
            .y(2)
            .z(3)
            .radius(5)
            .colour({255, 0, 0, 0.5f})
            .build();
    const Image image(800, 600, "output.png");
    if (image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }
    return 0;
}
