/*! @file ili9341.c
 * @brief  TFT LCD RGB 240x320 ILI9341 driver
 *
 * This driver provide functions to configure and control a 240x320 pixels
 * TFT color display connected to the ESP-EDU.
 * It uses a SPI port and 3 GPIOs to communicate with the ILI9341 LCD driver chip.
 *
 * @author Albano Peñalva
 *
 * @note Hardware connections:
 *
 * |   	Display		|   EDU-CIAA	|
 * |:--------------:|:--------------|
 * | 	SDO/MISO 	|	SPI_MISO	|
 * | 	LED		 	| 	3V3			|
 * | 	SCK		 	| 	SPI_SCK		|
 * | 	SDI/MOSI 	| 	SPI_MOSI	|
 * | 	DC/RS	 	| 	GPIOx		|
 * | 	RESET	 	| 	GPIOx		|
 * | 	CS		 	| 	GPIOx		|
 * | 	GND		 	| 	GND			|
 * | 	VCC		 	| 	3V3			|
 *
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/11/2018 | Document creation		                         |
 *
 */

#include "ili9341.h"
#include "fonts.h"
#include "spi_mcu.h"
#include "gpio_mcu.h"
#include "delay_mcu.h"

/*****************************************************************************
 * Private macros/types/enumerations/variables definitions
 ****************************************************************************/

#define NULL 0

#define SPI_BR 20000000				/*!< Frequency of sck for SPI communication */
#define MAX_PIXEL 320*240*2			/*!< Maximum number of bytes to write on LCD */
#define MSK_BIT16 0x8000			/*!< 16th bit mask */
#define MAX_VALUE_SIZE 256			/*!< Maximum length of a data array to prevent excessive use of memory */
#define LEFT -1						/*!< Horizontal grow direction */
#define RIGHT 1						/*!< Horizontal grow direction */
#define DOWN 1						/*!< Vertical grow direction */
#define UP -1						/*!< Vertical grow direction */

/* Command List */
#define RESET				0x01 	/*!< Resets the commands and parameters to their S/W Reset default values */
#define SLEEP_IN			0x10 	/*!< Enter to the minimum power consumption mode */
#define SLEEP_OUT			0x11 	/*!< Turns off sleep mode */
#define DISPLAY_INV_OFF		0x20 	/*!< Recover from display inversion mode */
#define DISPLAY_INV_ON		0x21 	/*!< Invert every bit from the frame memory to the display */
#define GAMMA_SET			0x26 	/*!< Select the desired Gamma curve for the current display */
#define DISPLAY_OFF			0x28 	/*!< The output from Frame Memory is disabled and blank page inserted */
#define DISPLAY_ON			0x29 	/*!< Recover from DISPLAY OFF mode */
#define COLUMN_ADDR_SET		0x2A 	/*!< Define columns of frame memory where MCU can access */
#define PAGE_ADDR_SET		0x2B 	/*!< Define rows of frame memory where MCU can access */
#define MEM_WRITE			0x2C 	/*!< Transfer data from MCU to frame memory */
#define MEM_ACC_CTRL		0x36 	/*!< Defines read/write scanning direction of frame memory */
#define PIXEL_FORMAT_SET	0x3A 	/*!< Sets the pixel format for the RGB image data used by the interface */
#define WRITE_DISP_BRIGHT	0x51 	/*!< Adjust the brightness value of the display */
#define WRITE_CTRL_DISP		0x53 	/*!< Control display brightness */
#define RGB_INTERFACE		0xB0 	/*!< Sets the operation status of the display interface */
#define FRAME_CTRL			0xB1 	/*!< Sets the division ratio for internal clocks of Normal mode at MCU interface */
#define BLANK_PORCH_CTRL	0xB5 	/*!< Blanking porch control */
#define DISP_FUN_CTRL		0xB6 	/*!< Display function control */
#define PWR_CTRL1			0xC0 	/*!< Set the GVDD level, which is a reference level for the VCOM level and the grayscale voltage level */
#define PWR_CTRL2			0xC1 	/*!< Sets the factor used in the step-up circuits */
#define VCOM_CTRL1			0xC5 	/*!< Set the VCOMH voltage */
#define VCOM_CTRL2			0xC7 	/*!< Set the VCOMH voltage */
#define PWR_CTRL_A			0xCB 	/*!< Vcore control */
#define PWR_CTRL_B			0xCF 	/*!< Power control */
#define POS_GAMMA			0xE0 	/*!< Set the gray scale voltage to adjust the gamma characteristics of the TFT panel */
#define NEG_GAMMA			0xE1 	/*!< Set the gray scale voltage to adjust the gamma characteristics of the TFT panel */
#define DRIV_TIM_CTRL_A		0xE8	/*!< Timing control */
#define DRIV_TIM_CTRL_B		0xEA	/*!< Timing control */
#define PWR_ON_CTRL			0xED	/*!< Power on sequence control */
#define EN_3_GAMMA			0xF2	/*!< 3 gamma control enable */
#define PUMP_RATIO_CTRL		0xF7	/*!< Pump ratio control */

