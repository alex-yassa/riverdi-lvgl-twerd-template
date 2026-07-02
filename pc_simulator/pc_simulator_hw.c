/*
 * pc_simulator_hw.c
 *
 * Provides mock implementations of hardware-dependent symbols referenced by
 * the shared UI code when building for the host PC target.
 *
 * On the real STM32H7 target these are implemented in:
 *   CM4/Core/Src/main.c  (HAL_Delay, HSEM, NVIC callbacks)
 *   Common/shared_memory.h (SHARED_BUFFER mapped to 0x30040000)
 *
 * On the PC simulator we replace hardware addresses with a statically
 * allocated struct populated with realistic telemetry values so the UI
 * renders data without requiring actual hardware.
 */

#include "shared_memory.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* -----------------------------------------------------------------------
 * Mock Shared Buffer
 * Populated with realistic startup values so widgets show meaningful data.
 * CM4 main.c accesses this via the SHARED_BUFFER macro; we redefine the
 * macro target below so it points to this struct instead of 0x30040000.
 * ----------------------------------------------------------------------- */
SharedBuffer_t sim_shared_buffer = {
    .pv_voltage          = 385.0f,
    .pv_current          = 11.0f,
    .pv_power            = 4.235f,
    .bat_voltage         = 51.2f,
    .bat_current         = 15.5f,
    .bat_soc             = 87.0f,
    .bat_charging_status = 1,         /* Charging */
    .grid_voltage_l1     = 230.0f,
    .grid_frequency      = 50.0f,
    .grid_active_power   = 5.5f,
    .heatsink_temp       = 42.0f,
    .active_fault_code   = 0,
    .config_grid_voltage = 230,
    .config_grid_freq    = 50,
    .config_bat_max_voltage = 58.4f,
};

/*
 * Override the SHARED_BUFFER macro so any code that calls
 *   SHARED_BUFFER->pv_voltage
 * actually reads/writes sim_shared_buffer on the host instead of
 * dereferencing the raw SRAM3 address (0x30040000).
 *
 * This is done via a compiler -D flag in CMakeLists.txt:
 *   add_compile_definitions(SHARED_BUFFER=((volatile SharedBuffer_t*)&sim_shared_buffer))
 *
 * The definition is repeated here as documentation. The CMakeLists.txt
 * passes it at build time so it applies to every translation unit.
 */

/* -----------------------------------------------------------------------
 * HAL stub — HAL_Delay is called from some init paths.
 * On the simulator we simply do nothing.
 * ----------------------------------------------------------------------- */
void HAL_Delay(uint32_t Delay)
{
    (void)Delay;
    /* No-op on PC simulator */
}

/* -----------------------------------------------------------------------
 * Logging stub — called optionally from UI tick code.
 * Prints to stdout so it is visible during development.
 * ----------------------------------------------------------------------- */
void sim_log(const char *msg)
{
    printf("[PC SIM] %s\n", msg);
}
