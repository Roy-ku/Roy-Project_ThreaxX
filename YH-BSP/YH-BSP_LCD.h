#ifndef __YH_BSP_LCD_H
#define __YH_BSP_LCD_H

/* Includes ------------------------------------------------------------------*/
#include "YH-BSP.h"
#include "./fonts/fonts.h"

/* LCD 层像素格式*/
#define ARGB8888 LTDC_PIXEL_FORMAT_ARGB8888 /*!< ARGB8888 LTDC像素格式 */
#define RGB888 LTDC_PIXEL_FORMAT_RGB888     /*!< RGB888 LTDC像素格式   */
#define RGB565 LTDC_PIXEL_FORMAT_RGB565     /*!< RGB565 LTDC像素格式   */
#define ARGB1555 LTDC_PIXEL_FORMAT_ARGB1555 /*!< ARGB1555 LTDC像素格式 */
#define ARGB4444 LTDC_PIXEL_FORMAT_ARGB4444 /*!< ARGB4444 LTDC像素格式 */

/**
 * @brief  LCD液晶參數
 */
typedef struct
{
  uint8_t hbp; // HSYNC后的无效像素
  uint8_t vbp; // VSYNC后的无效行数

  uint8_t hsw; // HSYNC宽度
  uint8_t vsw; // VSYNC宽度

  uint8_t hfp; // HSYNC前的无效像素
  uint8_t vfp; // VSYNC前的无效行数

  uint8_t comment_clock_2byte; // rgb565/argb4444等双字节像素时推荐使用的液晶时钟频率
  uint8_t comment_clock_4byte; // Argb8888等四字节像素时推荐使用的液晶时钟频率

  uint16_t lcd_pixel_width;  //液晶分辨率，宽
  uint16_t lcd_pixel_height; //液晶分辨率，高

  uint16_t m_palette_btn_width;  //液晶分辨率，宽
  uint16_t m_palette_btn_height; //液晶分辨率，高

} LCD_PARAM_TypeDef;

/* LCD液晶參數  */
#define LCD_PIXEL_WIDTH lcd_param[cur_lcd].lcd_pixel_width   // 實際大小 寬度
#define LCD_PIXEL_HEIGHT lcd_param[cur_lcd].lcd_pixel_height // 實際大小 高度

#define HBP lcd_param[cur_lcd].hbp // HSYNC后的无效像素
#define VBP lcd_param[cur_lcd].vbp // VSYNC后的无效行数

#define HSW lcd_param[cur_lcd].hsw // HSYNC宽度
#define VSW lcd_param[cur_lcd].vsw // VSYNC宽度

#define HFP lcd_param[cur_lcd].hfp // HSYNC前的无效像素
#define VFP lcd_param[cur_lcd].vfp // VSYNC前的无效行数

/**
 * @brief  LCD類型
 */
typedef enum
{
  INCH_5 = 0x00, /* 5寸 */
  INCH_7,        /* 7寸 */
  INCH_4_3,      /* 4.3寸 */

  LCD_TYPE_NUM /* LCD類型總數*/
} LCD_TypeDef;

/* 當前使用的LCD，默認為5寸屏 */
extern LCD_TypeDef cur_lcd;
/* 不同液晶屏的參數 */
extern const LCD_PARAM_TypeDef lcd_param[];

typedef struct
{
  uint32_t TextColor;
  uint32_t BackColor;
  sFONT *pFont;
} LCD_DrawPropTypeDef;

typedef struct
{
  int16_t X;
  int16_t Y;
} Point, *pPoint;

/**
 * @brief  字體對齊模式
 */
typedef enum
{
  CENTER_MODE = 0x01, /* 居中對齊 */
  RIGHT_MODE = 0x02,  /* 右對齊   */
  LEFT_MODE = 0x03    /* 左對齊   */
} Text_AlignModeTypdef;

#define MAX_LAYER_NUMBER ((uint32_t)2)

/**
 * @brief  LCD color
 */
