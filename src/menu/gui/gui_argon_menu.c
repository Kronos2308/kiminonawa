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
u32 tog = 0;
static int tool_menu_rSD(void* param);
static int tool_extr_rSD(void* param);

static int tool_reboot_rcm(void* param);
static int tool_power_off(void* param);
static int tool_menu_tog(void* param);
static int tool_menu_rem(void* param);
static int tool_theme_one(void* param);
static int tool_theme_two(void* param);
static int tool_theme_tres(void* param);
static int tool_theme_four(void* param);
static int tool_theme_five(void* param);
static int tool_theme_six(void* param);
static int tool_theme_siben(void* param);

static int backup_full(void* param);
static int backup_lite(void* param);
static int backup_boot(void* param);


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
/* Second Menu */
static void generate_payloads_back(char* payback, gui_menu_t* menu)
{

    u32 e = 0;
    /* For each payload generate its logo, its name and its path */
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

/* Init needed menus for ArgonNX */
void gui_init_argon_menu(void)
{
    /* Init pool for menu */
    gui_menu_pool_init();
gui_menu_t* menu = gui_menu_create("ArgonNX");
 
if (tog == 0){
//generate main menu
generate_payloads_entries(dirlist(PAYLOADS_DIR, "*.bin", false), menu);
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/arrowr.bmp"),250, 650, 70, 70, tool_menu_tog, NULL));
}else{
//generate menu2
generate_payloads_back(dirlist(PAYBACK_DIR, "*.bin", false), menu);
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/arrowl.bmp"),100, 650, 70, 70, tool_menu_tog, NULL));


gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Backup Nand", 1090, 370, 150, 100, NULL, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/button.bmp"),1070, 400, 200, 75, backup_full, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Raw full", 1095, 425, 150, 100, NULL, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/button.bmp"),1070, 500, 200, 75, backup_lite, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Lite System", 1093, 525, 150, 100, NULL, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/button.bmp"),1070, 600, 200, 75, backup_boot, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("boot0/1 Only", 1094, 625, 150, 100, NULL, NULL));


}

//Create Icons, SD exract and theme Remove
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/themes.bmp"),1200, 40, 70, 70, tool_menu_rem, NULL));


//Create Icons, themes
if(sd_file_exists("/StarDust/Atheme/1/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/1/icon.bmp"),300, 10, 70, 70, tool_theme_one, NULL));
if(sd_file_exists("/StarDust/Atheme/2/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/2/icon.bmp"),400, 10, 70, 70, tool_theme_two, NULL));
if(sd_file_exists("/StarDust/Atheme/3/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/3/icon.bmp"),500, 10, 70, 70, tool_theme_tres, NULL));
if(sd_file_exists("/StarDust/Atheme/4/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/4/icon.bmp"),600, 10, 70, 70, tool_theme_four, NULL));
if(sd_file_exists("/StarDust/Atheme/5/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/5/icon.bmp"),700, 10, 70, 70, tool_theme_five, NULL));
if(sd_file_exists("/StarDust/Atheme/6/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/6/icon.bmp"),800, 10, 70, 70, tool_theme_six, NULL));
if(sd_file_exists("/StarDust/Atheme/7/icon.bmp"))
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Atheme/7/icon.bmp"),900, 10, 70, 70, tool_theme_siben, NULL));


/* Generate reboot rcm and shutdown entry **/
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/screenshot.bmp"),400, 650, 70, 70, (int (*)(void *))screenshot, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/power.bmp"),600, 650, 70, 70, tool_power_off, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/rcm.bmp"),800, 650, 70, 70, tool_reboot_rcm, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/SD.bmp"),1200, 140, 70, 70, tool_extr_rSD, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Fix boot->", 1050, 80, 150, 100, NULL, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Remove SD->", 1050, 180, 150, 100, NULL, NULL));

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
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
//SD remove Menu
    gui_menu_pool_cleanup();

	gui_menu_pool_init();
	gui_menu_t* menu = gui_menu_create("ArgonNX");

gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Ya puedes extraer la SD, Al terminar ", 500, 100, 150, 100, NULL, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Pon la SD y presiona este icono", 520, 120, 150, 100, NULL, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("You can now extract the SD, When finished", 500, 160, 150, 100, NULL, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry_no_bitmap("Put the SD and press this icon", 520, 180, 150, 100, NULL, NULL));

gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/rcm.bmp"),800, 650, 70, 70, tool_reboot_rcm, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/power.bmp"),600, 650, 70, 70, tool_power_off, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/extrDS.bmp"),1100, 500, 200, 200, NULL, NULL));
gui_menu_append_entry(menu,gui_create_menu_entry("",sd_file_read("/StarDust/Icons/extpower.bmp"),500, 250, 200, 200, tool_menu_rSD, NULL));
    gui_menu_open2(menu);
    return 0;
}

