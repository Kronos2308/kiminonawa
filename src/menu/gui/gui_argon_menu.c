/*  
 * Copyright (c) 2018 Guillem96
 *
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
#include <string.h>
#include "menu/gui/gui_argon_menu.h"
#include "menu/gui/gui_menu.h"
#include "menu/gui/gui_menu_pool.h"
#include "core/custom-gui.h"
#include "gfx/di.h"
#include "gfx/gfx.h"

#include "utils/types.h"
#include "utils/fs_utils.h"
#include "utils/dirlist.h"
#include "utils/util.h"
#include "utils/touch.h"
#include "utils/btn.h"
#include "core/launcher.h"
#include "core/payloads.h"
#include "core/custom-gui.h"

#include "mem/heap.h"

#define COLUMNS 4  
#define ROWS 2
#define ELEM_SIZE 230
#define MARGIN_TOP 100
#define MARGIN_LEFT 46
//static int tool_menu_rSD(void* param);
static int tool_extr_rSD(void* param);

static int tool_reboot_rcm(void* param);
static int tool_power_off(void* param);
static int tool_menu_rem(void* param);
int tool_theme(char* param);
static int tool_emu(u32 param);
static int zbackup(char* param);
static int tool_Menus(u32 param);
int tool_dir(char *param);
int tool_servises(u32 param);
int tool_servises_on(char* title);
int tool_servises_off(char* title);

int tool_filete(char* fil);
int tool_file_act(u32 fil);
//menus
u64 main_menu = 2;

//sub menus
u32 permsubY = 0;
u32 permsubX = 0;
u32 sub_buttonW = 0;
u32 sub_buttonH = 0;

//some indicators
u32 onemi = 0;
u32 retir = 0;

//dinamic directory
char *directory = "";
char *filete = "";

/* Generate entries dynamically */
static void generate_payloads_entries(char* payloads, gui_menu_t* menu)
{
    if (payloads == NULL)
    {
        g_gfx_con.scale = 4;
        gfx_con_setpos(&g_gfx_con, 140, 250);
        gfx_printf(&g_gfx_con, "Payloads directory is empty...\n");
        
        g_gfx_con.scale = 3;
        gfx_con_setpos(&g_gfx_con, 110, 370);
        gfx_printf(&g_gfx_con, "Place your payloads inside \"%s\"", PAYLOADS_DIR);

        return;
    }

    u32 i = 0;
    /* For each payload generate its logo, its name and its path */
    while(payloads[i * 256])
    {
        char* payload_path = (char*)malloc(256);
        payload_full_path(&payloads[i * 256], payload_path);
        
        char payload_logo[256];
        payload_logo_path(&payloads[i * 256], payload_logo);

			u32 row = i;
        u32 col = i % COLUMNS;
        u32 x = g_gfx_ctxt.width / COLUMNS * col + MARGIN_LEFT;
        u32 y = 140 / ROWS * row + MARGIN_TOP + MARGIN_TOP;

        const char* payload_wo_bin = str_replace(&payloads[i * 256], ".bin", "");
        gui_menu_append_entry(menu, 
            gui_create_menu_entry(payload_wo_bin,
                                    sd_file_read(payload_logo), 
                                    x, y,
                                    200, 200,
                                    (int (*)(void *))launch_payload, (void*)payload_path));
        i++;
    }
}
// Second Menu 
/*

static void generate_payloads_back(char* payback, gui_menu_t* menu)
{

    u32 e = 0;
    // For each payload generate its logo, its name and its path
    while(payback[e * 256])
    {
        char* payload_path = (char*)malloc(256);
        payload_full_back(&payback[e * 256], payload_path);
        
        char payload_logo[256];
        payload_logo_path(&payback[e * 256], payload_logo);

		u32 row = e / COLUMNS;
        u32 col = e % COLUMNS;
        u32 x = g_gfx_ctxt.width / COLUMNS * col + MARGIN_LEFT;
        u32 y = g_gfx_ctxt.height / ROWS * row + MARGIN_TOP + (row == 0 ? 30 : -60);

        const char* payload_wo_bin = str_replace(&payback[e * 256], ".bin", "");
        gui_menu_append_entry(menu, 
            gui_create_menu_entry(payload_wo_bin,
                                    sd_file_read(payload_logo),
                                    x, y,
                                    200, 200,
                                    (int (*)(void *))launch_payload, (void*)payload_path));
        e++;
	
    }
}
*/
/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void)
{
    /* Init pool for menu */
    gui_menu_pool_init();
gui_menu_t* menu = gui_menu_create("ArgonNX");
sd_mount();


//		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("BETA WIP....", 500, 20, 150, 100, NULL, NULL));

	
//gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/arrowl.bmp"),100, 650, 70, 70,(int (*)(void *))tool_Menus, (void*)0));
char* files = listfil(directory, "*", true);
char* folder = listfol(directory, "*", true);
		
    u32 r = 0;
    u32 w = 0;
    u32 i = 0;
    u32 f = 0;
	u32 y = 90;
	u32 x = 10;
	u32 space = 50;
//	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(directory,600, y-20, 150, 100, (int (*)(void *))tool_Menus, (void*)33));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/sd-root.bmp"),170, 30, 70, 70,(int (*)(void *))tool_Menus, (void*)33));
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(directory, strlen(directory)*8-40+50, 98, 150, 100, NULL, NULL));
	
    while(folder[r * 256])
    {
	if(f >= 22){
	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,670, 150, 100, NULL, NULL));
	break;}
		if(strlen(&folder[r * 256]) <= 100){			
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/folder.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_dir, (void*)&folder[r * 256]));
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(&folder[r * 256], x+strlen(&folder[r * 256])*8-40, y+35, 150, 100, NULL, NULL));

		y = y + space;
		
		f++;
		i++;
			if(i == 11){
			y = 90;
			x = 700;
			i = 0;
			}
		}
	r++;
	}

    while(files[w * 256])
    {
	if(f >= 22){
	gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("More...", 1100,670, 150, 100, NULL, NULL));
	break;}
	if(strlen(&files[w * 256]) <= 100){
			if(strstr(&files[w * 256],".bin") != NULL)
			{
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files-bin.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));
			}else{
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/files.bmp"),x, y+30, 500, 25,(int (*)(void *))tool_filete, (void*)&files[w * 256]));			
			}

		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(&files[w * 256], x+strlen(&files[w * 256])*8-40, y+35, 150, 100, NULL, NULL));
		y = y + space;

	f++;			
	i++;			
			
			if(i == 11){
			y = 90;
			x = 600;
			i = 0;
			}	

		}
	w++;
	}
		if(strlen(filete) >= 1)
		{
		u32 actX = 800;
		u32 actY = 1;
			if(strstr(filete,".bin") != NULL)
			{
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/launch.bmp"),actX+100, actY, 75, 75, (int (*)(void *))tool_file_act, (void*)1));
			}
			if(strstr(filete,".BIN") != NULL)
			{
			gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/launch.bmp"),actX+100, actY, 75, 75, (int (*)(void *))tool_file_act, (void*)1));
			}

		//gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/button.bmp"),1070, 600, 200, 75, (int (*)(void *))zbackup, (void*)"emummc.bk"));
		gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/del.bmp"),actX, actY, 75, 75, (int (*)(void *))tool_file_act, (void*)0));
//		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("eliminar", actX+30, actY+30, 150, 100, NULL, NULL));
		gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap(filete, actX, actY+70, 150, 100, NULL, NULL));
		}

 





