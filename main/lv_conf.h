#ifndef LV_CONF_H
#define LV_CONF_H

/*====================
 * COLOR SETTINGS
 *===================*/

/* Swap the 2 bytes of RGB565 color. 
   1 = swap (big-endian style cho display bus, fix faded text/antialias)
   0 = no swap
*/
#define LV_COLOR_16_SWAP 1

/* Đảm bảo match config menuconfig của anh */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_FORMAT LV_COLOR_FORMAT_RGB565

/* Các option khác giữ default, không cần thêm nếu không dùng */

#endif /*LV_CONF_H*/