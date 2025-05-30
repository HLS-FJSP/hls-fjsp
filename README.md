# An Effective Hybrid Local Search method for FJSP problem(HLS-FJSP)

HLS-FJSP is an effective hybrid local search method for FJSP problem. The algorithm adopts a two-phase optimization framework and  introduces a control parameter dynamically coordinates greedy heuristics, genetic operations, and tabu search, thereby effectively addressing critical issues including solution feasibility, diversity, and optimization efficiency through the collaborative optimization of initialization and local search phases.

## Get Started

The proposed HLS-FJSP and all baselines are implemented in `C++`.

### Build Environment

The project has been tested with the following toolchain:
- **C++ Compilers**:
  - GCC/G++ 11.4.0
  - Clang 19.0.0
- **Build System**: CMake 3.22.1
- **OS**: Ubuntu 22.04.3 LTS

To verify your environment:
```bash
# Check compiler versions
g++ --version
clang++ --version

# Check CMake version
cmake --version
```

### Build

```shell
mkdir build && cd build
cmake ..
make
```

The executable will be generated as `production_scheduling` or similar.

### Usage

```shell
./production_scheduling [input_file] [algorithm]
```

- `input_file`: Path to the dataset file (default: `../input.txt`)

- ```
  algorithm
  ```

  : (Optional) Specify the scheduling algorithm. Supported values:

  - FIFO_EET
  - FIFO_SPT
  - MOPNR_EET
  - MOPNR_SPT
  - MWKR_EET
  - MWKR_SPT

If no algorithm is specified, the program will use HLS-FJSP method to generate the solution. 

#### Example

```shell
./production_scheduling ../dataset/fjsp.brandimarte.Mk03.m8j15c5.txt FIFO_EET
```

This will run the FIFO_EET algorithm on the specified dataset.

#### Output

- The program prints the total scheduling time to the console.

### Parameters

You can adjust search parameters in `main.cpp`:

- `search_mode_param`: Probability to choose random search vs. greedy search.
- `random_search_strategy_param`: Probability to choose genetic algorithm vs. tabu search in random search.
- `max_iter_count`: Maximum iterations per optimization round.
- `population_size`: Population size for genetic algorithm.
- `tabu_list_length`: Tabu list length for tabu search.
- `max_repeat_count`: Maximum allowed repeats of the best solution before stopping.