/* Generate reboot rcm and shutdown entry **/
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/screenshot.bmp"),400, 650, 70, 70, (int (*)(void *))screenshot, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/power.bmp"),600, 650, 70, 70, tool_power_off, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/rcm.bmp"),800, 650, 70, 70, tool_reboot_rcm, NULL));




    /* Start menu */
    gui_menu_open(menu);

    /* Clear all entries and menus */
    gui_menu_pool_cleanup();
}

static int tool_reboot_rcm(void* param)
{
   reboot_rcm();
    return 0;
}

static int tool_extr_rSD(void* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
	gfx_swap_buffer(&g_gfx_ctxt);
	display_backlight_brightness(100, 1000);
	g_gfx_con.scale = 3;
    gfx_con_setpos(&g_gfx_con, 160, 100);
    gfx_printf(&g_gfx_con, "Ya puedes extraer la SD, Al terminar\n");
    gfx_con_setpos(&g_gfx_con, 230, 130);
    gfx_printf(&g_gfx_con, "Pon la SD y presiona POWER\n\n");
    gfx_con_setpos(&g_gfx_con, 110, 600);
    gfx_printf(&g_gfx_con, "You can now extract the SD, When you finish\n");
    gfx_con_setpos(&g_gfx_con, 230, 630);
    gfx_printf(&g_gfx_con, "Put the SD and press POWER\n");
    gfx_swap_buffer(&g_gfx_ctxt);
	btn_wait_timeout(10000, BTN_POWER);
	display_backlight_brightness(0, 1000);
BootStrapNX();
/*
if (!sd_mount()){BootStrapNX();}//check sd
    u32 *fb = display_init_framebuffer();
    gfx_init_ctxt(&g_gfx_ctxt, fb, 1280, 720, 720);
    gfx_clear_buffer(&g_gfx_ctxt);
    gfx_con_init(&g_gfx_con, &g_gfx_ctxt);
    gfx_con_setcol(&g_gfx_con, 0xFFCCCCCC, 1, BLACK);
//SD remove Menu
    gui_menu_pool_cleanup();

	gui_menu_pool_init();
	gui_menu_t* menu = gui_menu_create("ArgonNX");

gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/rcm.bmp"),800, 650, 70, 70, tool_reboot_rcm, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/power.bmp"),600, 650, 70, 70, tool_power_off, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/extrDS.bmp"),1100, 500, 200, 200, NULL, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/extpower.bmp"),500, 250, 200, 200, tool_menu_rSD, NULL));
    gui_menu_open2(menu);*/
    return 0;
}


