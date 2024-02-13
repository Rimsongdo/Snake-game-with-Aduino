#include "lcd.h"
#include <xc.h>


#define LCD_EN PORTCbits.RC1
#define LCD_RS PORTCbits.RC0

void delay() {
    unsigned int i;
    for (i = 0; i < 100; i++) {
        // Vous pouvez ajuster ce délai selon votre besoin
    }
}




void SendCommand(unsigned char command) {
    LCD_RS = 0;
    delay();
    LCD_EN = 1;
    delay();
    PORTB = command;
    delay();
    LCD_EN = 0;
    delay();
}

void SendData(unsigned char lcddata) {
    LCD_RS = 1;
    delay();
    LCD_EN = 1;
    delay();
    PORTB = lcddata;
    delay();
    LCD_EN = 0;
    delay();
}

void print(const char *str) {
    while (*str != '\0') {
        SendData(*str);
        str++;
    }
}

void SendString(const char *str) {
    while (*str) {
        SendData(*str++);
    }
}

void ClearLCD() {
    unsigned char P = 0x01; 
    SendCommand(P);
}

void SetCursor(unsigned char row, unsigned char col) {
    unsigned char position;

    
    if (row == 0)
        position = 0x80 + col;
    else if (row == 1)
        position = 0xC0 + col;
    else if (row == 2)
        position = 0x94 + col; 
    else if (row == 3)
        position = 0xD4 + col; 
    else
        return; 

   
    SendCommand(position);
}