#define HighByte(x) x >> 8			/*!< High byte of a 16 bits data */
#define LowByte(x) x & 0xFF			/*!< Low byte of a 16 bits data */

/**
 * @brief  Structure with LCD orientation properties
 */
typedef struct {
	uint16_t width;						/*!< LCD width */
	uint16_t height;					/*!< LCD height */
	ili9341_orientation_t orientation;	/*!< LCD Orientation */
} orientation_properties_t;

/**
 * @brief Structure to configure or write LCD
 */
typedef struct {
    uint8_t cmd;			/*!< Command */
    uint32_t databytes; 	/*!< Number of bytes of data to transmit */
    uint8_t *data;			/*!< Pointer to data or parameters array */
} lcd_cmd_t;

/**
 * @brief Initial LCD configuration parameters
 */
uint8_t pwr_ctrl_a[] = {0x39, 0x2C, 0x00, 0x34, 0x02};		/*!< Default configuration after RST */
uint8_t pwr_ctrl_b[] = {0x00, 0xC1, 0x30};					/*!< Discharge path enable */
uint8_t driv_tim_ctrl_a[] = {0x85, 0x00, 0x78};				/*!< Default configuration after RST */
uint8_t driv_tim_ctrl_b[] = {0x00, 0x00};					/*!< Gate driver timing control: 0 unit */
uint8_t pwr_on_ctrl[] = {0x64, 0x03, 0x12, 0x81}; 			/*!< CP1 keeps 1 frame, 1st frame enable, vcl = 0, ddvdh = 3, vgh =1 , vgl = , DDVDH_ENH = 1 */
uint8_t pump_ratio_ctrl[] = {0x20};							/*!< DDVDH = 2xVCI */
uint8_t pwr_ctrl1[] = {0x23};								/*!< GVDD = 4.6V */
uint8_t pwr_ctrl2[] = {0x10};								/*!< Default configuration after RST */
uint8_t vcom_ctrl1[] = {0x3E, 0x28};						/*!< VCOMH = 4.25V, VCOML = -1.5V */
uint8_t vcom_ctrl2[] = {0x86};								/*!< VCOMH = VMH - 58, VCOML = VML - 58 */
uint8_t mem_acc_ctrl[] = {0x48};							/*!< MY = 0, MX = 1, MV = 0, ML = 0, BGR order, MH = 0 */
uint8_t pixel_format_set[] = {0x55};						/*!< 16 bits/pixel */
uint8_t frame_ctrl[] = {0x00, 0x18};						/*!< Frame Rate = 79Hz */
uint8_t disp_fun_ctrl[] = {0x0A, 0x82, 0x27};				/*!< Default configuration after RST */
uint8_t en_3_gamma[] = {0x02};								/*!< Default configuration after RST */
uint8_t column_addr_set[] = {0x00, 0x00, 0x00, 0xEF};		/*!< Start Column = 0, End Column = 239 */
uint8_t page_addr_set[] = {0x00, 0x00, 0x01, 0x3F};			/*!< Start Page = 0, End Page = 319 */
uint8_t gamma_set[] = {0x01};								/*!< Default configuration after RST */
uint8_t pos_gamma[] = {0x0F, 0X31, 0X2B, 0X0C, 0X0E, 0X08,
	0X4E, 0XF1, 0X37, 0X07, 0X10, 0X03, 0X0E, 0X09, 0X00};	/*!< Positive gamma correction */
