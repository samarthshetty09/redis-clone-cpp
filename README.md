# Redis Clone in C++

A simple Redis clone implemented in C++ for educational purposes. This project demonstrates the basic functionality of Redis, including in-memory data storage, basic key-value operations, and persistence.

## Features

- In-memory data store
- Basic Redis commands: `SET`, `GET`, `DEL`, `EXISTS`
- Data persistence to disk
- Simple command-line interface

## Getting Started

### Prerequisites

- C++ compiler (supporting C++11 or later)
- CMake (version 3.10 or later)

### Building the Project

1. Clone the repository:

    ```sh
    git clone https://github.com/samarthshetty09/redis-clone-cpp.git
    cd redis-clone
    ```

2. Create a build directory and navigate to it:

    ```sh
    mkdir build
    cd build
    ```

3. Configure the project with CMake:

    ```sh
    cmake ..
    ```

4. Build the project:

    ```sh
    make
    ```

### Running the Redis Clone

After building the project, you can run the Redis clone server:

```sh
./redis-clone
