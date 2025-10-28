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

class Strategy
{
public:
    virtual ~Strategy() = default;
    virtual void render() const = 0;
};


class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void set_strategy(RenderStrategy strategy);

    void render() const
    {
        if (strategy_) {
            strategy_->render();
        } else {
            std::cout << "Context: Strategy isn't set\n";
        }
    }

private:
    std::unique_ptr<Strategy> strategy_;
};


#endif //RENDERER_RENDERER_H