uint8_t neg_gamma[] = {0x00, 0X0E, 0X14, 0X03, 0X11, 0X07,
	0X31, 0XC1, 0X48, 0X08, 0X0F, 0X0C, 0X31, 0X36, 0X0F};	/*!< Negative gamma correction */

/**
 * @brief Initial LCD configuration
 */
lcd_cmd_t lcd_init[] = {
	{PWR_CTRL_A, 5, pwr_ctrl_a},
	{PWR_CTRL_B, 3, pwr_ctrl_b},
	{DRIV_TIM_CTRL_A, 3, driv_tim_ctrl_a},
	{DRIV_TIM_CTRL_B, 2, driv_tim_ctrl_b},
	{PWR_ON_CTRL, 4, pwr_on_ctrl},
	{PUMP_RATIO_CTRL, 1, pump_ratio_ctrl},
	{PWR_CTRL1, 1, pwr_ctrl1},
	{PWR_CTRL2, 1, pwr_ctrl2},
	{VCOM_CTRL1, 2, vcom_ctrl1},
	{VCOM_CTRL2, 1, vcom_ctrl2},
	{MEM_ACC_CTRL, 1, mem_acc_ctrl},
	{PIXEL_FORMAT_SET, 1, pixel_format_set},
	{FRAME_CTRL, 2, frame_ctrl},
	{DISP_FUN_CTRL, 3, disp_fun_ctrl},
	{EN_3_GAMMA, 1, en_3_gamma},
	{COLUMN_ADDR_SET, 4, column_addr_set},
	{PAGE_ADDR_SET, 4, page_addr_set},
	{GAMMA_SET, 1, gamma_set},
	{POS_GAMMA, 15, pos_gamma},
	{NEG_GAMMA, 15, neg_gamma},
};

lcd_cmd_t lcd_reset = {RESET, NULL, NULL};			/*!< SW reset */
lcd_cmd_t lcd_sleep_out = {SLEEP_OUT, NULL, NULL};	/*!< Exit sleep mode */
lcd_cmd_t lcd_on = {DISPLAY_ON, NULL, NULL};		/*!< Exit sleep mode */

/*
 * @brief: SPI port configuration compatible with LCD interface
 */
spi_mcu_config_t spi_conf = {
	.device = NULL, 
	.clk_mode = MODE0, 
	.bitrate = SPI_BR, 
	.transfer_mode = SPI_POLLING, 
	.func_p = NULL,
	.param_p = NULL };

static spi_dev_t ili9341_spi;							/*!< uC SPI port */
static gpio_t ili9341_dc, ili9341_rst;		/*!< uC GPIO ports to use as CS, DC and RST */

static orientation_properties_t lcd_orientation = {
		ILI9341_WIDTH,
		ILI9341_HEIGHT,
		ILI9341_Portrait_1
};												/*!< Default orientation configuration */

/*****************************************************************************
 * Public types/enumerations/variables declarations
 ****************************************************************************/

/*****************************************************************************
 * Private functions definitions
 ****************************************************************************/
/**
 * @brief  		Send command and parameters/data to LCD
 * @param[in]  	data: Structure with the command and parameters/data to send
 * @retval 		None
 */
void WriteLCD(lcd_cmd_t * data);

/**
 * @brief  		Define an area of frame memory where MCU can access
 * @param[in]  	x1: Start column
 * @param[in]  	y1: Start row
 * @param[in]  	x2: End column
 * @param[in]  	y2: End row
 * @retval 		None
 */
void SetCursorPosition(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

/**
 * @brief  		Fill an srea of LCD with a determined color
 * @param[in]  	x1: Start column
 * @param[in]  	y1: Start row
 * @param[in]  	x2: End column
 * @param[in]  	y2: End row
 * @param[in]	color: color
 * @retval 		None
 */
void Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);

/*****************************************************************************
 * Private functions declarations
 ****************************************************************************/