static int tool_emu(u32 status)
{
if (!sd_mount()){BootStrapNX();}//check sd

if(status == 33)
{
f_unlink("sxos/eMMC");
display_backlight_brightness(1, 1000);
f_rename("sxos/emunand","sxos/eMMC");

f_rename("sxos/eMMC/boot0.bin","sxos/eMMC/boot0");
f_rename("sxos/eMMC/boot1.bin","sxos/eMMC/boot1");

f_rename("sxos/eMMC/full.00.bin","sxos/eMMC/00");
f_rename("sxos/eMMC/full.01.bin","sxos/eMMC/01");
f_rename("sxos/eMMC/full.02.bin","sxos/eMMC/02");
f_rename("sxos/eMMC/full.03.bin","sxos/eMMC/03");
f_rename("sxos/eMMC/full.04.bin","sxos/eMMC/04");
f_rename("sxos/eMMC/full.05.bin","sxos/eMMC/05");
f_rename("sxos/eMMC/full.06.bin","sxos/eMMC/06");
f_rename("sxos/eMMC/full.07.bin","sxos/eMMC/07");
f_rename("sxos/eMMC/full.08.bin","sxos/eMMC/08");
f_rename("sxos/eMMC/full.09.bin","sxos/eMMC/09");
f_rename("sxos/eMMC/full.10.bin","sxos/eMMC/10");
f_rename("sxos/eMMC/full.11.bin","sxos/eMMC/11");
f_rename("sxos/eMMC/full.12.bin","sxos/eMMC/12");
f_rename("sxos/eMMC/full.13.bin","sxos/eMMC/13");
f_rename("sxos/eMMC/full.14.bin","sxos/eMMC/14");
if (sd_file_exists ("sxos/eMMC/00")&sd_file_exists ("sxos/eMMC/01")&sd_file_exists ("sxos/eMMC/boot1"))
{
	f_mkdir("emummc");
	FIL fp;
	f_open(&fp, "emummc/emummc.ini", FA_WRITE | FA_CREATE_ALWAYS);
	f_puts("[emummc]", &fp);
	f_puts("\nenabled=1", &fp);
	f_puts("\npath=sxos", &fp);
	f_puts("\nnintendo_path=Emutendo", &fp);
	f_puts("\n", &fp);
	f_close(&fp);
}

	gui_init_argon_menu();
return 0;
}
	
if(status == 1)
{
char *str1 = sd_file_read("emummc/emummc.ini");
char* payload_wo_bin = str_replace(str1, "enabled=0", "enabled=1");
FIL op;
f_open(&op, "emummc/emummc.ini", FA_READ);
u32 size = f_size(&op);
f_close(&op);
sd_save_to_file(payload_wo_bin,size,"emummc/emummc.ini");
retir = 2;
}

if(status == 0)
{
char *str1 = sd_file_read("emummc/emummc.ini");
char* payload_wo_bin = str_replace(str1, "enabled=1", "enabled=0");
FIL op;
f_open(&op, "emummc/emummc.ini", FA_READ);
u32 size = f_size(&op);
f_close(&op);
sd_save_to_file(payload_wo_bin,size,"emummc/emummc.ini");
retir = 1;
}

/*	
if (sd_file_exists ("emummc/emummc.ini"))
{f_unlink("emummc/emummc.ini.bak");}

if (sd_file_exists ("emummc/emummc.ini"))
{
f_rename("emummc/emummc.ini","emummc/emummc.ini.bak");
}else{
f_rename("emummc/emummc.ini.bak","emummc/emummc.ini");

    FIL fp;
    f_open(&fp, "emummc/emummc.ini", FA_WRITE);
    f_puts(payload_wo_bin, &fp);
    f_close(&fp);
*/
	gui_init_argon_menu();
return 0;
}

