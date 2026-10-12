==============================================================================
                               ImageNightmare
       Modern Lightweight Desktop Image Editor Application (C++17 / Qt)
==============================================================================

PROJECT NAME : ImageNightmare
COURSE: CS213 Object Oriented Programming
PROFESSOR: Mohammed El-Ramly
PLATFORMS    : Windows / Linux / macOS
CORE TECH    : C++17, Qt 6.x (Qt Widgets), CMake 3.16+
THIRD PARTY  : stb_image.h & stb_image_write.h (Header-only image I/O)
REPOSITORY   : https://github.com/Moataz35/ImageNightmare.git
DOCUMENTATION: https://docs.google.com/document/d/1F4fwBcmW47qdQ9xvXMfu8xSqQZQj8xk13apHzpT5D08/edit?tab=t.0#heading=h.fn5z8ywyuhpj


==============================================================================
1. TEAM MEMBERS & CONTRIBUTORS
==============================================================================
Developed by students from the Faculty of Computers and Artificial
Intelligence (FCAI), Cairo University:

-------------------------------------------------------------------------------------------------------------
|        Name        |     ID     |  Section  |                           Filters                           |
-------------------------------------------------------------------------------------------------------------
| Moustafa Mahmoud   | 20250646   | S9,10     | Grayscale, Flip, Merge, Sunlight                            |
| Moataz Mohamed     | 20250908   | S9,10     | Black&White, Rotate, Edge Detection, Old TV, Skew           |
| Abdullah Ibrahim   | 20250928   | S9,10     | Invert, Darken&Lighten, Crop, Purple Effect, Oil Painting   |
| Mohamed Sherif     | 20251334   | S9,10     | Add Frame, Blur, Resize, Infrared                           |
-------------------------------------------------------------------------------------------------------------

Academic Attribution:
- Dr. Mohamed El-Ramly and the FCAI teaching team for foundational Image
  class architecture.


==============================================================================
2. PROJECT OVERVIEW
==============================================================================
ImageNightmare is a full-featured, responsive desktop image editing and
processing application. Developed in modern C++ (C++17) with the Qt Widgets
framework, it pairs an intuitive, clean graphical user interface with an
extensive collection of algorithmic pixel manipulation filters.

All image processing algorithms are executed asynchronously using Qt
threads (QThread) to ensure the user interface remains smooth, responsive,
and completely unblocked even when operating on large high-resolution images.


==============================================================================
3. KEY HIGHLIGHTS & ARCHITECTURE
==============================================================================
- ASYNCHRONOUS PROCESSING:
  Computationally intensive filter algorithms execute off the main GUI thread
  via QThread, featuring a live spinner and status notifications in the status
  bar.

- MULTI-LEVEL UNDO / REDO / RESET:
  Full non-destructive state management built on a dual-stack model (std::deque
  and std::stack). Freely step backwards and forwards through edits, or reset
  instantly back to the original loaded file.

- LIVE NON-DESTRUCTIVE PREVIEWS:
  Interactive sliders for Brightness, Contrast, Saturation, and Skew compute
  real-time previews from the base image state without mutating history until
  explicitly committed with the "Apply" button.

- DRAG-AND-DROP CANVAS:
  Seamlessly load any supported image file by dragging and dropping it
  directly onto the central editor canvas (EditorDropArea) or using the File menu.

- INTERACTIVE ON-CANVAS CROP:
  Adjustable, draggable bounding box overlay (CropBox) with coordinate clipping
  and boundary validation to preview framing before finalizing crops.

- VECTOR SVG GRAPHICS & MODERN DARK UI:
  Fully styled dark theme with scalable SVG iconography, scrollable tool
  sidebar panels, and a real-time status bar showing image dimensions.

- UNSAVED WORK CONFIRMATION:
  Safeguards against accidental data loss with prompt dialogues on image close
  or program exit.


==============================================================================
4. COMPLETE FILTER & TOOL CATALOG
==============================================================================

[A] COLOR & TONALITY ADJUSTMENTS
--------------------------------
1. Brightness Adjustment:
   Linearly adjusts pixel values across a range of -100 to +100 with live preview.
2. Contrast Adjustment:
   Non-linear factor curve centered at midpoint 128:
   Factor = (259 * (C + 255)) / (255 * (259 - C))
3. Saturation Adjustment:
   Modulates color vibrance by displacing channels relative to average grayscale
   luminance.

[B] ARTISTIC & STYLISTIC FILTERS
--------------------------------
4. Grayscale:
   Converts RGB channels to uniform luminance: Gray = (R + G + B) / 3.
5. Black and White:
   High-contrast binarization thresholding pixels at intensity 128 (0 or 255).
6. Invert Colors (Negative):
   Inverts each color channel: New_Pixel = 255 - Old_Pixel.
7. Darken / Lighten:
   Quick tonal shifts applied across the canvas.
8. Box Blur:
   Multi-pass 5x5 spatial neighborhood averaging filter for smooth blurring.
9. Sobel Edge Detection:
   Calculates horizontal (Gx) and vertical (Gy) convolution gradients to map
   outlines and contours based on a configurable threshold.
10. Sunlight Effect:
    Warm golden-hour ambient filter boosting red (+20%) and green (+10%) while
    dampening blue (-20%).
11. Infrared Filter:
    Simulates thermal photography by maxing out red channels (255) and inverting
    blue and green channels.
12. Purple / Cyberpunk Effect:
    Stylistic hue shift emphasizing magenta and cyan tones.
13. Old TV Simulation:
    CRT scanline effect generating horizontal interlaced scanlines every 6th row.
14. Oil Painting Effect:
    Texture synthesis calculating local intensity histograms and painting the
    dominant intensity mode.

