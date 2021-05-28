///////////////////////////////////////////////////////////////////////////////////////////////////
/* Video/Display Driver for Kernel Debugging
	k_video.c
	This file does:
	1. very basic video driver for CGA text mode 3.
	2. initialises video through video memory.
	3. used by kernel.

	This work is licensed under the Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
	International License. To view a copy of this license, visit
	http://creativecommons.org/licenses/by-nc-nd/4.0/
	or send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

	Also see http://www.bluekernel.com.au

	copyright Paul Cuttler 2017
 */
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../policy/global.h"
#include "boot/bootdata.h"

#if VIDEO_DEBUG

// CGA Video Memory
#define VIDEO_BASE				(KERNEL_LOWMEM + 0xB8000)
#define VIDEO_ROWS				25
#define VIDEO_COLUMNS			80

#define EXC_LEFT				(41 * 2)
#define EXC_TOP					2
#define EXC_BOTTOM				15
#define NUM_EXC_STRINGS			19

// BGA Video Memory and Settings.
#define VBE_DISPI_IOPORT_INDEX	0x01CE
#define VBE_DISPI_IOPORT_DATA	0x01CF
#define BGA_INDEX				0x01CE
#define BGA_DATA				0X01CF

#define BGA_v0					0xB0C0
// setting X and Y resolution and bit depth (8 BPP only), banked mode

#define BGA_v1					0xB0C1
// virtual width and height, X and Y offset

#define BGA_v2					0xB0C2
// 15, 16, 24 and 32 BPP modes, support for linear frame buffer, support for retaining memory
//contents on mode switching

#define BGA_v3					0xB0C3
// support for getting capabilities, support for using 8 bit DAC

#define BGA_v4					0xB0C4
// VRAM increased to 8 MB

#define BGA_v5					0xB0C5
// VRAM increased to 16 MB?

#define BGA_REG_ID				0
#define BGA_REG_X_RES			1
#define BGA_REG_Y_RES			2
#define BGA_REG_BIT_DEPTH		3
#define BGA_REG_ENABLE			4
#define BGA_REG_BANK			5
#define BGA_REG_VIRT_WIDTH		6
#define BGA_REG_VIRT_HEIGHT		7
#define BGA_REG_X_OFFSET		8
#define BGA_REG_Y_OFFSET		9

///// Kernel Video Interfaces /////////////////////////////////////////////////////////////////////

int			VideoInit(void);
int			VideoChangeMode(void);
void		BgaWriteRegister(uint16_t index, uint16_t value);
uint16_t	BgaReadRegister(uint16_t index);
int			BgaIsAvailable(void);

void		VideoProcessUpdate(void);
void		VideoExceptionOut(uint32_t exc_msg);
void		VideoAddPage(int32_t curr_pID, pID_t pID, uint32_t virtual_address, uint32_t getphys);

///// Kernel Video Local Functions ////////////////////////////////////////////////////////////////

int		VideoProcessOut(struct process_table_t* pID_ptr);
int		VideoCharOut(unsigned char ch);
int		VideoStrOut(const char *s);
int		VideoNumOut(uint32_t num, int width);
int		VideoStrNumOut(const char *s, uint32_t num, int width);

///// External Interfaces /////////////////////////////////////////////////////////////////////////

extern uint16_t In16(uint32_t port);
extern uint32_t Out16(uint32_t port, uint16_t data);

///// Local Data //////////////////////////////////////////////////////////////////////////////////

const char *exception_strings[NUM_EXC_STRINGS] = {
	"divide by 0", "single step", "NmiExc", "breakpoint", "OverflowExc", "bounds check",
	"invalid opcode", "coprocessor exception", "double fault", "coprocessor overrun","invalid TSS",
	"segment not present", "stack exception", "gen prot", "page", "coprocessor error", "exc16",
	"exc17", "exc18"};

