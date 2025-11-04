//
// Created by jacopo on 10/28/25.
//

#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "renderer/SequentialRenderer.h"
#include "renderer/SimpleParallelRenderer.h"
#include "renderer/OptimizedParallelRenderer.h"

void Renderer::Renderer::setStrategy(const RenderStrategy strategy) {
    switch (strategy) {
        case SEQUENTIAL:
            _strategy = std::make_unique<SequentialRenderer>();
            break;
        case SIMPLE_PARALLEL:
            _strategy = std::make_unique<SimpleParallelRenderer>();
            break;
        case OPTIMIZED_PARALLEL:
            _strategy = std::make_unique<OptimizedParallelRenderer>();
            break;
        default:
            std::cout << "Renderer: Unknown strategy type.\n";
            _strategy = nullptr;
            break;
    }
}