static int tool_menu_rSD(void* param)
{
	sd_mount();
		if (g_sd_mounted)
		{
		launch_payload("payload.bin");
		}

return 0;
}

static int tool_menu_tog(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(50, 1000);
if (tog == 0){
tog = 1;
}else{
tog = 0;
}
 gfx_printf(&g_gfx_con, "loading\n");
gui_menu_pool_cleanup();
gui_init_argon_menu();

return 0;
}

static int tool_menu_rem(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
 gfx_printf(&g_gfx_con, "loading\n");
gui_menu_pool_cleanup();
	f_unlink("/atmosphere/titles/0100000000001000/fsmitm.flag");
	f_unlink("/ReiNX/titles/0100000000001000/fsmitm.flag");
	f_unlink("/SXOS/titles/0100000000001000/fsmitm.flag");
	f_unlink("/atmosphere/titles/0100000000001013/romfs/lyt/fsmitm.flag");
	f_unlink("/ReiNX/titles/0100000000001013/romfs/lyt/fsmitm.flag");
	f_unlink("/SXOS/titles/0100000000001013/romfs/lyt/fsmitm.flag");
	
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

    u8* buffer = (u8*)malloc(4);
    sd_save_to_file(buffer, 4, "fixer.del");
launch_payload("payload.bin");
return 0;
}

static int tool_theme_one(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/1/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/1/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/1/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/1/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/1/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/1/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/1/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/1/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/1/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/1/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/1/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/1/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}

static int tool_theme_two(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/2/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/2/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/2/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/2/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/2/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/2/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/2/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/2/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/2/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/2/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/2/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/2/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}

static int tool_theme_tres(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/3/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/3/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/3/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/3/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/3/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/3/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/3/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/3/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/3/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/3/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/3/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/3/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}

static int tool_theme_four(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/4/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/4/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/4/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/4/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/4/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/4/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/4/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/4/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/4/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/4/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/4/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/4/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}

static int tool_theme_five(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/5/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/5/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/5/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/5/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/5/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/5/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/5/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/5/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/5/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/5/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/5/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/5/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}

static int tool_theme_six(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/6/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/6/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/6/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/6/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/6/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/6/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/6/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/6/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/6/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/6/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/6/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/6/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}


static int tool_theme_siben(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
copyfile("StarDust/Atheme/7/Icons/arrowl.bmp","StarDust/Icons/arrowl.bmp");
copyfile("StarDust/Atheme/7/Icons/arrowr.bmp","StarDust/Icons/arrowr.bmp");
copyfile("StarDust/Atheme/7/Icons/power.bmp","StarDust/Icons/power.bmp");
copyfile("StarDust/Atheme/7/Icons/rcm.bmp","StarDust/Icons/rcm.bmp");
copyfile("StarDust/Atheme/7/Icons/screenshot.bmp","StarDust/Icons/screenshot.bmp");
copyfile("StarDust/Atheme/7/Icons/themes.bmp","StarDust/Icons/themes.bmp");
copyfile("StarDust/Atheme/7/Icons/SD.bmp","StarDust/Icons/SD.bmp");

copyfile("StarDust/Atheme/7/background.bmp","StarDust/background.bmp");
copyfile("StarDust/Atheme/7/logos/Atmosphere.bmp","StarDust/logos/Atmosphere.bmp");
copyfile("StarDust/Atheme/7/logos/Reinx.bmp","StarDust/logos/Reinx.bmp");
copyfile("StarDust/Atheme/7/logos/sxos.bmp","StarDust/logos/sxos.bmp");
copyfile("StarDust/Atheme/7/logos/zBackup.bmp","StarDust/logos/zBackup.bmp");
launch_payload("payload.bin");
return 0;
}

static int backup_full(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
u8* buffer = (u8*)malloc(4);
sd_save_to_file(buffer, 4, "StarDust/raw.bk");
launch_payload("StarDust/payloads/zbackup.bin");
return 0;
}

static int backup_lite(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
u8* buffer = (u8*)malloc(4);
sd_save_to_file(buffer, 4, "StarDust/syslite.bk");
launch_payload("StarDust/payloads/zbackup.bin");
return 0;
}

static int backup_boot(void* param)
{
sd_mount();
		if (!g_sd_mounted)
		{return 0;}
display_backlight_brightness(1, 1000);
u8* buffer = (u8*)malloc(4);
sd_save_to_file(buffer, 4, "StarDust/boot.bk");
launch_payload("StarDust/payloads/zbackup.bin");
return 0;
}

static int tool_power_off(void* param)
{
    gui_menu_pool_cleanup();
    power_off();
    return 0;
}