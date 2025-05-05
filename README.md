# Search Engine

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)
![License](https://img.shields.io/badge/License-MIT-orange.svg)

A C++ search engine implementation that processes text documents, builds an inverted index, and handles search queries to return relevant results.

## Project Structure

```bash
SearchEngine/
├── CMakeLists.txt
├── build/
├── resources/
│ └── file1.txt
│ └── file2.txt
│ └── file3.txt
│ └── file4.txt
│ └── file5.txt
├── include/
│ ├── ConverterJSON.h
│ ├── InvertedIndex.h
│ └── SearchServer.h
├── src/
│ ├── ConverterJSON.cpp
│ ├── InvertedIndex.cpp
│ ├── SearchServer.cpp
│ └── main.cpp
├── tests/
│ ├── test_ConverterJSON.cpp
│ ├── test_InvertedIndex.cpp
│ ├── test_SearchServer.cpp
│ └── other_tests.cpp
└── extern/
    ├── json/
    └── googletest/
```

## Features

- Document processing from text files
- Inverted index construction
- JSON configuration and request handling
- Multi-threaded search capabilities
- Relevance-ranked results
- Comprehensive unit tests

## Requirements

- C++17 compatible compiler
- CMake 3.10 or higher
- Git (for cloning the repository)

## Installation

1. Clone the repository:
```bash
git clone https://github.com/yourusername/SearchEngine.git
cd SearchEngine
```

2. Build the project:
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

3. Run the application:
```bash
./SearchEngine
```

4. Run tests:
```bash
./tests
```

## Configuration

Edit config.json to specify:

    Paths to document resources

    Search engine name and version

    Maximum responses per query

Example config.json:

```bash
{
  "config": {
    "name": "MySearchEngine",
    "version": "0.1",
    "max_responses": 5
  },
  "files": [
    "resources/file1.txt",
    "resources/file2.txt",
    "resources/file3.txt",
    "resources/file4.txt",
    "resources/file5.txt"
  ]
}
```

## Usage

1. Place your text documents in the resources/ directory

2. Add queries to requests.json:

```bash
{
  "requests": [
    "some word",
    "some",
    "word",
    "nonsense"
  ]
}
```

3. Run the application

4. Results will be saved in answers.json

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
