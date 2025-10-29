#include <iostream>

#include "include/Renderer.h"
#include "include/shape/Circle.h"

int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);
    renderer.render();
    renderer.set_strategy(PARALLEL);
    renderer.render();
    auto circle = Shape::Circle::Builder()
            .x(1)
            .y(2)
            .z(3)
            .radius(5)
            .colour({255, 0, 0, 0.5f})
            .build();
    return 0;
}
