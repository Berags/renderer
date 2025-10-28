#include <iostream>

#include "include/Renderer.h"

// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.
int main() {
    auto renderer = Renderer();
    renderer.set_strategy(SEQUENTIAL);
    renderer.render();
    renderer.set_strategy(PARALLEL);
    renderer.render();
    return 0;
}