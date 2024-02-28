/*
 * myLCD.h
 *
 *	Fungsi-fungsi LCD untuk LKS
 *
 *	Code pada file ini dibolehkan diotak-atik jika perlu.
 *	Tetapi seharusnya tidak perlu untuk diubah.
 *
 *  Created on: April 12, 2023
 *      Author: InaSkills
 */

#ifndef INC_MYLCD_H_
#define INC_MYLCD_H_

#include "main.h"
#include "string.h"

/**
  * @brief  Fungsi untuk mengirim data 4-bit pada LCD
  *
  *	@note:	Fungsi internal.
  *
  * @param  data: nilai 4-bit data yang dikirim
  * 		rs_state: kondisi pin RS untuk set mode
  *
  * @retval None
  */
void _lcd_write_4bit(uint8_t data, uint8_t rs_state) {
	HAL_GPIO_WritePin(LCD_RW_GPIO_Port, LCD_RW_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, rs_state);

	HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, (data >> 3) & 0x01);
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, (data >> 2) & 0x01);
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, (data >> 1) & 0x01);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, (data >> 0) & 0x01);

	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_SET);

	uint16_t i = 0;
	while(i < 7200) i++;

	HAL_GPIO_WritePin(LCD_EN_GPIO_Port, LCD_EN_Pin, GPIO_PIN_RESET);
}

/**
  * @brief  Fungsi untuk mengirim data command 8-bit pada LCD
  * 		sesuai datasheet.
  *	@note:	Fungsi internal.
  *
  * @param  cmd: command 8-bit yang akan dikirim
  *
  * @retval None
  */
void _lcd_write_cmd(uint8_t cmd) {
	_lcd_write_4bit(((cmd>>4)&0x0f), 0);
	_lcd_write_4bit((cmd&0x0f), 0);
}

/**
  * @brief  Fungsi untuk mengirim data karakter ASCII 8-bit
  * 		pada LCD sesuai datasheet/tabel ASCII.
  *
  * @note:	Fungsi internal.
  *
  * @param  data: data karakter 8-bit yang akan dikirim
  *
  * @retval None
  */
void _lcd_write_data(uint8_t cmd) {
	_lcd_write_4bit(((cmd>>4)&0x0f), 1);
	_lcd_write_4bit((cmd&0x0f), 1);
}

/**
  * @brief  Fungsi untuk inisialisasi LCD
  *
  * @note:	Fungsi ini dipanggil sekali sebelum penggunaan LCD.
  *
  * @param  None
  *
  * @retval None
  */
void LCD_Init(void) {
	//	Wait for stable Vcc..
	HAL_Delay(50);

	//	See datasheet..
	//	Set 4-bits
	_lcd_write_cmd(0x33);
	_lcd_write_cmd(0x32);

	//	Set 4-bit, normal font
	_lcd_write_cmd(0x28);
	//	Set display off, cursor and blink off
	_lcd_write_cmd(0x08);

	//	Set entry mode increment
	_lcd_write_cmd(0x06);
	//	Set display on, cursor and blink off
	_lcd_write_cmd(0x0C);

	//	Init end, clear entire display
	_lcd_write_cmd(0x01);
	HAL_Delay(1);
}

/**
  * @brief  Fungsi untuk mengatur posisi cursor LCD pada koordinat (x, y)
  *
  * @note:	Sebaiknya digunakan setiap sebelum memanggil LCD_Print()
  * 		agar posisi karakter konsisten.
  *
  * @param  x: posisi kursor pada baris (0-19)
  * 		y: posisi kursor pada kolom (0-3)
  *
  * @retval None
  */
void LCD_SetCursor(uint8_t pos_x, uint8_t pos_y) {
	uint8_t col = 0x80;
	switch (pos_y)
	{
	case 0:
		col |= 0x00;
		break;
	case 1:
		col |= 0x40;
		break;
	case 2:
		col |= 0x14;
		break;
	case 3:
		col |= 0x54;
		break;
	}
	col += pos_x;
	_lcd_write_cmd(col);
}

/**
  * @brief  Fungsi untuk menghapus seluruh tampilan LCD
  *
  *	@note:	Setelah pemanggilan fungsi ini, cursor otomatis
  *			kembali ke posisi home (0, 0)
  *
  * @param  None
  *
  * @retval None
  */
void LCD_Clear(void) {
	_lcd_write_cmd(0x01);
	HAL_Delay(1);
}

/**
  * @brief  Fungsi untuk mengatur kondisi backlight LCD
  *
  * @param  state: 0 = off, 1 = on.
  *
  * @retval None
  */
void LCD_SetBacklight(_Bool state) {
	HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, state);
}


/**
  * @brief  Fungsi untuk mengirimkan beberapa karakter atau string
  * 		pada tampilan LCD
  *
  * @param  str: Data string yang akan dikirimkan
  *
  * @note:	contoh Penggunaan
  *			LCD_Print("Hello World")
  *			atau
  *			LCD_Print(lcd_buffer) jika lcd_buffer adalah char[]
  *
  * @retval None
  */
void LCD_Print(char *str) {
	static char *strs = NULL;
	if(strs == NULL || strcmp(strs,str)!=0){
		while (*str) _lcd_write_data (*str++);
	}
	strs = str;
}

#endif /* INC_MYLCD_H_ */
