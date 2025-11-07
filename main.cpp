#include <ctime>
#include <iostream>

#include "include/Image.h"
#include "include/renderer/Renderer.h"
#include "include/shape/Circle.h"
#include <omp.h>
#include "utils/Utils.h"

int main() {
    auto renderer = Renderer::Renderer();
    renderer.setStrategy(Renderer::RenderStrategy::SPATIAL_GRID_PARALLEL);

    Image image(2048, 2048, "output.png");
    std::vector<std::unique_ptr<Shape::IShape> > shapes;

    std::cout << "Creating shapes...\n";
    Utils::createShapes(shapes, image, 15000);

    std::cout << "Rendering image...\n";
    const double startTime = omp_get_wtime();
    renderer.render(image, shapes);
    const double endTime = omp_get_wtime();

    const double renderTimeMs = (endTime - startTime) * 1000.0;
    std::cout << "Rendering completed in: " << renderTimeMs << " ms.\n";

    if (image.save()) {
        std::cout << "Image saved successfully.\n";
    } else {
        std::cout << "Failed to save image.\n";
    }

    return 0;
}