void WriteLCD(lcd_cmd_t * data){
	SpiInit(&spi_conf);
	/* If command is NULL don't send command */
	if (data->cmd != NULL){
		/* Send command */
		GPIOOff(ili9341_dc);
		SpiWrite(ili9341_spi, &data->cmd, 1);
	}
	/* If there are parameters or data to send */
	if (data->databytes != NULL){
		/* Send parameters or data */
		GPIOOn(ili9341_dc);
		SpiWrite(ili9341_spi, data->data, data->databytes);
	}
}

void SetCursorPosition(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
	static uint16_t aux;
	/* The lower column must be send first */
	if (x0 > x1){
		aux = x0;
		x0 = x1;
		x1 = aux;
	}
	/* The lower row must be send first */
	if (y0 > y1){
		aux = y0;
		y0 = y1;
		y1 = aux;
	}
	uint8_t columns[] = {HighByte(x0), LowByte(x0), HighByte(x1), LowByte(x1)};
	lcd_cmd_t lcd_columns = {COLUMN_ADDR_SET, 4, columns};
	uint8_t rows[] = {HighByte(y0), LowByte(y0), HighByte(y1), LowByte(y1)};
	lcd_cmd_t lcd_rows = {PAGE_ADDR_SET, 4, rows};
	WriteLCD(&lcd_columns);
	WriteLCD(&lcd_rows);
}

void Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){
	static uint16_t i;
	static int32_t bytes_count;
	static int16_t x_dist, y_dist;
	static uint8_t pixel[MAX_VALUE_SIZE];

	x_dist = x1 - x0;
	y_dist = y1 - y0;
	if (x0 > x1){
		x_dist = - x_dist;
	}
	if (y0 > y1){
		y_dist = - y_dist;
	}
	/* Number of bytes to write. We have to write 2 bytes/pixel (16bits color) */
	bytes_count = (x_dist + 1) * (y_dist + 1) * 2;
	/* Define area to fill */
	SetCursorPosition(x0, y0, x1, y1);

	for (i = 0; i < MAX_VALUE_SIZE; i += 2){
		pixel[i] = HighByte(color);
		pixel[i + 1] = LowByte(color);
	}
	/* Start writing LCD memory */
	lcd_cmd_t lcd_write = {MEM_WRITE, NULL, NULL};
	WriteLCD(&lcd_write);

	while(bytes_count - MAX_VALUE_SIZE > 0){
		lcd_cmd_t lcd_pixel = {NULL, MAX_VALUE_SIZE, pixel};
		WriteLCD(&lcd_pixel);
		bytes_count -= MAX_VALUE_SIZE;
	}
	lcd_cmd_t lcd_pixel = {NULL, bytes_count, pixel};
	WriteLCD(&lcd_pixel);
}

/*****************************************************************************
 * Public functions declarations
 ****************************************************************************/

uint8_t ILI9341Init(spi_dev_t spi_dev, uint8_t gpio_dc, uint8_t gpio_rst){
	/* SPI configuration */
	spi_conf.device = spi_dev;
	ili9341_spi = spi_dev;
	/* GPIOs configuration and initialization */
	ili9341_dc = gpio_dc;
	ili9341_rst = gpio_rst;
	GPIOInit(ili9341_dc, GPIO_OUTPUT);
	GPIOInit(ili9341_rst, GPIO_OUTPUT);

	/* RST must be held low for minimum 10µsec after VCC have been applied */
	DelayUs(10);
	GPIOOn(ili9341_rst);
	/* Wait more than 10µsec after RST high before sending a command */
	DelayUs(10);
	/* It will be necessary to wait 5msec before sending new command following software reset */
	WriteLCD(&lcd_reset);
	DelayMs(5);
	/* Send initial configuration to LCD */
	for (uint8_t i = 0; i < sizeof(lcd_init)/sizeof(lcd_cmd_t); i++){
		WriteLCD(&lcd_init[i]);
	}
	/* It will be necessary to wait 5msec before sending next command after sleep out */
	WriteLCD(&lcd_sleep_out);
	DelayMs(10);
	WriteLCD(&lcd_on);
	DelayMs(20);
	/* Start screen on White */
	ILI9341Fill(ILI9341_WHITE);
	DelayMs(20);
	return true;
}

