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
   ./build/main
   ```

3. Analyze the profiling results:
   ```bash
   gprof build/main gmon.out > profile_analysis.txt
   ```

4. View the analysis:
   ```bash
   less profile_analysis.txt
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
