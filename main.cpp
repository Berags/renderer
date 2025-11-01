#include <iostream>

#include "include/Image.h"
#include "include/Renderer.h"
#include "include/shape/Circle.h"
#include "utils/Utils.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);

    Image image(2048, 2048, "output.png");
    std::vector<std::unique_ptr<Shape::IShape> > shapes;

    std::cout << "Creating shapes...\n";
    Utils::createShapes(shapes, image, 1000);

    std::cout << "Rendering image...\n";
    renderer.render(image, shapes);

    if (image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }

    return 0;
}