void ILI9341DrawPixel(uint16_t x, uint16_t y, uint16_t color){
	/* Define area (pixel) to fill */
	SetCursorPosition(x, y, x, y);
	uint8_t pixels[] = {HighByte(color), LowByte(color)};
	lcd_cmd_t lcd_pixels = {MEM_WRITE, sizeof(pixels), pixels};
	WriteLCD(&lcd_pixels);
}

void ILI9341Fill(uint16_t color){
	Fill(0, 0, lcd_orientation.width, lcd_orientation.height, color);
}

void ILI9341Rotate(ili9341_orientation_t orientation){
	uint8_t mem_acc[1];
	switch(orientation)	{
	case ILI9341_Portrait_1:
		mem_acc[0] = 0x48;		/*!< Row Address Order (MY) = 0, Column Address Order (MX) = 1, Row/Column Exchange (MV) = 0 */
		lcd_orientation.width = ILI9341_WIDTH;
		lcd_orientation.height = ILI9341_HEIGHT;
		lcd_orientation.orientation = ILI9341_Portrait_1;
		break;

	case ILI9341_Portrait_2:
		mem_acc[0] = 0x88;		/*!< Row Address Order (MY) = 1, Column Address Order (MX) = 1, Row/Column Exchange (MV) = 0 */
		lcd_orientation.width = ILI9341_WIDTH;
		lcd_orientation.height = ILI9341_HEIGHT;
		lcd_orientation.orientation = ILI9341_Portrait_2;
		break;

	case ILI9341_Landscape_1:
		mem_acc[0] = 0x28;		/*!< Row Address Order (MY) = 0, Column Address Order (MX) = 0, Row/Column Exchange (MV) = 1 */
		lcd_orientation.width = ILI9341_HEIGHT;
		lcd_orientation.height = ILI9341_WIDTH;
		lcd_orientation.orientation = ILI9341_Landscape_1;
		break;

	case ILI9341_Landscape_2:
		mem_acc[0] = 0xE8;		/*!< Row Address Order (MY) = 1, Column Address Order (MX) = 1, Row/Column Exchange (MV) = 1 */
		lcd_orientation.width = ILI9341_HEIGHT;
		lcd_orientation.height = ILI9341_WIDTH;
		lcd_orientation.orientation = ILI9341_Landscape_2;
		break;
	}
	lcd_cmd_t lcd_mem_acc = {MEM_ACC_CTRL, 1, mem_acc};
	WriteLCD(&lcd_mem_acc);
}

void ILI9341DrawChar(uint16_t x, uint16_t y, char data, Font_t* font, uint16_t foreground, uint16_t background){
	static uint16_t i, j, k;
	static uint16_t char_row;
	static uint16_t lcd_x, lcd_y;
	static int32_t bytes_count;
	static uint8_t pixel[MAX_VALUE_SIZE];

	/* Set coordinates */
	lcd_x = x;
	lcd_y = y;

	/* If at the end of a line of display, go to new line and set x to 0 position */
	if ((lcd_x + font->FontWidth) > lcd_orientation.width)	{
		lcd_y += font->FontHeight;
		lcd_x = 0;
	}

	SetCursorPosition(lcd_x, lcd_y, lcd_x + font->FontWidth - 1, lcd_y + font->FontHeight - 1);

	/* Number of bytes to write. We have to write 2 bytes/pixel */
	bytes_count = font->FontHeight * font->FontWidth * 2;

	/* Start writing LCD memory */
	lcd_cmd_t lcd_write = {MEM_WRITE, NULL, NULL};
	WriteLCD(&lcd_write);

	/* Draw font data */
	/* go through character rows */
	k = 0;
	for (i = 0; i < font->FontHeight; i++)	{
		/* each 16bits data of a font character draws a full row of that character */
		char_row = font->data[(data - ' ') * font->FontHeight + i];
		/* go through character columns */
		for (j = 0; j < font->FontWidth; j++)		{
			/* If exceed buffer size, send buffer */
			if ((2 * j + i * font->FontWidth * 2 - k * MAX_VALUE_SIZE + 1) > MAX_VALUE_SIZE)			{
				lcd_cmd_t lcd_pixels = {NULL, MAX_VALUE_SIZE, pixel};
				WriteLCD(&lcd_pixels);
				bytes_count -= MAX_VALUE_SIZE;
				k++;
			}
			/* The n=FontWidth first bits of the 16bits row data draws the corresponding part of a character */
			if (char_row & (MSK_BIT16 >> j)){
				/* if bit = 1, draw put foreground color */
				pixel[2 * j + i * font->FontWidth * 2 - k * MAX_VALUE_SIZE] = HighByte(foreground);
				pixel[2 * j + i * font->FontWidth * 2 - k * MAX_VALUE_SIZE + 1] = LowByte(foreground);
			}
			else{
				pixel[2 * j + i * font->FontWidth * 2 - k * MAX_VALUE_SIZE] = HighByte(background);
				pixel[2 * j + i * font->FontWidth * 2 - k * MAX_VALUE_SIZE + 1] = LowByte(background);
			}
		}
	}
	/* Send the rest of the buffer */
	lcd_cmd_t lcd_pixels = {NULL, bytes_count, pixel};
	WriteLCD(&lcd_pixels);
}

