/*
 * Copyright (c) 2018 Guillem96
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "gfx/di.h"
#include "gfx/gfx.h"
#include "soc/hw_init.h"
#include "mem/heap.h"


#include "core/launcher.h"

#include "utils/util.h"
#include "utils/fs_utils.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "utils/dirlist.h"
#include "menu/gui/gui_argon_menu.h"

#include "minerva/minerva.h"

#define PATH_ENABLE_TOUCH "StarDust/touch"

extern void pivot_stack(u32 stack_top);

static inline void setup_gfx()
{
    u32 *fb = display_init_framebuffer();
    gfx_init_ctxt(&g_gfx_ctxt, fb, 1280, 720, 720);
    gfx_clear_buffer(&g_gfx_ctxt);
    gfx_con_init(&g_gfx_con, &g_gfx_ctxt);
    gfx_con_setcol(&g_gfx_con, 0xFFCCCCCC, 1, BLACK);
}

void filelist(char* directory, char* filet)
{
			gfx_printf(&g_gfx_con, "\n");	
			gfx_printf(&g_gfx_con, "SD card\n");
			
char* files = listfil(directory, filet, true);
char* folder = listfol(directory, filet, true);
    u32 i = 0;
    u32 j = 0;
    while(folder[i * 256])
    {
			if(strlen(&folder[i * 256]) <= 100){			
			gfx_printf(&g_gfx_con, "\n");	
			gfx_printf(&g_gfx_con, &folder[i * 256]);
			}
	i++;
	}
    gfx_con_setcol(&g_gfx_con, 0xFF008F39, 0xFF726F68, 0xFF191414);

    while(files[j * 256])
    {
			if(strlen(&files[j * 256]) <= 100){			
			gfx_printf(&g_gfx_con, "\n");	
			gfx_printf(&g_gfx_con, &files[j * 256]);		
			}
	j++;
    }
    gfx_con_setcol(&g_gfx_con, 0xFFF9F9F9, 0, 0xFF191414);
	
			gfx_swap_buffer(&g_gfx_ctxt);
			msleep(20000);
}

//first Delete After Update
void clean_up()
{
	f_unlink("/fixer.del");

}


void ipl_main()
{
    /* Configure Switch Hardware (thanks to hekate project) */
    config_hw();

    /* Init the stack and the heap */
    pivot_stack(0x90010000);
    heap_init(0x90020000);

    /* Init display and gfx module */
    display_init();
    setup_gfx();
    display_backlight_pwm_init();
    display_backlight_brightness(20, 1000);


    /* Train DRAM */
    g_gfx_con.mute = 1; /* Silence minerva, comment for debug */
    minerva();
    g_gfx_con.mute = 0;

    /* Cofigure touch input */
    touch_power_on();
    
    /* Mount Sd card and launch payload */
    if (sd_mount())
    {
        bool cancel_auto_chainloading = btn_read() & BTN_VOL_DOWN;
        bool load_menu = cancel_auto_chainloading || launch_payload("StarDust/payload.bin");
		
filelist("", "*");
		
        gfx_printf(&g_gfx_con, "Autochainload canceled. Loading menu...\n");
        gfx_swap_buffer(&g_gfx_ctxt);
//		msleep(25000);
        if (load_menu)
            gui_init_argon_menu();

    } else {
        gfx_printf(&g_gfx_con, "No sd card found...\n");
    }

    /* If payload launch fails wait for user input to reboot the switch */
    gfx_printf(&g_gfx_con, "Press power button to reboot into RCM...\n\n");
    gfx_swap_buffer(&g_gfx_ctxt);
    wait_for_button_and_reboot();
}
