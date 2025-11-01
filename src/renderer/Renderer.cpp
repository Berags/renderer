//
// Created by jacopo on 10/28/25.
//

#include "../../include/renderer/Renderer.h"

#include <iostream>
#include <mmintrin.h>
#include <ostream>
#include <xmmintrin.h>

#include "../../include/renderer/SequentialRenderer.h"
#include "../../include/renderer/SimpleParallelRenderer.h"

void Renderer::set_strategy(const RenderStrategy strategy) {
    switch (strategy) {
        case SEQUENTIAL:
            _strategy = std::make_unique<SequentialRenderer>();
            break;
        case SIMPLE_PARALLEL:
            _strategy = std::make_unique<SimpleParallelRenderer>();
            break;
        default:
            std::cout << "Renderer: Unknown strategy type.\n";
            _strategy = nullptr;
            break;
    }
}