void ILI9341DrawInt(uint16_t x, uint16_t y, uint32_t num, uint8_t dig, Font_t* font, uint16_t foreground, uint16_t background){
	static uint16_t i;
	static uint16_t lcd_x, lcd_y;

	/* Set coordinates */
	lcd_x = x;
	lcd_y = y;

	for (i=0; i<dig; i++){
		lcd_x = x + font->FontWidth * (dig-1-i);
		ILI9341DrawChar(lcd_x, lcd_y, num%10 + '0', font, foreground, background);
		num = num/10;
	}
}

void ILI9341DrawString(uint16_t x, uint16_t y, char* str, Font_t *font, uint16_t foreground, uint16_t background){
	static uint16_t lcd_x, lcd_y;

	/* Set coordinates */
	lcd_x = x;
	lcd_y = y;

	while (*str != '\0'){	/* End of string */
		/* New line */
		if (*str == '\n'){
			lcd_y += font->FontHeight + 1;
			/* if after \n is also \r, than go to the left of the screen */
			if (*(str + 1) == '\r'){
				lcd_x = 0;
				str++;
			}
			else{
				lcd_x = x;
			}
			str++;
		}
		else if (*str == '\r'){
				str++;
			}

		/* Put character to LCD */
		ILI9341DrawChar(lcd_x, lcd_y, *str, font, foreground, background);
		/* Next character */
		str++;
		lcd_x += font->FontWidth;
	}
}

void ILI9341GetStringSize(char* str, Font_t* font, uint16_t* width, uint16_t* height){
	static uint16_t w;

	*height = font->FontHeight;
	w = 0;
	while (*str != '\0'){	/* End of string */
	
		w += font->FontWidth;
		str++;
	}
	*width = w;
}

void ILI9341DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){
	static int16_t x_dist, y_dist, x_grow, y_grow, error, error_2;

	/* Check for overflow */
	if (x0 >= lcd_orientation.width){
		x0 = lcd_orientation.width - 1;
	}
	if (x1 >= lcd_orientation.width){
		x1 = lcd_orientation.width - 1;
	}
	if (y0 >= lcd_orientation.height){
		y0 = lcd_orientation.height - 1;
	}
	if (y1 >= lcd_orientation.height){
		y1 = lcd_orientation.height - 1;
	}

	/* Calculate x y distances and determine grow direction */
	x_dist = x1 - x0;
	y_dist = y1 - y0;
	if (x0 > x1){
		x_dist = -x_dist;
		x_grow = LEFT;
	}
	else{
		x_grow = RIGHT;
	}
	if (y0 > y1){
		y_dist = -y_dist;
		y_grow = UP;
	}
	else{
		y_grow = DOWN;
	}

	/* Vertical or horizontal line */
	if (x_dist == 0 || y_dist == 0){
		Fill(x0, y0, x1, y1, color);
	}
	/* Diagonal line */
	else{
		if(x_dist > y_dist){
			error = x_dist / 2;
		}
		else{
			error = y_dist / 2;
		}

		while (1){
			/* Draw start point */
			ILI9341DrawPixel(x0, y0, color);
			/* Loop ends when start point reaches end point */
			if (x0 == x1 || y0 == y1){
				break;
			}
			error_2 = error;
			/* Determine if line must grow in x direction */
			if (error_2 > -x_dist){
				error -= y_dist;
				x0 += x_grow;	/* Move start point */
			}
			/* Determine if line must grow in y direction */
			if (error_2 < y_dist){
				error += x_dist;
				y0 += y_grow;	/* Move start point */
			}
		}
	}
}

