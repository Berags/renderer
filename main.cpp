#include <iostream>

#include "include/Image.h"
#include "include/Renderer.h"
#include "include/shape/Circle.h"
#include "utils/Utils.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);

    Image image(1024, 1024, "output.png");
    std::vector<std::unique_ptr<Shape::IShape> > shapes;

    Utils::createShapes(shapes, image, 150);

    renderer.render(image, shapes);

    if (image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }

    return 0;
}
