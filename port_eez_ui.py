#!/usr/bin/env python3
import os
import sys
import shutil
import glob

# Constants
PROJECT_ROOT = "/home/alex/Documents/riverdi/LVGL_Demo_H7-H4_switch/lv_port_riverdi_70-stm32h7"
TARGET_SRC_DIR = os.path.join(PROJECT_ROOT, "CM4/Core/Src/eez_ui")
RELEASE_DIR = os.path.join(PROJECT_ROOT, "STM32CubeIDE/CM4/Release")
SUBDIR_MK_PATH = os.path.join(RELEASE_DIR, "Application/User/Core/eez_ui/subdir.mk")

def main():
    # 1. Parse command-line args for source directory
    source_dir = ""
    if len(sys.argv) > 1:
        source_dir = sys.argv[1]
    else:
        # Check if user generated code in default eez project path first
        default_project_path = os.path.join(PROJECT_ROOT, "EEZ/Riverdi-template/src/ui")
        if os.path.exists(default_project_path) and any(f.endswith('.c') for f in os.listdir(default_project_path)):
            source_dir = default_project_path
        else:
            source_dir = os.path.join(PROJECT_ROOT, "EEZ_Output")

    if not os.path.exists(source_dir):
        print(f"Source directory '{source_dir}' does not exist. Creating it...")
        os.makedirs(source_dir, exist_ok=True)

    print(f"Porting EEZ Studio project from: {source_dir}")
    print(f"Target UI directory: {TARGET_SRC_DIR}")

    c_files = []
    if os.path.abspath(source_dir) != os.path.abspath(TARGET_SRC_DIR):
        if os.path.exists(TARGET_SRC_DIR):
            shutil.rmtree(TARGET_SRC_DIR)
        os.makedirs(TARGET_SRC_DIR, exist_ok=True)

        for root_dir, dirs, files in os.walk(source_dir):
            for file in files:
                if file.endswith((".c", ".h")):
                    src_path = os.path.join(root_dir, file)
                    dest_path = os.path.join(TARGET_SRC_DIR, file)
                    shutil.copy2(src_path, dest_path)
                    if file.endswith(".c"):
                        c_files.append(file)
    else:
        # Just scan the target directory directly
        if os.path.exists(TARGET_SRC_DIR):
            for file in os.listdir(TARGET_SRC_DIR):
                if file.endswith(".c"):
                    c_files.append(file)

    print(f"Copied {len(c_files)} source files to {TARGET_SRC_DIR}")

    if not c_files:
        print("No EEZ UI files found in source. Generating default placeholder screen...")
        write_placeholder_files()
        c_files = ["ui.c"]

    # 4. Generate build directory if it does not exist
    eez_build_dir = os.path.dirname(SUBDIR_MK_PATH)
    os.makedirs(eez_build_dir, exist_ok=True)

    # 5. Generate subdir.mk
    generate_subdir_mk(c_files)
    print(f"Generated {SUBDIR_MK_PATH}")

    # 6. Update objects.list
    update_objects_list(c_files)

    # 7. Update sources.mk
    update_sources_mk()

    # 8. Update makefile
    update_makefile()

    print("EEZ project successfully ported to the CM4 firmware build environment!")

