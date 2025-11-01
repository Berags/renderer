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
            _strategy = std::make_unique<SequentialRenderer>();
            break;
        case PARALLEL:
            _strategy = std::make_unique<ParallelRenderer>();
            break;
        default:
            std::cout << "Renderer: Unknown strategy type.\n";
            _strategy = nullptr;
            break;
    }
}

void Renderer::set_filename(const std::string &filename) {
    this->_output_filename = filename;
}

void Renderer::save() const {
    std::cout << "Renderer: Saving rendered output to " << this->_output_filename << "\n";
}
