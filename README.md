# Optimizer

A high-performance optimization tool.

## Building

### Build Modes

The project supports different build modes controlled by the `DEBUG` flag:

#### Release Mode (Default)
```bash
make
# or explicitly
make DEBUG=0
```
- Optimized for performance (`-O2`)
- Debug symbols removed (`-s`)
- Assertions disabled (`-DNDEBUG`)

#### Debug Mode with Symbols
```bash
make DEBUG=1
```
- Includes debug symbols (`-g`)
- Enables debug output (`-DDEBUG`)
- No optimization for better debugging

#### Profiling Mode
```bash
make DEBUG=2
```
- Includes debug symbols (`-g`)
- Enables profiling (`-pg`)
- Adds profiling flags to both compiler and linker

## Profiling with gprof

After building with `DEBUG=2`, you can profile the application:

1. Build with profiling enabled:
   ```bash
   make clean
   make DEBUG=2
   ```

2. Run your program to generate profiling data (`gmon.out`):
   ```bash
   ./build/optimizer
   ```

3. Analyze the profiling results:
   ```bash
   gprof build/optimizer gmon.out > profile_analysis.txt
   ```
   
4. View the analysis:
   ```bash
   less profile_analysis.txt
   ```

## Visualizing Profiling Results with gprof2dot

For a graphical representation of the profiling data, you can use `gprof2dot` with Graphviz:

1. Install the required system dependencies:
   ```bash
   sudo apt-get install python3 graphviz
   ```

2. Create and activate a Python virtual environment:
   ```bash
   mkdir -p gprof_visualizer
   cd gprof_visualizer
   python3 -m venv venv
   source venv/bin/activate
   ```

3. Install gprof2dot in the virtual environment:
   ```bash
   pip install gprof2dot
   ```

4. Generate a visual call graph (from your project root):
   ```bash
   gprof ./build/optimizer | gprof2dot | dot -Tpng -o callgraph.png
   ```

5. View the generated image with your preferred image viewer:
   ```bash
   xdg-open callgraph.png  # On Linux
   open callgraph.png      # On macOS
   ```

   The call graph will show:
   - Function execution time percentages
   - Call hierarchy
   - Time spent in each function

   To deactivate the virtual environment when done:
   ```bash
   deactivate
   ```

## Running Tests

To run the test suite:
```bash
make test
```

## Cleaning

To clean build artifacts:
```bash
make clean
```
