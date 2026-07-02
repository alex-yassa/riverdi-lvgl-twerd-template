#include "lv_port_disp.h"
#include "main.h"

#define LCD_SCREEN_WIDTH                  ((uint16_t)1024)
#define LCD_SCREEN_HEIGHT                 ((uint16_t)600)
#define SDRAM_DEVICE_ADDR                 ((uint32_t)0xD0000000)

/* Extern handles from main.c */
extern DMA2D_HandleTypeDef hdma2d;

/* Declare two draw buffers in internal SRAM (15 lines of RGB565) */
static __attribute__((aligned(32))) lv_color_t lv_disp_buf1[LCD_SCREEN_WIDTH * 15];
static __attribute__((aligned(32))) lv_color_t lv_disp_buf2[LCD_SCREEN_WIDTH * 15];

static lv_disp_drv_t disp_drv;

/* Private callback prototypes */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
static void DMA2D_CopyBuffer(void *pSrc, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t srcOffset, uint32_t dstOffset);

void lv_port_disp_init(void)
{
  static lv_disp_draw_buf_t draw_buf_dsc;

  /* Initialize draw buffer structure */
  lv_disp_draw_buf_init(&draw_buf_dsc, lv_disp_buf1, lv_disp_buf2, LCD_SCREEN_WIDTH * 15);

  /* Initialize display driver structure */
  lv_disp_drv_init(&disp_drv);

  /* Set display resolution */
  disp_drv.hor_res = LCD_SCREEN_WIDTH;
  disp_drv.ver_res = LCD_SCREEN_HEIGHT;

  /* Register flush callback */
  disp_drv.flush_cb = disp_flush;

  /* Link the draw buffer */
  disp_drv.draw_buf = &draw_buf_dsc;

  /* Register the driver in LVGL */
  lv_disp_drv_register(&disp_drv);
}

/**
  * @brief LVGL flush callback. Renders area on the LCD using hardware DMA2D
  */
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
  /* Calculate the address of the target window in the external SDRAM frame buffer */
  uint32_t width = area->x2 - area->x1 + 1;
  uint32_t height = area->y2 - area->y1 + 1;
  
  /* Active LCD frame buffer address is SDRAM_DEVICE_ADDR (now ARGB8888 32-bit) */
  uint32_t *fb = (uint32_t *)SDRAM_DEVICE_ADDR;
  uint32_t *dst_addr = fb + (area->y1 * LCD_SCREEN_WIDTH) + area->x1;

  uint32_t srcOffset = 0;
  uint32_t dstOffset = LCD_SCREEN_WIDTH - width;

  /* Copy buffer using DMA2D accelerator with Pixel Format Conversion */
  DMA2D_CopyBuffer((void *)color_p, (void *)dst_addr, width, height, srcOffset, dstOffset);

  /* Call display flush ready to release LVGL draw buffer lock */
  lv_disp_flush_ready(disp_drv);
}

/**
  * @brief Hardware accelerated buffer copy using DMA2D (Chrom-ART) with PFC
  */
static void DMA2D_CopyBuffer(void *pSrc, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t srcOffset, uint32_t dstOffset)
{
  /* Configure DMA2D Transfer */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M_PFC;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = dstOffset;

  /* Foreground Configuration */
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0xFF;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_RGB565;
  hdma2d.LayerCfg[1].InputOffset = srcOffset;

  if (HAL_DMA2D_Init(&hdma2d) == HAL_OK)
  {
    if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) == HAL_OK)
    {
      /* Start hardware DMA2D transfer and wait until completion */
      HAL_DMA2D_Start(&hdma2d, (uint32_t)pSrc, (uint32_t)pDst, xSize, ySize);
      HAL_DMA2D_PollForTransfer(&hdma2d, 100);
    }
  }
}
