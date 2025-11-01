#include <iostream>

#include "include/Image.h"
#include "include/Renderer.h"
#include "include/shape/Circle.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);

    Image image(800, 600, "output.png");
    std::vector<std::unique_ptr<Shape::IShape> > shapes;

    shapes.push_back(Shape::Circle::Builder()
        .x(400)
        .y(300)
        .z(3)
        .radius(100)
        .colour({0, 0, 255, .2f})
        .build());
    shapes.push_back(Shape::Circle::Builder()
        .x(450)
        .y(320)
        .z(2)
        .radius(80)
        .colour({255, 0, 0, 0.5f})
        .build());

    shapes.push_back(Shape::Circle::Builder()
        .x(350)
        .y(320)
        .z(1)
        .radius(200)
        .colour({255, 90, 0, 0.5f})
        .build());

    renderer.render(image, shapes);

    if (image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }

    return 0;
}
