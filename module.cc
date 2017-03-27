// By F6CF

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <bcm2835.h>
#include <time.h>
#include <nan.h>
#include "lcm_table.h"

#define LCD_CS 1
#define TOUCH_CS 0

uint8_t lcd_rotations[4] = {
	0b11101010, //   0
	0b01001010, //  90
	0b00101010, // 180
	0b10001010 // 270
};

volatile uint8_t color;
volatile uint8_t lcd_rotation;
volatile uint16_t lcd_h;
volatile uint16_t lcd_w;

void spiTransmit(int devsel, uint8_t* data, int len) {
	if (devsel == 0)
		bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	else
		bcm2835_spi_chipSelect(BCM2835_SPI_CS1);
	bcm2835_spi_transfern((char*)data, len);
}

void dsi_set_cmdq_V2(unsigned cmd, unsigned char count, unsigned char* para_list, unsigned char force_update) {
	// CMD
	uint8_t b1[2];
	b1[0] = cmd >> 1;
	b1[1] = ((cmd & 1) << 5) | 0x11;
	spiTransmit(LCD_CS, &b1[0], sizeof(b1));
	b1[0] = cmd >> 1;
	b1[1] = ((cmd & 1) << 5) | 0x1B;
	spiTransmit(LCD_CS, &b1[0], sizeof(b1));
	// DATA
	for (int i = 0; i < count; i++) {
		uint8_t dat = para_list[i];
		b1[0] = dat >> 1;
		b1[1] = ((dat & 1) << 5) | 0x15;
		spiTransmit(LCD_CS, &b1[0], sizeof(b1));
		b1[0] = dat >> 1;
		b1[1] = ((dat & 1) << 5) | 0x1F;
		spiTransmit(LCD_CS, &b1[0], sizeof(b1));
	}
}
static void push_table(struct LCM_setting_table* table, unsigned int count, unsigned char force_update) {
	unsigned int i;
	for (i = 0; i < count; i++) {
		unsigned cmd;
		cmd = table[i].cmd;
		switch (cmd) {
		case REGFLAG_DELAY:
			delay(table[i].count);
			break;
		case REGFLAG_END_OF_TABLE:
			break;
		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}
}

uint8_t lowLevelOpen(void) {
	int r;
	r = bcm2835_init();
	if (r != 1)
		return -1;
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);
	return 0;
}
uint8_t lowLevelClose(void) {
	bcm2835_spi_end();
	int r = bcm2835_close();
	if (r != 1)
		return -1;
	return 0;
}

void lowLevelInit(void) {
#if 1 // Yaay, china govnocode!
	uint8_t b1[1];
	b1[0] = 0x00;
	spiTransmit(LCD_CS, &b1[0], sizeof(b1));
	delay(150);
	b1[0] = 0x01;
	spiTransmit(LCD_CS, &b1[0], sizeof(b1));
	delay(250);
#endif
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 0);
	lcd_h = LCD_HEIGHT;
	lcd_w = LCD_WIDTH;
}
void lowLevelColor(uint32_t col) {
	uint8_t b1[3];
	uint8_t pseud = ((col >> 5) & 0x40) | ((col << 5) & 0x20);
	b1[0] = col >> 8;
	b1[1] = col & 0x00FF;
	b1[2] = pseud | 0x15;
	spiTransmit(LCD_CS, &b1[0], sizeof(b1));
	b1[0] = col >> 8;
	b1[1] = col & 0x00FF;
	b1[2] = pseud | 0x1F;
	spiTransmit(LCD_CS, &b1[0], sizeof(b1));
}
void lowLevelRotate(uint8_t m) {
	unsigned char para_list[64] = { lcd_rotations[m] };
	dsi_set_cmdq_V2(0x36, 1, para_list, 0);
	if (m & 1) {
		lcd_h = LCD_WIDTH;
		lcd_w = LCD_HEIGHT;
	}
	else {
		lcd_h = LCD_HEIGHT;
		lcd_w = LCD_WIDTH;
	}
}
void lowLevelSetFrame(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
	unsigned char para_list1[64] = { x >> 8, x & 0xff, ((w + x) - 1) >> 8, ((w + x) - 1) & 0xFF };
	dsi_set_cmdq_V2(0x2a, 4, para_list1, 0);
	unsigned char para_list2[64] = { y >> 8, y & 0xFF, ((h + y) - 1) >> 8, ((h + y) - 1) & 0xFF };
	dsi_set_cmdq_V2(0x2b, 4, para_list2, 0);
	unsigned char para_list3[64] = {};
	dsi_set_cmdq_V2(0x2c, 0, para_list3, 0);
}
void lowLevelFill(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t col) {
	lowLevelSetFrame(x, y, w, h);
	for (int i = 0; i < h * w; i++)
		lowLevelColor(col);
}

void open(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	v8::Local<v8::Number> num = Nan::New(lowLevelOpen());
	info.GetReturnValue().Set(num);
}
void init(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	lowLevelInit();
	info.GetReturnValue().Set(Nan::Undefined());
}
void close(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	v8::Local<v8::Number> num = Nan::New(lowLevelClose());
	info.GetReturnValue().Set(num);
}
void setRotation(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	uint8_t rotation = info[0]->NumberValue();
	lowLevelRotate(rotation);
	info.GetReturnValue().Set(Nan::Undefined());
}
void fill(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	uint16_t fx = 0;
	uint16_t tx = lcd_w;
	uint16_t fy = 0;
	uint16_t ty = lcd_h;
	uint32_t color = 0xff00ff; // Purple
	if (info.Length() == 5) {
		fx = info[0]->NumberValue();
		fy = info[1]->NumberValue();
		tx = info[2]->NumberValue();
		ty = info[3]->NumberValue();
		color = info[4]->NumberValue();
	}
	else {
		color = info[0]->NumberValue();
	}
	lowLevelFill(fx, fy, tx, ty, color);
	info.GetReturnValue().Set(Nan::Undefined());
}
void getSize(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	v8::Local<v8::Number> num = Nan::New(lcd_w * 1000000 + lcd_h);
	info.GetReturnValue().Set(num);
}

void Init(v8::Local<v8::Object> exports) {
	exports->Set(Nan::New("open").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(open)->GetFunction());
	exports->Set(Nan::New("init").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(init)->GetFunction());
	exports->Set(Nan::New("close").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(close)->GetFunction());
	exports->Set(Nan::New("setRotation").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(setRotation)->GetFunction());
	exports->Set(Nan::New("fill").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(fill)->GetFunction());
	exports->Set(Nan::New("getSize").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(getSize)->GetFunction());
}

NODE_MODULE(module, Init);