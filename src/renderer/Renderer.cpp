//
// Created by jacopo on 10/28/25.
//

#include "renderer/Renderer.h"

#include <iostream>
#include <ostream>

#include "renderer/OptimizedParallelRenderer.h"
#include "renderer/SequentialRenderer.h"
#include "renderer/SimpleParallelRenderer.h"
#include "renderer/SpatialGridParallelRenderer.h"

void Renderer::Renderer::set_strategy(const RenderStrategy strategy) {
  switch (strategy) {
    case SEQUENTIAL:
      strategy_ = std::make_unique<SequentialRenderer>();
      break;
    case SIMPLE_PARALLEL:
      strategy_ = std::make_unique<SimpleParallelRenderer>();
      break;
    case OPTIMIZED_PARALLEL:
      strategy_ = std::make_unique<OptimizedParallelRenderer>();
      break;
    case SPATIAL_GRID_PARALLEL:
      strategy_ = std::make_unique<SpatialGridParallelRenderer>();
      break;
    default:
      std::cout << "Renderer: Unknown strategy type.\n";
      strategy_ = nullptr;
      break;
  }
}
