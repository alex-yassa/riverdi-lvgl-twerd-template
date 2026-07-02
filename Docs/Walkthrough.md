# Walkthrough: Dual-Core GUI Architecture Refactor & Docker Compiler Setup

We have successfully migrated the GUI architecture to Cortex-M4 and configured a containerized compilation environment using Docker Compose. Both the Cortex-M4 and Cortex-M7 binaries build with 100% success using the exact compiler toolchain.

---

## 🏗️ 1. Architecture Alignment & Resource Mapping

The responsibilities of the dual cores are decoupled as follows:

| Component | Core Responsibility | Allocation Details |
|---|---|---|
| **Cortex-M7** | System Master & Boot Loader | Configures Clocks, FMC (SDRAM), LTDC, DSI, then wakes up CM4. |
| **Cortex-M4** | Graphic & HMI Host | Runs the LVGL engine, I2C1 Touch driver, and triggers DMA2D blitting. |
| **SRAM (Internal)** | LVGL Buffers | Dual 30KB draw buffers (`buf_1`, `buf_2`) allocated in CM4's internal RAM. |
| **SDRAM (External)** | Framebuffer | Configured at `0xD0000000` (1024x600, ARGB8888). |

---

## 🛠️ 2. Docker compilation Setup

We have integrated a Docker compiler environment that mirrors the official STM32CubeIDE build settings without requiring the IDE to run:

1. **Root `Makefile`**: Delegates build targets (`all`, `cm4`, `cm7`, `clean`) to the IDE-generated Release Makefiles.
2. **`Dockerfile`**: Provides the build utilities (e.g. `make`).
3. **`docker-compose.yml`**: Mounts the project folder to the exact same host absolute path inside the container (resolving hardcoded compiler path definitions) and passes the host's ST GCC toolchain (`GNU Tools for STM32 14.3.rel1`) directly into the container.

### Running compilation via Docker:
To compile both cores cleanly inside the container:
```bash
docker compose run --rm builder
```

---

## 🚀 3. Verification & Flash Guide

### Flashing CM7 (Flash Bank 1):
```bash
~/.local/share/stm32cube/bundles/programmer/2.22.0+st.1/bin/STM32_Programmer_CLI \
  -c port=SWD freq=4000 ap=0 reset=HWrst \
  -d /home/alex/Documents/riverdi/LVGL_Demo_H7-H4_switch/lv_port_riverdi_70-stm32h7/STM32CubeIDE/CM7/Release/riverdi-70-stm32h7-lvgl_CM7.elf \
  -hardRst
```

### Flashing CM4 (Flash Bank 2):
```bash
~/.local/share/stm32cube/bundles/programmer/2.22.0+st.1/bin/STM32_Programmer_CLI \
  -c port=SWD freq=4000 ap=3 reset=HWrst \
  -d /home/alex/Documents/riverdi/LVGL_Demo_H7-H4_switch/lv_port_riverdi_70-stm32h7/STM32CubeIDE/CM4/Release/riverdi-70-stm32h7-lvgl_CM4.elf \
  -hardRst
```

### Full Hardware Reset:
```bash
~/.local/share/stm32cube/bundles/programmer/2.22.0+st.1/bin/STM32_Programmer_CLI \
  -c port=SWD reset=HWrst -hardRst
```
