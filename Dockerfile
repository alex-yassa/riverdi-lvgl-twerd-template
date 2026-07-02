# Use a stable Ubuntu LTS base image
FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Update system package registry and install build tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    git \
    ninja-build \
    python3 \
    pkg-config \
    # SDL2 for PC simulator (native x86_64 target)
    libsdl2-dev \
    # GNU Arm Embedded Toolchain and C Library stubs for Cortex-M
    gcc-arm-none-eabi \
    libnewlib-arm-none-eabi \
    libstdc++-arm-none-eabi-newlib \
    # Cleanup apt caches to minimize image size
    && rm -rf /var/lib/apt/lists/*

# Set compilation workspace
WORKDIR /workspace

# Default command compiles the targets using the Makefile
CMD ["make"]
