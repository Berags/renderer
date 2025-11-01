//
// Created by jacopo on 10/28/25.
//

#ifndef RENDERER_RENDERER_H
#define RENDERER_RENDERER_H

#include <iostream>
#include <memory>

typedef enum RenderStrategy {
    SEQUENTIAL,
    PARALLEL
} RenderStrategy;

class Strategy {
public:
    virtual ~Strategy() = default;

    virtual void render() const = 0;
};


class Renderer {
public:
    Renderer() = default;

    ~Renderer() = default;

    void set_strategy(RenderStrategy strategy);

    void set_filename(const std::string &filename);

    void render() const {
        if (!_strategy) {
            throw std::runtime_error("Rendering strategy is not set.");
        }

        if (_output_filename.empty()) {
            throw std::runtime_error("Output filename is required.");
        }

        _strategy->render();
        save();
    }

private:
    std::unique_ptr<Strategy> _strategy;
    std::string _output_filename;

    void save() const;
};


#endif //RENDERER_RENDERER_H
