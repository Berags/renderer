//
// Created by jacopo on 11/7/25.
//

#ifndef RENDERER_EXPERIMENTS_H
#define RENDERER_EXPERIMENTS_H
#include <assert.h>
#include <fstream>
#include <iostream>
#include <map>
#include <omp.h>

#include "renderer/Renderer.h"
#include "utils/Utils.h"

namespace Experiments {
    inline void runBenchmark(const Renderer::Renderer &renderer,
                             const int minShapes,
                             const int maxShapes,
                             const int step,
                             const int imageWidth,
                             const int imageHeight,
                             const std::string &outputFilename) {
        std::ofstream outputFile(outputFilename);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Could not open output file " << outputFilename << std::endl;
            return;
        }

        // Write CSV header
        outputFile << "Shapes,RenderTimeMs\n";
        std::cout << "Starting benchmark. Results will be saved to " << outputFilename << std::endl;

        Image image(imageWidth, imageHeight, "unused.png"); // Create a single image instance for all tests
        std::map<int, std::vector<std::unique_ptr<Shape::IShape> > > shapes;

        for (int numberOfShapes = minShapes; numberOfShapes <= maxShapes; numberOfShapes += step) {
            // check if this number of shapes was already generated
            if (!shapes.contains(numberOfShapes)) {
                shapes[numberOfShapes] = std::vector<std::unique_ptr<Shape::IShape> >();
                Utils::createShapes(shapes[numberOfShapes], image, numberOfShapes);
            }

            assert(numberOfShapes == shapes[numberOfShapes].size());
            // Start the rendering process measuring the time
            const double startTime = omp_get_wtime();
            renderer.render(image, (shapes[numberOfShapes]));
            const double endTime = omp_get_wtime();

            const double renderTimeMs = (endTime - startTime) * 1000.0;

            std::cout << "Test with " << numberOfShapes << " shapes completed in: " << renderTimeMs << " ms.\n";
            outputFile << numberOfShapes << "," << renderTimeMs << "\n";
        }

        outputFile.close();
        std::cout << "Benchmark finished.\n";
    }
}

#endif //RENDERER_EXPERIMENTS_H
