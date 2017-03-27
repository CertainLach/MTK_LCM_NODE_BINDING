#define REGFLAG_DELAY 0xfe
#define REGFLAG_END_OF_TABLE 0xff

#define LCD_WIDTH  480
#define LCD_HEIGHT 320

struct LCM_setting_table {
	unsigned char cmd;
	unsigned char count;
	unsigned char para_list[64];
};
	
// 	{cmd, count, {params}}
// 	{REGFLAG_DELAY, ms, {}},
// 	{REGFLAG_END_OF_TABLE, 0x00, {}}

static struct LCM_setting_table lcm_initialization_setting[] = {
	{0x00,0,{}},
	{0x11,0,{}}, // Sleep out
	{REGFLAG_DELAY,200,{}},
	
	{0xee,4,{0x02,0x01,0x02,0x01}},
	{0xed,15,{0x00,0x00,0x9a,0x9a,0x9b,0x9b,0x00,0x00,0x00,0x00,0xae,0xae,0x01,0xa2,0x00}},
	{0xb4,1,{0x00}},
	
	{0xc0,5,{0x10,0x3b,0x00,0x02,0x11}},
	{0xc1,1,{0x10}},
	{0xc8,12,{0x00,0x46,0x12,0x20,0x0c,0x00,0x56,0x12,0x67,0x02,0x00,0x0c}},
	
	{0xd0,3,{0x44,0x42,0x06}},
	{0xd1,2,{0x43,0x16}},
	{0xd2,2,{0x04,0x22}},
	{0xd3,2,{0x04,0x12}},
	{0xd4,2,{0x07,0x12}},
	
	{0xe9,1,{0x00}},
	{0xc5,1,{0x08}},
	
	{0x36,1,{0x2a}}, // Same as setRotation(0);
	{0x3a,1,{0x66}}, // RGB666 18bit

#if 1 // Comment if screen has noize
	{0x2a,4,{0x00,0x00,0x01,0x3f}},
	{0x2b,4,{0x00,0x00,0x01,0xe0}},
#endif
	
	{0x35,1,{0x00}},
	{0x29,0,{}}, // Display on
	{REGFLAG_DELAY,200,{}},
	{0x00,0,{}}, // 0x00 = NOP
	{0x11,0,{}}, // Sleep out
	{REGFLAG_DELAY,200,{}},
	
	{0xee,4,{0x02,0x01,0x02,0x01}},
	{0xed,17,{0x00,0x00,0x9a,0x9a,0x9b,0x9b,0x00,0x00,0x00,0x00,0xae,0xaf,0x01,0xa2,0x01,0xbf,0x2a}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    {0x11, 0, {}},
    {REGFLAG_DELAY, 120, {}},
    {0x29, 0, {}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
    {0x28, 0, {}},
    {REGFLAG_DELAY, 20, {}},
    {0x10, 0, {}},
    {REGFLAG_DELAY, 120, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};