void ILI9341DrawRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){
	ILI9341DrawLine(x0, y0, x1, y0, color);		/* Draw top line */
	ILI9341DrawLine(x1, y0, x1, y1, color);		/* Draw right line */
	ILI9341DrawLine(x0, y1, x1, y1, color);		/* Draw bottom line */
	ILI9341DrawLine(x0, y0, x0, y1, color);		/* Draw left line */
}

void ILI9341DrawFilledRectangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color){
	Fill(x0, y0, x1, y1, color);
}

void ILI9341DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){
	static int16_t f, ddF_x, ddF_y, x, y;

	f = 1 - r;
	ddF_x = 1;
	ddF_y = -2 * r;
	x = 0;
	y = r;

	ILI9341DrawPixel(x0, y0 + r, color);
	ILI9341DrawPixel(x0, y0 - r, color);
	ILI9341DrawPixel(x0 + r, y0, color);
	ILI9341DrawPixel(x0 - r, y0, color);

    while (x < y){
        if (f >= 0){
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9341DrawPixel(x0 + x, y0 + y, color);
        ILI9341DrawPixel(x0 - x, y0 + y, color);
        ILI9341DrawPixel(x0 + x, y0 - y, color);
        ILI9341DrawPixel(x0 - x, y0 - y, color);

        ILI9341DrawPixel(x0 + y, y0 + x, color);
        ILI9341DrawPixel(x0 - y, y0 + x, color);
        ILI9341DrawPixel(x0 + y, y0 - x, color);
        ILI9341DrawPixel(x0 - y, y0 - x, color);
    }
}

void ILI9341DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color){
	static int16_t f, ddF_x, ddF_y, x, y;

	f = 1 - r;
	ddF_x = 1;
	ddF_y = -2 * r;
	x = 0;
	y = r;

    ILI9341DrawPixel(x0, y0 + r, color);
    ILI9341DrawPixel(x0, y0 - r, color);
    ILI9341DrawPixel(x0 + r, y0, color);
    ILI9341DrawPixel(x0 - r, y0, color);
    ILI9341DrawLine(x0 - r, y0, x0 + r, y0, color);

    while (x < y){
        if (f >= 0){
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        ILI9341DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        ILI9341DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

        ILI9341DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
        ILI9341DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

void ILI9341DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
	ILI9341DrawLine(x0, y0, x1, y1, color);
	ILI9341DrawLine(x0, y0, x2, y2, color);
	ILI9341DrawLine(x1, y1, x2, y2, color);
}

void ILI9341DrawFilledTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color){
	static int16_t x_0 = 0;
	static int16_t y_0 = 0;
	static int16_t x_1 = 0;
	static int16_t y_1 = 0;
	static int16_t x_2 = 0;
	static int16_t y_2 = 0;
	static int16_t x_aux = 0;
	static int16_t y_aux = 0;
	static int16_t scanline_y = 0;
	float invslope1, invslope2, curx1, curx2;
	if((y0 <= y1) && (y0 <= y2)){
		x_0 = x0;
		y_0 = y0;
		if(y1 <= y2){
			x_1 = x1;
			y_1 = y1;
			x_2 = x2;
			y_2 = y2;
		} else{
			x_1 = x2;
			y_1 = y2;
			x_2 = x1;
			y_2 = y1;
		}
	}else if((y1 <= y2) && (y1 <= y0)){
		x_0 = x1;
		y_0 = y1;
		if(y0 <= y2){
			x_1 = x0;
			y_1 = y0;
			x_2 = x2;
			y_2 = y2;
		} else{
			x_1 = x2;
			y_1 = y2;
			x_2 = x0;
			y_2 = y0;
		}
	}else if((y2 <= y1) && (y2 <= y0)){
		x_0 = x2;
		y_0 = y2;
		if(y0 <= y1){
			x_1 = x0;
			y_1 = y0;
			x_2 = x1;
			y_2 = y1;
		} else{
			x_1 = x1;
			y_1 = y1;
			x_2 = x0;
			y_2 = y0;
		}
	}
	if(y_1 == y_2){
		// Bottom flat triangle
		invslope1 = (float)(x_1 - x_0) / (float)(y_1 - y_0);
		invslope2 = (float)(x_2 - x_0) / (float)(y_2 - y_0);
		curx1 = x_0;
		curx2 = x_0;
		scanline_y = y_0;
		while(scanline_y < y_1){
			ILI9341DrawLine((int)curx1, scanline_y, (int)curx2, scanline_y, color);
			curx1 += invslope1;
			curx2 += invslope2;
			scanline_y++;
		}
  	}
  	else if (y_0 == y_1){
		// Top flat triangle
		invslope1 = (float)(x_2 - x_0) / (float)(y_2 - y_0);
		invslope2 = (float)(x_2 - x_1) / (float)(y_2 - y_1);
		curx1 = x_2;
		curx2 = x_2;
		scanline_y = y_2;
		while(scanline_y > y_0){
			ILI9341DrawLine((int)curx1, scanline_y, (int)curx2, scanline_y, color);
			curx1 -= invslope1;
			curx2 -= invslope2;
			scanline_y--;
		}
  	}
  	else{
		// Split in a flat top triangle and a bottom flat triangle
		x_aux = (int)(x_0 + (float)(y_1 - y_0) / (float)(y_2 - y_0) * (x_2 - x_0));
		y_aux = y_1;
		// Bottom flat triangle
		invslope1 = (float)(x_1 - x_0) / (float)(y_1 - y_0);
		invslope2 = (float)(x_aux - x_0) / (float)(y_aux - y_0);
		curx1 = x_0;
		curx2 = x_0;
		scanline_y = y_0;
		while(scanline_y < y_1){
			ILI9341DrawLine((int)curx1, scanline_y, (int)curx2, scanline_y, color);
			curx1 += invslope1;
			curx2 += invslope2;
			scanline_y++;
		}
		// Top flat triangle
		invslope1 = (float)(x_2 - x_1) / (float)(y_2 - y_1);
		invslope2 = (float)(x_2 - x_aux) / (float)(y_2 - y_aux);
		curx1 = x_2;
		curx2 = x_2;
		scanline_y = y_2;
		while(scanline_y > y_1){
			ILI9341DrawLine((int)curx1, scanline_y, (int)curx2, scanline_y, color);
			curx1 -= invslope1;
			curx2 -= invslope2;
			scanline_y--;
		}
		ILI9341DrawLine(x_1, y_1, x_aux, y_aux, color);
  	}
}

void ILI9341DrawPicture(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t* pic){
	static uint16_t i, j;
	static int32_t bytes_count;
	static uint8_t pixel[MAX_VALUE_SIZE];

	SetCursorPosition(x, y, x + width - 1, y + height - 1);

	/* Number of bytes to write. We have to write 2 bytes/pixel */
	bytes_count = width * height * 2;

	/* Start writing LCD memory */
	lcd_cmd_t lcd_write = {MEM_WRITE, NULL, NULL};
	WriteLCD(&lcd_write);

	j = 0;
	while(bytes_count - MAX_VALUE_SIZE > 0){
		for (i = 0; i < MAX_VALUE_SIZE; i++){
			pixel[i] = pic[j * MAX_VALUE_SIZE + i];
		}
		lcd_cmd_t lcd_pixel = {NULL, MAX_VALUE_SIZE, pixel};
		WriteLCD(&lcd_pixel);
		bytes_count -= MAX_VALUE_SIZE;
		j++;
	}
	for (i = 0; i < bytes_count; i++){
		pixel[i] = pic[j * MAX_VALUE_SIZE + i];
	}
	lcd_cmd_t lcd_pixel = {NULL, bytes_count, pixel};
	WriteLCD(&lcd_pixel);
}
