#include <ctime>
#include <iostream>

#include "include/Image.h"
#include "include/renderer/Renderer.h"
#include "include/shape/Circle.h"
#include "utils/Utils.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SIMPLE_PARALLEL);

    Image image(1024, 1024, "output.png");
    std::vector<std::unique_ptr<Shape::IShape> > shapes;

    std::cout << "Creating shapes...\n";
    Utils::createShapes(shapes, image, 5000);

    std::cout << "Rendering image...\n";
    renderer.render(image, shapes);

    if (image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }

    return 0;
}