char*		video				= (char*)(VIDEO_BASE);
char*		video_base			= (char*)(VIDEO_BASE);
int			video_limit			= VIDEO_COLUMNS * VIDEO_ROWS * 2; //columns * rows * char/attr pair.
int			x					= 0;
int			video_mode			= V_NORMAL;
char*		v_exc;
uint32_t	vx, vy;
int			lead_zero_enable	= 0;

///// External Data ///////////////////////////////////////////////////////////////////////////////

extern uint32_t num_processes;

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Kernel Video Interfaces /////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int VideoInit(void)
{
	v_exc		= video + EXC_LEFT;
	vx			= EXC_LEFT;
	vy			= EXC_TOP;
	video		= video_base;
	video_mode	= V_NORMAL;
	VideoStrOut("pID state pri T reason base timer size\n");
	VideoStrOut(" num sectors transferred: ");
	VideoNumOut(*sec_transferred, 4);
	
	return OK;
}

int VideoChangeMode(void)
{
	// Change to Bochs' BGA mode.

	// Confirm that the hardware allows BGA modes.
	if (!BgaIsAvailable)
		return -1;
	
	// Disable VBE.
	BgaWriteRegister(BGA_REG_ENABLE, 0);

	// Set the X Resolution
	BgaWriteRegister(BGA_REG_X_RES, 1920);

	// Set the Y Resolution.
	BgaWriteRegister(BGA_REG_Y_RES, 1080);

	// Set the bit depth
	BgaWriteRegister(BGA_REG_BIT_DEPTH, 32);
	
	// Enable VBE again.
	BgaWriteRegister(BGA_REG_ENABLE, 1);
}

void BgaWriteRegister(uint16_t index, uint16_t value)
{
	Out16(BGA_INDEX, index);
	Out16(BGA_DATA, value);
}

uint16_t BgaReadRegister(uint16_t index)
{
	Out16(BGA_INDEX, index);
	
	return In16(BGA_DATA);
}
 
int BgaIsAvailable(void)
{
	uint16_t bgaAvailable = BgaReadRegister(BGA_REG_ID);
	
	if (bgaAvailable < BGA_v2 || bgaAvailable > BGA_v5)
		return 1;
	else
		return 0;
}

void VideoProcessUpdate(void)
{
	int i;
	video				= (char*)(video_base + 80);
	lead_zero_enable	= 1;
	
	VideoStrNumOut("time:", clock_seconds, 4);
	VideoStrNumOut(":", clock_ticks, 3);
	VideoStrNumOut(" pID:", current_pID >> VERSION_SHIFT, 3);
	VideoStrOut(" *=coordinator");
	
	video=(char*)(video_base + VIDEO_COLUMNS*3);
	
	#if KERNEL_DEBUG
	VideoStrNumOut("exc's: ", exception_count, 2);
	VideoStrNumOut(" int's: ", interrupt_count, 4);
	#endif

	VideoStrNumOut(" timers: ", num_sleepers, 2);
	
	lead_zero_enable	= 0;
	VideoStrNumOut(" ", memory_size / MB, 2);
	VideoStrOut("MB");
	
	lead_zero_enable	= 1;
	lead_zero_enable	= 0;
	
	struct process_table_t* pID_ptr = first_process_ptr;
	i=0;
	while (i<11 /* pID_ptr < last_process_ptr */)
	{
		VideoProcessOut(pID_ptr);
		pID_ptr++;
		i++;
	}
}
void VideoExceptionOut(uint32_t exc_msg)
{
	uint32_t	*stack_addr;
	int			show_esp_ss	= 1;
	video_mode				= V_EXCEPTION;
	stack_addr				= (uint32_t*)(exc_msg);
	uint32_t	exc_num		= stack_addr[EXC_NO];
	
	VideoStrOut("##exc:");

	lead_zero_enable	= 0;
	VideoNumOut(exc_num, 8);
	
	lead_zero_enable	= 1;
	VideoStrOut(exception_strings[exc_num]);
	VideoStrNumOut(",esp:", (uint32_t)stack_addr, 8);

	lead_zero_enable	= 1;
	VideoStrOut("pID:");

	lead_zero_enable	= 0;
	VideoNumOut(current_pID >> VERSION_SHIFT, 8);

	lead_zero_enable	= 1;
	VideoStrNumOut("CR2:",		stack_addr[CR2], 8);
	VideoStrNumOut("Err Code:",	stack_addr[ERROR_CODE], 8);
	VideoStrNumOut("EIP:",		stack_addr[EIP], 8);
	
	if (*stack_addr == CSk_SEL)
		show_esp_ss = 0;
	
	VideoStrNumOut("CS:",		stack_addr[CS], 4);
	VideoStrNumOut("EFLAGS:",	stack_addr[EFLAGS], 8);
	
	if (show_esp_ss)
	{
		VideoStrNumOut("ESP:",	stack_addr[ESP], 8);
		VideoStrNumOut("SS:",	stack_addr[SS], 4);
	}
	VideoStrOut("........\n");
	
	video_mode			= V_NORMAL;
	lead_zero_enable	= 0;
}

