#include "GT9xx.h"
RAM_D2 struct i2c_msg msgs[2];
/* 触摸IC类型默认为5寸屏的ic */
TOUCH_IC touchIC = GT9157;

const TOUCH_PARAM_TypeDef touch_param[4] =
    {
        /* GT9157,5寸屏 */
        {
            .max_width = 800,
            .max_height = 480,
            .config_reg_addr = 0x8047,
        },
        /* GT911,7寸屏 */
        {
            .max_width = 800,
            .max_height = 480,
            .config_reg_addr = 0x8047,
        },
        /* GT5688,4.3寸屏 */
        {
            .max_width = 480,
            .max_height = 272,
            .config_reg_addr = 0x8050,
        },
        /* GT917S,5寸屏 */
        {
            .max_width = 800,
            .max_height = 480,
            .config_reg_addr = 0x8050,
        },
};

static int8_t GTP_I2C_Test(void);

/**
 * @brief   使用IIC进行数据传输
 * @param
 *		@arg i2c_msg:数据传输结构体
 *		@arg num:数据传输结构体的个数
 * @retval  正常完成的传输结构个数，若不正常，返回0xff
 */
static int I2C_Transfer(struct i2c_msg *msgs, int num)
{
  int im = 0;
  int ret = 0;

  GTP_DEBUG_FUNC();

  for (im = 0; ret == 0 && im != num; im++)
  {
    if ((msgs[im].flags & I2C_M_RD)) //根据flag判断是读数据还是写数据
    {
      ret = I2C_ReadBytes(msgs[im].addr, msgs[im].buf, msgs[im].len); // IIC读取数据
    }
    else
    {
      ret = I2C_WriteBytes(msgs[im].addr, msgs[im].buf, msgs[im].len); // IIC写入数据
    }
  }

  if (ret)
    return ret;

  return im; //正常完成的传输结构个数
}

/**
 * @brief   从IIC设备中读取数据
 * @param
 *		@arg client_addr:设备地址
 *		@arg  buf[0~1]: 读取数据寄存器的起始地址
 *		@arg buf[2~len-1]: 存储读出来数据的缓冲buffer
 *		@arg len:    GTP_ADDR_LENGTH + read bytes count（寄存器地址长度+读取的数据字节数）
 * @retval  i2c_msgs传输结构体的个数，2为成功，其它为失败
 */