static int tool_Menus(u32 param)
{
if (!sd_mount()){BootStrapNX();}//check sd

if(param == 33){
directory = "";
gui_init_argon_menu();
return 0;
}
display_backlight_brightness(50, 1000);
//check if is the same menu
if(main_menu == param)
return 0;

//set menu number
main_menu = param;

//gui_menu_pool_cleanup();
gui_init_argon_menu();
return 0;
}

int tool_servises_on(char* title)
{
if (!sd_mount()){BootStrapNX();}//check sd
char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		sd_save_to_file("",0,path);
		strcpy(path, "ReiNX/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		sd_save_to_file("",0,path);
gui_init_argon_menu();
return 0;
}

int tool_servises_off(char* title)
{
if (!sd_mount()){BootStrapNX();}//check sd
char* path = (char*)malloc(256);
		strcpy(path, "atmosphere/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);
		strcpy(path, "ReiNX/titles/");
		strcat(path, title);
		strcat(path, "/flags/boot2.flag");
		f_unlink(path);
gui_init_argon_menu();
return 0;
}

int tool_dir(char *param)
{
if (!sd_mount()){BootStrapNX();}//check sd
if(strlen(directory) <= 1)
{
directory = param;
}else{
strcat(directory, "/");
strcat(directory, param);

}
gui_init_argon_menu();
return 0;
}

int tool_filete(char* fil)
{

if (!sd_mount()){BootStrapNX();}//check sd
if(strlen(directory) <= 1)
{
filete = fil;
}else{
    char tmp[256];
    strcpy(tmp, directory);
    strcat(tmp, "/");
	strcat(tmp, fil);
filete = tmp;		
}
gui_init_argon_menu();
return 0;
}

int tool_file_act(u32 fil)
{
if (!sd_mount()){BootStrapNX();}//check sd
	if(fil == 0)
	{
	f_unlink(filete);
	free(filete);
	filete = "";
	}
	
	if(fil == 1)
	{
	launch_payload(filete);
	}
gui_init_argon_menu();
return 0;
}

static int tool_menu_rem(void* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
 gfx_printf(&g_gfx_con, "loading\n");
//gui_menu_pool_cleanup();
	f_unlink("/atmosphere/titles/0100000000001000/fsmitm.flag");
	f_unlink("/ReiNX/titles/0100000000001000/fsmitm.flag");
	f_unlink("/SXOS/titles/0100000000001000/fsmitm.flag");
	f_unlink("/sxos/titles/0100000000001000/fsmitm.flag");
	
	//this is a panic option so i will disable the servises also
	f_unlink("/atmosphere/titles/420000000000000E/flags/boot2.flag");
	f_unlink("/ReiNX/titles/420000000000000E/boot2.flag");
	f_unlink("/atmosphere/titles/0100000000000352/flags/boot2.flag");
	f_unlink("/ReiNX/titles/0100000000000352/boot2.flag");
	f_unlink("/atmosphere/titles/4200000000000010/flags/boot2.flag");
	f_unlink("/ReiNX/titles/4200000000000010/boot2.flag");
	f_unlink("/atmosphere/titles/420000000000000B/flags/boot2.flag");
	f_unlink("/ReiNX/titles/420000000000000B/boot2.flag");
	f_unlink("/atmosphere/titles/0100000000000FAF/flags/boot2.flag");
	f_unlink("/ReiNX/titles/0100000000000FAF/boot2.flag");

    sd_save_to_file("", 0, "fixer.del");
launch_payload("payload.bin");
return 0;
}

int tool_theme(char* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
display_backlight_brightness(0, 1000);
copyfileparam(param ,"Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfileparam(param ,"Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfileparam(param ,"Icons/power.bmp","StarDust/Icons/power.bmp");
copyfileparam(param ,"Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfileparam(param ,"Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfileparam(param ,"Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfileparam(param ,"Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfileparam(param,"background.bmp","StarDust/background.bmp");
copyfileparam(param,"logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfileparam(param,"logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfileparam(param,"/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfileparam(param,"logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
gui_init_argon_menu();
return 0;
}

static int zbackup(char* param)
{
if (!sd_mount()){BootStrapNX();}//check sd
display_backlight_brightness(1, 1000);
sd_save_to_file("", 0, param);
launch_payload("StarDust/payloads/zbackup.bin");
return 0;
}

static int tool_power_off(void* param)
{
    gui_menu_pool_cleanup();
    power_off();
    return 0;
}
/*
static int tool_menu_rSD(void* param)
{
	sd_mount();
		if (g_sd_mounted)
		{
		launch_payload("payload.bin");
		}

return 0;
}
*/