void VideoAddPage(int32_t curr_pID, int32_t pID, uint32_t virtual_address, uint32_t getphys)
{
	video_mode = V_EXCEPTION;
	VideoStrOut("Add Page: ");
	lead_zero_enable=1;
	VideoNumOut(curr_pID,2);
	VideoNumOut(pID >> VERSION_SHIFT,2);
	VideoNumOut(virtual_address,8);
	VideoNumOut(getphys,8);
	VideoStrOut("\n");
	video_mode = V_NORMAL;
	lead_zero_enable = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///// Kernel Video Local Functions ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

int VideoProcessOut(struct process_table_t* pID_ptr)
{
	int process_index = pID_ptr->pID >> VERSION_SHIFT;
	video=(char*)video_base+160*(process_index+2);
	VideoNumOut(process_index,8);
	VideoStrOut(" ");
	lead_zero_enable=1;
	VideoNumOut(pID_ptr->pID,1);
	lead_zero_enable=0;
	//VideoStrOut(" ");
	if ((pID_ptr->privilege & COORD_PRIVILEGE) == COORD_PRIVILEGE) VideoStrOut("*");
	else VideoStrOut(" ");
	VideoNumOut(pID_ptr->status,8);
	VideoStrOut(" ");
	lead_zero_enable=1;
	VideoNumOut(pID_ptr->times_scheduled,2);
	VideoStrOut("/");
	VideoNumOut(pID_ptr->priority,1);
	VideoStrOut(" ");
	uint32_t run_sec = pID_ptr->time_running/TICK_FREQ;
	uint32_t run_min = run_sec/60;
	run_sec = run_sec%60;
	VideoNumOut(run_min,2);
	VideoCharOut(':');
	VideoNumOut(run_sec,2);
	VideoCharOut(':');
	uint16_t run_tick = pID_ptr->time_running - (run_sec*TICK_FREQ);
	VideoNumOut(run_tick,3);
	VideoStrOut(" ");
	VideoNumOut(pID_ptr->reason,2);
	VideoStrOut(" ");
	VideoNumOut(pID_ptr->base_page,3);
	VideoStrOut(" ");
	VideoNumOut(pID_ptr->mechanisms[0],3);
	VideoStrOut(" ");
	VideoNumOut(pID_ptr->timer,4);
	VideoStrOut(" ");
	VideoNumOut(pID_ptr->size/0x400,2);
	lead_zero_enable=0;
	return 1;
}

int VideoCharOut(unsigned char ch)
// output a single character at the current cursor
{
	int i;
	if (video >= (video_base + video_limit) || video < video_base )
	/* If current cursor is outside the video's range, output an error.*/
	{
		video = video_base;
		for (i=0;i<80;i++)
		{
			*video = 'A'+x;
			video++;
			*video = 0x03;
			video++;
		}
	x++;
	if (x==26) x=0;
	}
	*video = ch;
	video++;
	*video = 0x07;
	video++;
	return 1;
}

int VideoStrNumOut(const char *s, uint32_t num, int width)
{
	VideoStrOut(s);
	VideoNumOut(num, width);
	
	return 1;
}

int VideoStrOut(const char *s)
{
	unsigned int v,x,y;
	char *video_temp;
	if (video_mode==V_NORMAL)
	{
		while (*s!=0)
		{
			if (*s==10)
			{
				v = (video-video_base);
				x = v/160;
				y = (x+1)*160;
				video = (char*)(y+video_base);
				if (video >= video_base + video_limit || video < video_base)
						video = video_base;
			}
			else VideoCharOut(*s);
			s++;
		}
	}
	else // V_EXCEPTION
	{
		video_temp = video;
		while (*s!=0)
		{
			video = video_base + vx + vy*160;
			vx += 2;
			if (video >= video_base + video_limit) video = video_base;
			if (*s != 10) VideoCharOut(*s);
			else {vy++;vx=EXC_LEFT;}
			s++;
			if (vx==160)
			{
				vx=EXC_LEFT; vy++;
				if (vy==EXC_BOTTOM) vy=EXC_TOP;
			}
		}
		video = video_temp;
	}
	return 0;
}

int VideoNumOut(uint32_t num, int width)
{
	char c[8];
	int i, temp;
	char *video_temp;

	int lead_zero	= 0;
	int num_t		= num;

	if (video > video_base + video_limit)
		i = 4;

	for(i = 0; i < 8; i++)
	{
		temp = (num % 16);
		
		if (temp > 9)
			c[i] = temp - 10 + 'a';
		else
			c[i] = temp + '0';
		
		num = num / 16;
	}
	if (video_mode == V_NORMAL)
	{
		{
			for (i = width - 1; i >= 0; i--)
			{
		 		if (video >= video_base + video_limit)
					 	video = video_base;
				
				if (lead_zero_enable==1 || lead_zero==1 || c[i]!='0')
				{
					*video = c[i];
					video++;
					*video=0x7;
					video++;
					lead_zero=1;
				}
			}
			if (num_t == 0 && lead_zero_enable == 0)
			{
		 		if (video >= video_base + video_limit)
					video = video_base;
				
				*video	= '0';
				video++;
				*video	= 0x7;
				video++;
			}
		}
	}
	else //V_EXCEPTION
	{
		video_temp = video;
		{
			for(i = width - 1; i >= 0; i--)
			{
				video = video_base + vx + vy*160;
				
				if (video >= video_base + video_limit)
					video = video_base;
				
				if (lead_zero_enable == 1 || lead_zero == 1 || c[i] != '0')
				{
					VideoCharOut(c[i]);
					lead_zero	= 1;
					vx			+= 2;
					if (vx == 160)
					{
						vx = EXC_LEFT;
						vy++;
						
						if (vy == EXC_BOTTOM)
							vy = EXC_TOP;
					}
				}
			}
		}
		VideoCharOut(' ');
		vx += 2;
		if (vx == 160)
		{
			vx = EXC_LEFT;
			vy++;
			
			if (vy == EXC_BOTTOM)
				vy = EXC_TOP;
		}
		video = video_temp;
	}
	return 1;
}

void VideoTestGraphics()
{
	unsigned *g;
	unsigned i = 0x4466;
	while (1)
	{
		i++;
		if (i > 0xFFFFFFFE)
			i = 0;
		g = (unsigned*)(KERNEL_LOWMEM + 0xa0000);
	
		int x = 0;
		while (g < (unsigned*)(KERNEL_LOWMEM + 0xc0000))
		{
			*g = i;
			g++;
			x++;
			if (x > 0x1000)
			{
				x = 0;
			}
			i += 0x10;
			if (i > 0xFFFFFFFE)
				i = 0x35;
		}
	}
}

#endif // VIDEO_DEBUG

/* END OF FILE */