static int32_t GTP_I2C_Read(uint8_t client_addr, uint8_t *buf, int32_t len)
{

  int32_t ret = -1;
  int32_t retries = 0;

  GTP_DEBUG_FUNC();
  /*一个读数据的过程可以分为两个传输过程:
   * 1. IIC  写入 要读取的寄存器地址
   * 2. IIC  读取  数据
   * */

  msgs[0].flags = !I2C_M_RD;     //写入
  msgs[0].addr = client_addr;    // IIC设备地址
  msgs[0].len = GTP_ADDR_LENGTH; //寄存器地址为2字节(即写入两字节的数据)
  msgs[0].buf = &buf[0];         // buf[0~1]存储的是要读取的寄存器地址

  msgs[1].flags = I2C_M_RD;            //读取
  msgs[1].addr = client_addr;          // IIC设备地址
  msgs[1].len = len - GTP_ADDR_LENGTH; //要读取的数据长度
  msgs[1].buf = &buf[GTP_ADDR_LENGTH]; // buf[GTP_ADDR_LENGTH]之后的缓冲区存储读出的数据

  while (retries < 5)
  {
    ret = I2C_Transfer(msgs, 2); //调用IIC数据传输过程函数，有2个传输过程
    if (ret == 2)
      break;
    retries++;
  }
  if ((retries >= 5))
  {
    bsp_ERROR("I2C Read: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((uint16_t)(buf[0] << 8)) | buf[1]), len - 2, ret);
  }
  return ret;
}

/**
 * @brief   向IIC设备写入数据
 * @param
 *		@arg client_addr:设备地址
 *		@arg  buf[0~1]: 要写入的数据寄存器的起始地址
 *		@arg buf[2~len-1]: 要写入的数据
 *		@arg len:    GTP_ADDR_LENGTH + write bytes count（寄存器地址长度+写入的数据字节数）
 * @retval  i2c_msgs传输结构体的个数，1为成功，其它为失败
 */
static int32_t GTP_I2C_Write(uint8_t client_addr, uint8_t *buf, int32_t len)
{
  struct i2c_msg msg;
  int32_t ret = -1;
  int32_t retries = 0;

  GTP_DEBUG_FUNC();
  /*一个写数据的过程只需要一个传输过程:
   * 1. IIC连续 写入 数据寄存器地址及数据
   * */
  msg.flags = !I2C_M_RD;  //写入
  msg.addr = client_addr; //从设备地址
  msg.len = len;          //长度直接等于(寄存器地址长度+写入的数据字节数)
  msg.buf = buf;          //直接连续写入缓冲区中的数据(包括了寄存器地址)

  while (retries < 5)
  {
    ret = I2C_Transfer(&msg, 1); //调用IIC数据传输过程函数，1个传输过程
    if (ret == 1)
      break;
    retries++;
  }
  if ((retries >= 5))
  {

    bsp_ERROR("I2C Write: 0x%04X, %d bytes failed, errcode: %d! Process reset.", (((uint16_t)(buf[0] << 8)) | buf[1]), len - 2, ret);
  }
  return ret;
}

/**
 * @brief   关闭GT91xx中断
 * @param 无
 * @retval 无
 */
void GTP_IRQ_Disable(void)
{
  GTP_DEBUG_FUNC();
  I2C_GTP_IRQDisable();
}

/**
 * @brief   使能GT91xx中断
 * @param 无
 * @retval 无
 */
void GTP_IRQ_Enable(void)
{
  GTP_DEBUG_FUNC();
  I2C_GTP_IRQEnable();
}

/**
 * @brief   用于处理或报告触屏检测到按下
 * @param
 *    @arg     id: 触摸顺序trackID
 *    @arg     x:  触摸的 x 坐标
 *    @arg     y:  触摸的 y 坐标
 *    @arg     w:  触摸的 大小
 * @retval 无
 */

/*用於記錄連續觸摸時(長按)的上一次觸摸位置，負數值表示上一次無觸摸按下*/
static int16_t pre_x[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};
static int16_t pre_y[GTP_MAX_TOUCH] = {-1, -1, -1, -1, -1};

static void GTP_Touch_Down(int32_t id, int32_t x, int32_t y, int32_t w)
{

  GTP_DEBUG_FUNC();

  /*取x、y初始值大於屏幕像素值*/
  bsp_DEBUG("ID:%d, X:%d, Y:%d, W:%d", id, x, y, w);

  /************************************/
  /*在此處添加自己的觸摸點按下時處理過程即可*/
  /* (x,y) 即為最新的觸摸點 *************/
  /************************************/
  //bsp_LCD_Test_4(id, x, y, w);
  /*prex,prey數組存儲上一次觸摸的位置，id為軌跡編號(多點觸控時有多軌跡)*/
  pre_x[id] = x;
  pre_y[id] = y;
}

/**
 * @brief   用于处理或报告触屏释放
 * @param 释放点的id号
 * @retval 无
 */
static void GTP_Touch_Up(int32_t id)
{
  /************************************/
  /*在此處添加自己的觸摸點按下時處理過程即可*/
  /* (x,y) 即為最新的觸摸點 *************/
  /************************************/

  /*觸筆釋放，把pre xy 重置為負*/
  pre_x[id] = -1;
  pre_y[id] = -1;

  bsp_DEBUG("Touch id[%2d] release!", id);
}

/**
 * @brief   触屏处理函数，轮询或者在触摸中断调用
 * @param 无
 * @retval 无
 */
static void Goodix_TS_Work_Func(void)
{
  uint8_t end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
  uint8_t point_data[2 + 1 + 8 * GTP_MAX_TOUCH + 1] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
  uint8_t touch_num = 0;
  uint8_t finger = 0;
  static uint16_t pre_touch = 0;
  static uint8_t pre_id[GTP_MAX_TOUCH] = {0};

  uint8_t client_addr = GTP_ADDRESS;
  uint8_t *coor_data = NULL;
  int32_t input_x = 0;
  int32_t input_y = 0;
  int32_t input_w = 0;
  uint8_t id = 0;

  int32_t i = 0;
  int32_t ret = -1;

  GTP_DEBUG_FUNC();

  ret = GTP_I2C_Read(client_addr, point_data, 12); // 10字节寄存器加2字节地址
  if (ret < 0)
  {
    bsp_ERROR("I2C transfer error. errno:%d\n ", ret);

    return;
  }

  finger = point_data[GTP_ADDR_LENGTH]; //状态寄存器数据

  if (finger == 0x00) //没有数据，退出
  {
    return;
  }

  if ((finger & 0x80) == 0) //判断buffer status位
  {
    goto exit_work_func; //坐标未就绪，数据无效
  }

  touch_num = finger & 0x0f; //坐标点数
  if (touch_num > GTP_MAX_TOUCH)
  {
    goto exit_work_func; //大于最大支持点数，错误退出
  }

  if (touch_num > 1) //不止一个点
  {
    uint8_t buf[8 * GTP_MAX_TOUCH] = {(GTP_READ_COOR_ADDR + 10) >> 8, (GTP_READ_COOR_ADDR + 10) & 0xff};

    ret = GTP_I2C_Read(client_addr, buf, 2 + 8 * (touch_num - 1));
    memcpy(&point_data[12], &buf[2], 8 * (touch_num - 1)); //复制其余点数的数据到point_data
  }

  if (pre_touch > touch_num) // pre_touch>touch_num,表示有的点释放了
  {
    for (i = 0; i < pre_touch; i++) //一个点一个点处理
    {
      uint8_t j;
      for (j = 0; j < touch_num; j++)
      {
        coor_data = &point_data[j * 8 + 3];
        id = coor_data[0] & 0x0F; // track id
        if (pre_id[i] == id)
          break;

        if (j >= touch_num - 1) //遍历当前所有id都找不到pre_id[i]，表示已释放
        {
          GTP_Touch_Up(pre_id[i]);
        }
      }
    }
  }

  if (touch_num)
  {
    for (i = 0; i < touch_num; i++) //一个点一个点处理
    {
      coor_data = &point_data[i * 8 + 3];

      id = coor_data[0] & 0x0F; // track id
      pre_id[i] = id;

      input_x = coor_data[1] | (coor_data[2] << 8); // x坐标
      input_y = coor_data[3] | (coor_data[4] << 8); // y坐标
      input_w = coor_data[5] | (coor_data[6] << 8); // size

      {
        GTP_Touch_Down(id, input_x, input_y, input_w); //数据处理
      }
    }
  }
  else if (pre_touch) // touch_ num=0 且pre_touch！=0
  {
    for (i = 0; i < pre_touch; i++)
    {
      GTP_Touch_Up(pre_id[i]);
    }
  }

  pre_touch = touch_num;

exit_work_func:
{
  ret = GTP_I2C_Write(client_addr, end_cmd, 3);
  if (ret < 0)
  {
    bsp_INFO("GTP write end_cmd error!");
  }
}
}

/**
 * @brief   给触屏芯片重新复位
 * @param 无
 * @retval 无
 */
int8_t GTP_Reset_Guitar(void)
{
  GTP_DEBUG_FUNC();
#if 1
  I2C_ResetChip();
  return 0;
#else //软件复位
  int8_t ret = -1;
  int8_t retry = 0;
  uint8_t reset_command[3] = {(uint8_t)GTP_REG_COMMAND >> 8, (uint8_t)GTP_REG_COMMAND & 0xFF, 2};

  //写入复位命令
  while (retry++ < 5)
  {
    ret = GTP_I2C_Write(GTP_ADDRESS, reset_command, 3);
    if (ret > 0)
    {
      bsp_INFO("GTP enter sleep!");

      return ret;
    }
  }
  bsp_ERROR("GTP send sleep cmd failed.");
  return ret;
#endif
}

/*******************************************************
Function:
    Initialize gtp.
Input:
    ts: goodix private data
Output:
    Executive outcomes.
        0: succeed, otherwise: failed
*******************************************************/
int32_t GTP_Init_Panel(void)
{
  GTP_DEBUG_FUNC();
  I2C_Touch_Init();
  return 0;
}

/*******************************************************
Function:
    I2c test Function.
Input:
    client:i2c client.
Output:
    Executive outcomes.
        2: succeed, otherwise failed.
*******************************************************/
static int8_t GTP_I2C_Test(void)
{
  uint8_t test[3] = {GTP_REG_CONFIG_DATA >> 8, GTP_REG_CONFIG_DATA & 0xff};
  uint8_t retry = 0;
  int8_t ret = -1;

  GTP_DEBUG_FUNC();

  while (retry++ < 5)
  {
    ret = GTP_I2C_Read(GTP_ADDRESS, test, 3);
    if (ret > 0)
    {
      return ret;
    }
    bsp_ERROR("GTP i2c test failed time %d.", retry);
  }
  return ret;
}

//检测到触摸中断时调用，
void GTP_TouchProcess(void)
{
  GTP_DEBUG_FUNC();
  Goodix_TS_Work_Func();
}

/**
 * @brief  触屏检测函数，本函数作为GUI的定制检测函数，
 *        参考Goodix_TS_Work_Func修改而来， 只读取单个触摸点坐标
 * @param x[out] y[out] 读取到的坐标
 * @retval 坐标有效返回1，否则返回0
 */
int GTP_Execu(int *x, int *y, int *w)
{
  uint8_t end_cmd[3] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF, 0};
  uint8_t touch_num = 0;

  int32_t ret = -1;

  if (Touch_event == 1)
  {
    // 2-寄存器地址 1-状态寄存器 8*1-每个触摸点使用8个寄存器
    uint8_t point_data[2 + 1 + 8 * 1 + 1] = {GTP_READ_COOR_ADDR >> 8, GTP_READ_COOR_ADDR & 0xFF};
    int32_t input_x = 0;
    int32_t input_y = 0;
    int32_t input_w = 0;
    uint8_t finger = 0;

    Touch_event = 0;
    GTP_DEBUG_FUNC();
    GTP_IRQ_Disable();
    ret = GTP_I2C_Read(GTP_ADDRESS, point_data, 12); // 10字节寄存器加2字节地址
    if (ret < 0)
    {
      bsp_ERROR("I2C transfer error. errno:%d\n ", ret);
      return 0;
    }

    finger = point_data[GTP_ADDR_LENGTH]; //状态寄存器数据

    if (finger == 0x00) //没有数据，退出
    {
      return 0;
    }

    if ((finger & 0x80) == 0) //判断buffer status位
    {
      goto exit_work_func; //坐标未就绪，数据无效
    }

    touch_num = finger & 0x0f; //坐标点数
    if (touch_num > GTP_MAX_TOUCH)
    {
      goto exit_work_func; //大于最大支持点数，错误退出
    }

    if (touch_num)
    {
      //      id = point_data[0] & 0x0F;									//track id

      input_x = point_data[3 + 1] | (point_data[3 + 2] << 8); // x坐标
      input_y = point_data[3 + 3] | (point_data[3 + 4] << 8); // y坐标
      input_w = point_data[3 + 5] | (point_data[3 + 6] << 8); // size

      if (input_x < GTP_MAX_WIDTH && input_y < GTP_MAX_HEIGHT)
      {
        *x = input_x;
        *y = input_y;
        *w = input_w;
      }
      else
      {
        //超出范围，错误退出
        goto exit_work_func;
      }
    }
  }
exit_work_func:
{
  //清空标志
  ret = GTP_I2C_Write(GTP_ADDRESS, end_cmd, 3);
  if (ret < 0)
  {
    bsp_INFO("GTP write end_cmd error!");
    Touch_event = 0;
    return 0;
  }
}

  GTP_IRQ_Enable();
  return touch_num;
}
