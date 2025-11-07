//
// Created by jacopo on 11/7/25.
//

#ifndef RENDERER_ISHAPEVISITOR_H
#define RENDERER_ISHAPEVISITOR_H

namespace Shape {
    class Circle;
    class Rectangle;

    /**
     * Interface for the Visitor pattern applied to shapes.
     * Allows operations to be performed on different shape types without modifying their classes.
     */
    class IShapeVisitor {
    public:
        virtual ~IShapeVisitor() = default;

        /**
         * Visit a Circle shape.
         * @param circle Reference to the Circle object being visited.
         */
        virtual void visit(const Circle &circle) = 0;

        /**
         * Visit a Rectangle shape.
         * @param rectangle Reference to the Rectangle object being visited.
         */
        virtual void visit(const Rectangle &rectangle) = 0;
    };
}

#endif //RENDERER_ISHAPEVISITOR_H
