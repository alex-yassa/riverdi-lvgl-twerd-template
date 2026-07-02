#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/* Screen dimensions */
#define LV_HOR_RES_MAX          (1024)
#define LV_VER_RES_MAX          (600)

/* Color settings */
#define LV_COLOR_DEPTH          16  /* RGB565 */
#define LV_COLOR_16_SWAP        0   /* No swap for STM32 LTDC */

/* Memory management */
#define LV_MEM_CUSTOM           0   /* Use LVGL's internal memory manager */
#define LV_MEM_SIZE             (128U * 1024U) /* Heap size (128 KB) in D1 AXI SRAM */
#ifdef STM32H757xx
#define LV_MEM_ADR              0x24020000     /* Placed inside D1 RAM for STM32 */
#else
#define LV_MEM_ADR              0              /* Custom address disabled for PC */
#endif

/* Hardware acceleration */
#ifdef STM32H757xx
#define LV_USE_GPU_STM32_DMA2D  1   /* Enable STM32 Chrom-ART (DMA2D) */
#define LV_GPU_DMA2D_CMSIS_INCLUDE "stm32h7xx.h"
#else
#define LV_USE_GPU_STM32_DMA2D  0   /* Disabled for PC Simulator */
#endif

/* Timer configuration */
#define LV_DISP_DEF_REFR_PERIOD 30  /* ms */
#define LV_INDEV_DEF_READ_PERIOD 30 /* ms */

/* Feature usage */
#define LV_USE_LOG              1
#define LV_LOG_LEVEL            LV_LOG_LEVEL_WARN

#define LV_USE_ASSERT_NULL      1
#define LV_USE_ASSERT_MEM       1
#define LV_USE_ASSERT_OBJ       1

#define LV_USE_USER_DATA        1

/* Widget configuration */
#define LV_USE_ARC              1
#define LV_USE_BAR              1
#define LV_USE_BTN              1
#define LV_USE_BTNMATRIX        1
#define LV_USE_CANVAS           1
#define LV_USE_CHECKBOX         1
#define LV_USE_CHART            1
#define LV_USE_DROPDOWN         1
#define LV_USE_IMG              1
#define LV_USE_LABEL            1
#define LV_USE_LINE             1
#define LV_USE_LIST             1
#define LV_USE_METER            1
#define LV_USE_ROLLER           1
#define LV_USE_SLIDER           1
#define LV_USE_SPINBOX          1
#define LV_USE_SWITCH           1
#define LV_USE_TEXTAREA         1
#define LV_USE_TABLE            1
#define LV_USE_TABVIEW          1

/* Active Fonts configuration */
#define LV_FONT_MONTSERRAT_10   1
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_MONTSERRAT_32   1
#define LV_FONT_MONTSERRAT_48   1

#define LV_FONT_DEFAULT         &lv_font_montserrat_14

/* Navigation key configs */
#define LV_USE_THEME_DEFAULT    1
#define LV_THEME_DEFAULT_DARK   1
#define LV_THEME_DEFAULT_GROW   1

/* Layouts */
#define LV_USE_FLEX             1
#define LV_USE_GRID             1

/* Compatibility font macros for EEZ Studio */
#define lv_font_montserrat_46 lv_font_montserrat_48

#endif /* LV_CONF_H */
