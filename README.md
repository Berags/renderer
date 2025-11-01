A renderer that draws (semi) transparent circles and rectangles. Shapes have 3D coordinates and the order along Z axis matters for the correct rendering. A pixel belongs to a circle if its center is within the circle.

Handled transparency/alpha blending 
- Sort by Z-axis for correct rendering 
- A pixel belongs to a shape if its center is within the shape

Output:
- Generate images at various resolutions (e.g. 512×512, 1024×1024, 2048×2048)
- Support hundreds to thousands of shapes

Parallelization Opportunities:
- Pixel-level parallelism 
- Shape-level parallelism with synchronization 
- Tile-based rendering