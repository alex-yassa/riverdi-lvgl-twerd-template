# Build & Flash via CLI — STM32H757 (CM4 / CM7)

This guide covers building and flashing the **Riverdi STM32H7 7"** project from the command line
using Docker (for the compiler toolchain) and `STM32_Programmer_CLI` (for flashing over SWD).

---

## Prerequisites

| Tool | Location |
|---|---|
| Docker + Docker Compose | System PATH |
| ST-LINK debug probe | Connected via USB |
| STM32CubeProgrammer CLI | `/opt/st/stm32cubeide_2.2.0/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.linux64_2.2.500.202603051304/tools/bin/STM32_Programmer_CLI` |

> **Tip:** Add `STM32_Programmer_CLI` to your PATH to shorten the commands below:
> ```bash
> export PATH="/opt/st/stm32cubeide_2.2.0/plugins/com.st.stm32cube.ide.mcu.externaltools.cubeprogrammer.linux64_2.2.500.202603051304/tools/bin:$PATH"
> ```

---

## 1 — Build with Docker

The Docker container provides the ARM GNU toolchain without requiring a local install.

### Build CM7 only
```bash
docker compose run --rm builder make cm7
```

### Build CM4 only
```bash
docker compose run --rm builder make cm4
```

### Build both cores
```bash
docker compose run --rm builder make all
```

### Clean build outputs
```bash
docker compose run --rm builder make clean
```

Output binaries are placed in:
```
STM32CubeIDE/CM7/Release/riverdi-70-stm32h7-lvgl_CM7.hex
STM32CubeIDE/CM4/Release/riverdi-70-stm32h7-lvgl_CM4.hex
```

---

## 2 — Flash with STM32_Programmer_CLI

Connect the ST-LINK probe to the board's **SWD** header and power the board before flashing.

### Flash CM7 only
```bash
STM32_Programmer_CLI -c port=SWD \
  -w STM32CubeIDE/CM7/Release/riverdi-70-stm32h7-lvgl_CM7.elf \
  -rst
```

### Flash CM4 only
```bash
STM32_Programmer_CLI -c port=SWD \
  -w STM32CubeIDE/CM4/Release/riverdi-70-stm32h7-lvgl_CM4.elf \
  -rst
```

### Flash both cores (single reset at the end)
```bash
STM32_Programmer_CLI -c port=SWD \
  -w STM32CubeIDE/CM7/Release/riverdi-70-stm32h7-lvgl_CM7.elf \
  -w STM32CubeIDE/CM4/Release/riverdi-70-stm32h7-lvgl_CM4.elf \
  -rst
```

> **Note:** CM7 must always be flashed before CM4. CM7 is the boot master on the STM32H757;
> it is responsible for waking up the CM4 core at startup.

The `-rst` flag issues a **soft reset** immediately after flashing — the board starts running the
new firmware without needing a power cycle.

---

## 3 — One-shot: build + flash both cores

```bash
docker compose run --rm builder make all && \
STM32_Programmer_CLI -c port=SWD \
  -w STM32CubeIDE/CM7/Release/riverdi-70-stm32h7-lvgl_CM7.elf \
  -w STM32CubeIDE/CM4/Release/riverdi-70-stm32h7-lvgl_CM4.elf \
  -rst
```

---

## VS Code Task Explorer shortcuts

All the above commands are available as VS Code tasks (`.vscode/tasks.json`):

| Task label | Action |
|---|---|
| `Docker: Build CM4` | Build CM4 in Docker |
| `Docker: Build CM7` | Build CM7 in Docker |
| `Docker: Build All (CM4 + CM7)` | Build both cores in Docker |
| `Docker: Clean` | Clean all build outputs |
| `Flash: CM4` | Flash CM4 hex + soft reset |
| `Flash: CM7` | Flash CM7 hex + soft reset |
| `Flash: Both (CM7 then CM4 + reset)` | Flash both cores + soft reset |

Run via **Terminal → Run Task…** or the **Task Explorer** panel.
