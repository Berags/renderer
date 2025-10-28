//
// Created by jacopo on 10/28/25.
//

#include "../include/Renderer.h"

#include <iostream>
#include <ostream>

#include "../include/SequentialRenderer.h"
#include "../include/ParallelRenderer.h"

void Renderer::set_strategy(const RenderStrategy strategy) {
    switch (strategy) {
        case SEQUENTIAL:
            strategy_ = std::make_unique<SequentialRenderer>();
            break;
        case PARALLEL:
            strategy_ =  std::make_unique<ParallelRenderer>();
            break;
        default:
            std::cout << "Renderer: Unknown strategy type.\n";
            strategy_ = nullptr;
            break;
    }
}