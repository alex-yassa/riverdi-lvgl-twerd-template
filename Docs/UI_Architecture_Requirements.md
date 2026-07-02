# System Architecture & Requirements Specification
## Dual-Core Resource Allocation for Riverdi 7.0" STM32H7 (REV1.2)

### 1. Architectural Overview
To guarantee maximum deterministic performance and computational throughput for critical calculations and communication protocols, the system architecture decouples the application layer across the dual-core topology of the STM32H757 microcontroller.
* **Cortex-M7 Core (480 MHz):** Dedicated entirely to high-priority mathematical modeling, algorithmic processing, and real-time communication stacks. It is completely isolated from user interface execution overhead.
* **Cortex-M4 Core (240 MHz):** Serves as the dedicated Graphic User Interface (GUI) engine, driving the LVGL framework and managing peripheral human-machine interactions.

---

### 2. Core Responsibilities & Domain Allocation

```
+-------------------------------------------------------------------+
|                           STM32H757                               |
|                                                                   |
|   +--------------------------+     +--------------------------+   |
|   |    Cortex-M7 Domain      |     |     Cortex-M4 Domain     |   |
|   |         (D1)             |     |         (D2)             |   |
|   +--------------------------+     +--------------------------+   |
|   | - High-Priority Math     |     | - LVGL Framework Engine  |   |
|   | - Comm Stacks / Protocols|     | - Direct DMA2D Triggers  |   |
|   | - Initial LTDC Power-On  |     | - None (Non-Touch HMI)   |   |
|   | - Runs from ITCM/DTCM RAM|     | - UI Ticks & Timers      |   |
|   +--------------------------+     +--------------------------+   |
|                 |                               |                 |
|                 +---------------+---------------+                 |
|                                 |                                 |
|                                 v                                 |
|                    +--------------------------+                   |
|                    |     Shared RAM Block     |                   |
|                    |  (IPC / Data Exchange)   |                   |
|                    +--------------------------+                   |
+-------------------------------------------------------------------+
```

#### A. Cortex-M7 (Data & Compute Engine)
* **Algorithmic Execution:** Runs core calculation logic entirely inside high-speed Tightly-Coupled Memory (`ITCM` and `DTCM` RAM) to prevent bus contention with the graphics pipeline.
* **Communication Pipelines:** Manages high-speed interfaces and protocol processing.
* **Display Initialization:** Handles initial system clock setup (RCC), external SDRAM configurations, and LTDC (LCD Controller) activation during the early boot sequence. Once initialized, the M7 drops all interaction with the visual display loop.

#### B. Cortex-M4 (Graphics & Peripheral Master)
* **LVGL Stack Hosting:** Compiles and runs the entire LVGL framework, widget layouts, and application state machines.
* **Hardware Graphic Acceleration:** Directs the **DMA2D (Chrom-ART)** and JPEG hardware accelerators directly from the D2 bus domain. The M4 programs the DMA2D registers for color-filling, image blending, and asset rendering, offloading pixel shifts from its own CPU execution.
* **HMI Input Processing:** Runs the UI tick timing tracking (`lv_tick_inc`) (touch panel input is disabled on this model).

---

### 3. Inter-Processor Communication (IPC) & Memory Mapping
To eliminate M7 CPU cycles being consumed by graphical workflows, the data handshake occurs via a strict transactional memory model:
* **Shared Data Buffer:** A dedicated segment of internal SRAM (`SRAM3` or `SRAM4`) is used as a shared registry. The M7 writes raw computation outputs into this registry and ceases interaction.
* **Synchronization:** Hardware Semaphores (`HSEM`) or light cross-core interrupts manage race conditions during structural memory reads/writes.
* **UI Data Binding:** The M4 polls or handles notifications from the shared registry, reading raw variables and binding them to the visual LVGL widgets during its routine `lv_task_handler()` cycle.
* **Display Buffers:** Frame buffers map directly into regions accessible to both the M4 rendering engine (via DMA2D) and the passive LTDC hardware controller loop.

---

### 4. Step-by-Step Porting Guide from Working M7 Demo
To migrate your working `lv_port_riverdi_70-stm32h7` demo from the M7 to the M4:

1. **Project Tree Reorganization:** Move the LVGL source tree, `lv_conf.h`, and your generated UI asset files from the `_CM7` subproject into the `_CM4` subproject structure within STM32CubeIDE.
2. **Linker Script Adjustments:** Modify the CM4 linker script (`STM32H757XIHx_FLASH_CM4.ld`) to target internal `RAM_D2` (such as `SRAM1` and `SRAM2`) for the LVGL memory heap allocations to prevent cache coherency conflicts with the M7 (`RAM_D1`).
3. **Peripheral Migration:** Shift the hardware timer instance tracking systemic system ticks into the M4 core initialization steps (the I2C touchscreen driver is not used in this non-touch display configuration).
4. **Boot Sequence Handling:** Ensure the M7 boots first, starts up the system clocks and external memories, turns on the backlight, and executes `HAL_RCC_BOOT_CM4_RELEASE()` to spin up the M4 core.