def generate_subdir_mk(c_files):
    c_srcs_lines = []
    objs_lines = []
    c_deps_lines = []
    rules_lines = []

    for f in sorted(c_files):
        src_path = f"{PROJECT_ROOT}/CM4/Core/Src/eez_ui/{f}"
        obj_name = f.replace(".c", ".o")
        dep_name = f.replace(".c", ".d")
        
        c_srcs_lines.append(f"{src_path} \\")
        objs_lines.append(f"./Application/User/Core/eez_ui/{obj_name} \\")
        c_deps_lines.append(f"./Application/User/Core/eez_ui/{dep_name} \\")

        # Explicit compile rule for this file
        rule = f"""Application/User/Core/eez_ui/{obj_name}: {src_path} Application/User/Core/eez_ui/subdir.mk
\tarm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32H747xx -c -I../../../CM4/Core/Inc -I../../../CM4/Core/Src/eez_ui -I../../../Middlewares/Third_Party/lvgl -I../../../Middlewares/Third_Party -I../../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../../Drivers/CMSIS/Include -I../../../Common -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" """
        rules_lines.append(rule)

    # Remove trailing backslash from the last item
    if c_srcs_lines:
        c_srcs_lines[-1] = c_srcs_lines[-1].rstrip(" \\")
    if objs_lines:
        objs_lines[-1] = objs_lines[-1].rstrip(" \\")
    if c_deps_lines:
        c_deps_lines[-1] = c_deps_lines[-1].rstrip(" \\")

    c_srcs_str = "\n".join(c_srcs_lines)
    objs_str = "\n".join(objs_lines)
    c_deps_str = "\n".join(c_deps_lines)
    rules_str = "\n".join(rules_lines)

    content = f"""################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (14.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \\
{c_srcs_str}

OBJS += \\
{objs_str}

C_DEPS += \\
{c_deps_str}


# Each subdirectory must supply rules for building sources it contributes
{rules_str}

clean: clean-Application-2f-User-2f-Core-2f-eez_ui

clean-Application-2f-User-2f-Core-2f-eez_ui:
\t-$(RM) ./Application/User/Core/eez_ui/*.cyclo ./Application/User/Core/eez_ui/*.d ./Application/User/Core/eez_ui/*.o ./Application/User/Core/eez_ui/*.su

.PHONY: clean-Application-2f-User-2f-Core-2f-eez_ui
"""
    with open(SUBDIR_MK_PATH, "w") as f:
        f.write(content)

def update_objects_list(c_files):
    objects_file = os.path.join(RELEASE_DIR, "objects.list")
    if not os.path.exists(objects_file):
        return

    with open(objects_file, "r") as f:
        lines = f.readlines()

    # Filter out any existing eez_ui entries
    new_lines = [l for l in lines if "eez_ui/" not in l]

    # Append new entries
    for f in sorted(c_files):
        obj_name = f.replace(".c", ".o")
        new_lines.append(f'"./Application/User/Core/eez_ui/{obj_name}"\n')

    # Remove any extra empty lines and save
    new_lines = [l for l in new_lines if l.strip()]
    with open(objects_file, "w") as f:
        f.writelines(sorted(list(set(new_lines))))
    print(f"Updated {objects_file}")

def update_sources_mk():
    sources_file = os.path.join(RELEASE_DIR, "sources.mk")
    if not os.path.exists(sources_file):
        return

    with open(sources_file, "r") as f:
        content = f.read()

    target_subdir = "Application/User/Core/eez_ui \\"
    if target_subdir not in content:
        # Insert it in the SUBDIRS list
        lines = content.splitlines()
        inserted = False
        for idx, line in enumerate(lines):
            if "SUBDIRS := \\" in line:
                lines.insert(idx + 1, target_subdir)
                inserted = True
                break
        if inserted:
            with open(sources_file, "w") as f:
                f.write("\n".join(lines) + "\n")
            print(f"Updated {sources_file}")

def update_makefile():
    makefile_file = os.path.join(RELEASE_DIR, "makefile")
    if not os.path.exists(makefile_file):
        return

    with open(makefile_file, "r") as f:
        lines = f.read().splitlines()

    target_include = "-include Application/User/Core/eez_ui/subdir.mk"
    if target_include not in lines:
        # Insert it after other -include lines
        inserted_idx = -1
        for idx, line in enumerate(lines):
            if line.startswith("-include") and "subdir.mk" in line:
                inserted_idx = idx
        
        if inserted_idx != -1:
            lines.insert(inserted_idx + 1, target_include)
            with open(makefile_file, "w") as f:
                f.write("\n".join(lines) + "\n")
            print(f"Updated {makefile_file}")

def write_placeholder_files():
    h_content = """#ifndef EEZ_LVGL_UI_GUI_H
#define EEZ_LVGL_UI_GUI_H

#ifdef __cplusplus
extern "C" {
#endif

void ui_init();
void ui_tick();

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_GUI_H
"""
    c_content = """#include "ui.h"
#include "lvgl/lvgl.h"

void ui_init() {
    /* Create dark blue background and centered label with text 'TWERD ENERGO-PLUS' */
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x00003B), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "TWERD ENERGO-PLUS");
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void ui_tick() {
    // Nothing to do in the dummy loop
}
"""
    with open(os.path.join(TARGET_SRC_DIR, "ui.h"), "w") as f:
        f.write(h_content)
    with open(os.path.join(TARGET_SRC_DIR, "ui.c"), "w") as f:
        f.write(c_content)

if __name__ == "__main__":
    main()