[C] SPATIAL & GEOMETRIC TRANSFORMATIONS
---------------------------------------
15. Rotate:
    Lossless 90, 180, and 270 degree clockwise rotations.
16. Flip:
    Mirror reflection across the horizontal or vertical axes.
17. Resize:
    Arbitrary dimension scaling using nearest-neighbor mapping.
18. Interactive Crop:
    Visual draggable selection box on canvas with boundary validation.
19. Skew / Shear:
    Horizontal shear transformation (-45 to +45 degrees) with real-time preview:
    x' = x + y * tan(theta) + offset_x.

[D] FRAMES & BORDERS
--------------------
20. Simple Frame:
    Solid border of customizable pixel width and user-chosen color.
21. Fancy Frame:
    Multi-lined decorative framing with contrasting concentric border bands.
    Includes an integrated QColorDialog color picker.

[E] MULTI-IMAGE MERGE & BLENDING
--------------------------------
22. Merge by Scaling:
    Scales a secondary overlay image to match base dimensions with an alpha
    blending slider (0% to 100%).
23. Merge by Common Area:
    Blends an overlay image onto the canvas at a custom (X, Y) coordinate offset.


==============================================================================
5. SYSTEM REQUIREMENTS & PREREQUISITES
==============================================================================
- Compiler: C++17 compliant (MinGW GCC 11+, Clang 13+, or MSVC 2019+)
- Framework: Qt 6.x (Widgets module required)
- Build System: CMake 3.16+
- Supported OS: Windows 10/11, Ubuntu 20.04+, macOS 11+


==============================================================================
6. HOW TO BUILD AND RUN
==============================================================================

METHOD 1: USING QT CREATOR (RECOMMENDED)
----------------------------------------
1. Launch Qt Creator.
2. Go to: File -> Open File or Project...
3. Select "D:/projects/ImageNightmare/GUI/CMakeLists.txt".
4. Choose your configured Kit (e.g., Desktop Qt 6.x MinGW 64-bit).
5. Click "Configure Project".
6. Press Run Button to build and start ImageNightmare.

METHOD 2: USING CMAKE COMMAND LINE
----------------------------------
1. Open terminal / PowerShell:
   cd D:\projects\ImageNightmare\GUI\

2. Create and enter build directory:
   mkdir build
   cd build

3. Configure:
   cmake ../GUI -DCMAKE_BUILD_TYPE=Release

   (Note: If Qt is not in system PATH, pass CMAKE_PREFIX_PATH):
   cmake ../GUI -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64"

4. Compile:
   cmake --build . --config Release

5. Run:
   .\GUI.exe   (On Windows)
   ./GUI       (On Linux/macOS)


==============================================================================
7. USER INTERFACE & WORKFLOW INSTRUCTIONS
==============================================================================
1. OPEN AN IMAGE:
   - Click "File" -> "Open" in the top navigation bar, or
   - Drag and drop an image directly into the center area.

2. APPLYING TOOLS:
   - Select a tool category from the left sidebar (Filters, Adjust, Rotate,
     Flip, Frame, Resize, Crop, Skew, Merge).
   - For slider tools (Adjust, Skew, Merge):
     Adjust the slider for real-time live preview. Click "Apply" to confirm,
     or "Cancel" to discard changes.

3. CROPPING:
   - Click "Crop" on the sidebar.
   - Drag or resize the crop rectangle directly on top of the image.
   - Click "Apply" to perform the crop.

4. UNDO, REDO & RESET:
   - Use "Edit" -> "Undo" (Ctrl+Z) or "Edit" -> "Redo" (Ctrl+Y).
   - Click "Edit" -> "Reset" to return immediately to the original image.

5. SAVING:
   - Click "File" -> "Save" to overwrite the current file.
   - Click "File" -> "Save As" to export a copy with a new name and format.


==============================================================================
8. SUPPORTED FILE FORMATS
==============================================================================
ImageNightmare natively reads and writes:
- Portable Network Graphics (.png)
- JPG and JPEG (.jpg, .jpeg)
- Windows Bitmap (.bmp)
- Truevision TGA (.tga)

All loaded files are normalized to standard 24-bit RGB (3 channels).


==============================================================================
9. PROJECT DIRECTORY STRUCTURE
==============================================================================
ImageNightmare/
|-- GUI/                         # Qt graphical user interface
|   |-- icons/                   # Vector SVG icons
|   |-- CMakeLists.txt           # Build configuration
|   |-- CropBox.h                # Draggable on-canvas crop overlay
|   |-- EditorDropArea.h/.cpp    # Central canvas and drag-and-drop
|   |-- MainWindow.h/.cpp/.ui    # Main application window & logic
|   |-- navbar.h/.cpp            # Navigation bar (File / Edit menus)
|   |-- resources.qrc            # Qt resource file
|   |-- SideBar.h/.cpp/.ui       # Tool palette and slider controls
|   `-- StatusMenu.h/.cpp        # Bottom status bar & loading spinner
|-- include/                     # Core engine headers
|   |-- Image.h                  # Image buffer and I/O wrapper
|   |-- ImageEditor.h            # Session manager & history stack
|   `-- ImageFilter.h            # Filter prototypes and color structs
|-- src/                         # Core algorithmic implementations
|   |-- ImageEditor.cpp          # History and editor logic
|   `-- ImageFilter.cpp          # Pixel manipulation math and algorithms
|-- lib/include/                 # Third-party libraries
|   |-- stb_image.h              # Single-header image reader
|   `-- stb_image_write.h        # Single-header image writer
|-- README.md                    # Rich Markdown documentation
`-- README.txt                   # Plain text project documentation
==============================================================================
