#ifndef LCD_H
#define LCD_H

void LCD_Setup(void);                       // Initialize the LCD
void LCD_Clear(void);                       // Clear the screen and return to position (0,0)
void LCD_Move(int line, int col);           // Move the position to the given line and column
void LCD_WriteChar(char c);                 // Write a character at the current position
void LCD_WriteString(const char * string);  // Write a string, starting at the current position

void LCD_Home(void);                        // Move to (0,0) and reset any scrolling
void LCD_Entry(int id, int s);              // Control how the display moves after sending a character
void LCD_Display(int d, int c, int b);      // Turn the display on/off and change cursor settings
void LCD_Shift(int sc, int rl);             // Shift the position of the display  
void LCD_Function(int n, int f);            // Set the number of lines (0,1) and the font size
void LCD_CustomChar(unsigned char val, const char data[7]); // Write a custom character to CGRAM
void LCD_Write(int rs, unsigned char db70); // Write a command to the LCD
void LCD_CMove(unsigned char addr);         // Move to the given address in CGRAM
unsigned char LCD_Read(int rs);             // Read a value from the LCD
#endif
