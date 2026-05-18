#!/usr/bin/env bash
# NanoMark Compilation Tool
set -e

GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

show_help() {
    echo -e "${BLUE}NanoMark Compilation Guide:${NC}"
    echo -e "Usage: ./build.sh [options]"
    echo -e ""
    echo -e "Options:"
    echo -e "  -h, --help      Show this help menu"
    echo -e "  -c, --clean     Wipe out build directory and build everything from scratch"
    echo -e "  -d, --debug     Compile in DEBUG mode (CMAKE_BUILD_TYPE=Debug)"
    echo -e "  -r, --release   Compile in RELEASE mode (CMAKE_BUILD_TYPE=Release) [Default]"
}

CLEAN_MODE=false
BUILD_TYPE="Release"

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -h|--help) show_help; exit 0 ;;
        -c|--clean) CLEAN_MODE=true; shift ;;
        -d|--debug) BUILD_TYPE="Debug"; shift ;;
        -r|--release) BUILD_TYPE="Release"; shift ;;
        *) echo -e "${RED}Unknown option: $1${NC}"; show_help; exit 1 ;;
    esac
done

echo -e "${BLUE}===================================================${NC}"
echo -e "${BLUE}     NanoMark C++ Application Builder Tool          ${NC}"
echo -e "${BLUE}===================================================${NC}"

if [ "$CLEAN_MODE" = true ]; then
    echo -e "${YELLOW}Clean Mode Active: Deleting build/ folder...${NC}"
    rm -rf build
fi

# Ensure build folder exists
if [ ! -d "build" ]; then
    echo -e "${YELLOW}Creating build/ directory...${NC}"
    mkdir -p build
fi

cd build

# Run CMake
echo -e "${YELLOW}Configuring CMake project in ${BUILD_TYPE} mode...${NC}"
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

# Run Make
echo -e "${YELLOW}Compiling C++ sources...${NC}"
make -j$(nproc)

echo -e "${BLUE}===================================================${NC}"
echo -e "${GREEN}   Compilation Successful! Binary ready inside build/NanoMark.${NC}"
echo -e "${BLUE}===================================================${NC}"
