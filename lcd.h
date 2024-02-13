#ifndef LCD_H
#define LCD_H

#include <xc.h>

// Function prototypes
void SendCommand(unsigned char command);
void SendData(unsigned char lcddata);
void print(const char *str);
void SendString(const char *str);
void ClearLCD();
void SetCursor(unsigned char row, unsigned char col);

#endif /* LCD_H */
