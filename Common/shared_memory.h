#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdint.h>

/* Shared Memory Address: SRAM3 in D2 domain is visible to both CM7 and CM4 */
#define SHARED_SRAM_ADDR          ((uint32_t)0x30040000)

/* Hardware Semaphore ID for protecting shared buffer access */
#define HSEM_ID_SHARED_MEM        (1)

/* Telemetry data layout shared between CM7 (Math/Comms) and CM4 (Graphics) */
typedef struct {
  /* Solar Inputs (Calculated by CM7, Read by CM4) */
  float pv_voltage;
  float pv_current;
  float pv_power;

  /* Battery status */
  float bat_voltage;
  float bat_current;
  float bat_soc;
  uint8_t bat_charging_status; /* 0=Idle, 1=Charging, 2=Discharging */

  /* AC Grid parameters */
  float grid_voltage_l1;
  float grid_frequency;
  float grid_active_power;

  /* Diagnostics info */
  float heatsink_temp;
  uint32_t active_fault_code; /* Error word */

  /* Configurations edited by CM4 UI buttons, parsed by CM7 controller */
  uint16_t config_grid_voltage; /* e.g. 230 V */
  uint16_t config_grid_freq;    /* e.g. 50 Hz */
  float config_bat_max_voltage; /* e.g. 58.4 V */

  /* User login state (Written by CM4 UI, read by CM7) */
  char login_username[32];
  uint8_t login_access_level; /* AccessLevel_t */
} SharedBuffer_t;

typedef enum {
  ACCESS_LEVEL_GUEST = 0,
  ACCESS_LEVEL_OPERATOR = 1,
  ACCESS_LEVEL_ADMIN = 2
} AccessLevel_t;

/* Helper macros for accessing the shared buffer pointer.
 * On the PC Simulator (PC_SIMULATOR=1) this macro is supplied via CMake
 * target_compile_definitions, pointing to the mock struct in pc_simulator_hw.c.
 * On the real STM32H7 target it maps directly to the SRAM3 hardware address. */
#ifndef PC_SIMULATOR
#define SHARED_BUFFER             ((volatile SharedBuffer_t *)SHARED_SRAM_ADDR)
#endif

#endif /* SHARED_MEMORY_H */