#define LCD_COLOR_BLUE ((uint32_t)0xFF0000FF)
#define LCD_COLOR_GREEN ((uint32_t)0xFF00FF00)
#define LCD_COLOR_RED ((uint32_t)0xFFFF0000)
#define LCD_COLOR_CYAN ((uint32_t)0xFF00FFFF)
#define LCD_COLOR_MAGENTA ((uint32_t)0xFFFF00FF)
#define LCD_COLOR_YELLOW ((uint32_t)0xFFFFFF00)
#define LCD_COLOR_LIGHTBLUE ((uint32_t)0xFF8080FF)
#define LCD_COLOR_LIGHTGREEN ((uint32_t)0xFF80FF80)
#define LCD_COLOR_LIGHTRED ((uint32_t)0xFFFF8080)
#define LCD_COLOR_LIGHTCYAN ((uint32_t)0xFF80FFFF)
#define LCD_COLOR_LIGHTMAGENTA ((uint32_t)0xFFFF80FF)
#define LCD_COLOR_LIGHTYELLOW ((uint32_t)0xFFFFFF80)
#define LCD_COLOR_DARKBLUE ((uint32_t)0xFF000080)
#define LCD_COLOR_DARKGREEN ((uint32_t)0xFF008000)
#define LCD_COLOR_DARKRED ((uint32_t)0xFF800000)
#define LCD_COLOR_DARKCYAN ((uint32_t)0xFF008080)
#define LCD_COLOR_DARKMAGENTA ((uint32_t)0xFF800080)
#define LCD_COLOR_DARKYELLOW ((uint32_t)0xFF808000)
#define LCD_COLOR_WHITE ((uint32_t)0xFFFFFFFF)
#define LCD_COLOR_LIGHTGRAY ((uint32_t)0xFFD3D3D3)
#define LCD_COLOR_GRAY ((uint32_t)0xFF808080)
#define LCD_COLOR_DARKGRAY ((uint32_t)0xFF404040)
#define LCD_COLOR_BLACK ((uint32_t)0xFF000000)
#define LCD_COLOR_BROWN ((uint32_t)0xFFA52A2A)
#define LCD_COLOR_ORANGE ((uint32_t)0xFFFFA500)
#define LCD_COLOR_TRANSPARENT ((uint32_t)0xFF000000)

/**
 * @brief LCD default font
 */
#define LCD_DEFAULT_FONT Font24

extern __IO uint8_t dma2d_TransferComplete_Flag;
void bsp_LCD_Init(void);
uint8_t LCD_DeInit(void);
uint32_t bsp_LCD_GetXSize(void);
uint32_t bsp_LCD_GetYSize(void);
void bsp_LCD_SetXSize(uint32_t imageWidthPixels);
void bsp_LCD_SetYSize(uint32_t imageHeightPixels);

/* Functions using the LTDC controller */
void LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FrameBuffer);
void LCD_LayerRgb565Init(uint16_t LayerIndex, uint32_t FB_Address);
void bsp_LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency);
void bsp_LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address);
void LCD_SetColorKeying(uint32_t LayerIndex, uint32_t RGBValue);
void LCD_ResetColorKeying(uint32_t LayerIndex);
void bsp_LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);

void bsp_LCD_SelectLayer(uint32_t LayerIndex);
void bsp_LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State);

void bsp_LCD_SetTextColor(uint32_t Color);
uint32_t bsp_LCD_GetTextColor(void);
void bsp_LCD_SetBackColor(uint32_t Color);
uint32_t bsp_LCD_GetBackColor(void);

void bsp_LCD_SetColors(uint32_t TextColor, uint32_t BackColor);

void bsp_LCD_SetFont(sFONT *fonts);
sFONT *bsp_LCD_GetFont(void);

uint32_t bsp_LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void bsp_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t pixel);
void bsp_LCD_Clear(uint32_t Color);
void bsp_LCD_ClearLine(uint32_t Line);
void bsp_LCD_DisplayStringLine(uint16_t Line, uint8_t *ptr);
void bsp_LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode);
void bsp_LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);

void bsp_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void bsp_LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void bsp_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void bsp_LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void bsp_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void bsp_LCD_DrawPolygon(pPoint Points, uint16_t PointCount);
void bsp_LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
void bsp_LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp);

void bsp_LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void bsp_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void bsp_LCD_FillPolygon(pPoint Points, uint16_t PointCount);
void bsp_LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
void bsp_LCD_Color_Flush(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t Color);
void bsp_LCD_Copy_Flush(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t *Color);
void bsp_LCD_ShowPicture(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t Color);

void bsp_LCD_Display_ON(void);
void bsp_LCD_Display_OFF(void);

void bsp_LCD_Backlight_ON(void);
void bsp_LCD_Backlight_OFF(void);
/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
void LCD_MspInit(LTDC_HandleTypeDef *hltdc, void *Params);
void LCD_MspDeInit(LTDC_HandleTypeDef *hltdc, void *Params);
void LCD_ClockConfig(void);

void bsp_LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address, uint32_t PixelFormat);
void bsp_LCD_Test(void);
void bsp_LCD_Test_2(void);
void bsp_LCD_Test_3(void);
void bsp_LCD_Test_4(int32_t id, int32_t x, int32_t y, int32_t w);
#endif /* __BSP_LCD_H */
