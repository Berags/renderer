//
// Created by jacopo on 10/28/25.
//

#include "renderer/Renderer.h"

#include "renderer/OptimizedParallelRenderer.h"
#include "renderer/SequentialRenderer.h"
#include "renderer/SimpleParallelRenderer.h"
#include "renderer/SpatialGridParallelRenderer.h"

void Renderer::Renderer::set_strategy(const RenderStrategy strategy) {
  switch (strategy) {
    case kSequential:
      strategy_ = std::make_unique<SequentialRenderer>();
      break;
    case kSimpleParallel:
      strategy_ = std::make_unique<SimpleParallelRenderer>();
      break;
    case kOptimizedParallel:
      strategy_ = std::make_unique<OptimizedParallelRenderer>();
      break;
    case kSpatialGridParallel:
      strategy_ = std::make_unique<SpatialGridParallelRenderer>();
      break;
    default:
      LOG(ERROR) << "Renderer: Unknown strategy type.";
      strategy_ = nullptr;
      break;
  }
}
