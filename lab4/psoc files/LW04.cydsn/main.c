/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"

/* Завдання 5 і 7: Масив двійкових кодів для символів 0-9, -, A, b, C, d, E, F, . */
static const uint8 LED_NUM[] = {
    0xC0, // 0  (індекс 0)
    0xF9, // 1  (індекс 1)
    0xA4, // 2  (індекс 2)
    0xB0, // 3  (індекс 3)
    0x99, // 4  (індекс 4)
    0x92, // 5  (індекс 5)
    0x82, // 6  (індекс 6)
    0xF8, // 7  (індекс 7)
    0x80, // 8  (індекс 8)
    0x90, // 9  (індекс 9)
    0xBF, // -  (індекс 10)
    0x88, // A  (індекс 11)
    0x83, // b  (індекс 12)
    0xC6, // C  (індекс 13)
    0xA1, // d  (індекс 14)
    0x86, // E  (індекс 15)
    0x8E, // F  (індекс 16)
    0x7F  // .  (індекс 17)
};

/* Функція відправки 1 байту даних на зсувний регістр */
static void FourDigit74HC595_sendData(uint8 data) {
    uint8 i;
    for (i = 0; i < 8; i++) {
        if (data & (0x80 >> i)) {
            Pin_DO_Write(1);
        } else {
            Pin_DO_Write(0);
        }
        Pin_CLK_Write(1);
        Pin_CLK_Write(0);
    }
}

/* Функція відправки 2 байтів даних для увімкнення одного символа */
static void FourDigit74HC595_sendOneDigit(uint8 position, uint8 digit, uint8 dot) {
    if (position >= 8) {
        FourDigit74HC595_sendData(0xFF);
        FourDigit74HC595_sendData(0xFF);
    } else {
        FourDigit74HC595_sendData(0xFF & ~(1 << position)); // Відправляємо позицію
        if (dot) {
            FourDigit74HC595_sendData(LED_NUM[digit] & 0x7F);
        } else {
            FourDigit74HC595_sendData(LED_NUM[digit]); // Відправляємо символ
        }
    }
    Pin_Latch_Write(1); // Засувка: виводимо дані на індикатор
    Pin_Latch_Write(0);
}

/* ================= ЛОГІКА МАТРИЧНОЇ КЛАВІАТУРИ (з ЛР №3) ================= */
static void (*COLUMN_x_SetDriveMode[3])(uint8 mode) = {
    COLUMN_0_SetDriveMode,
    COLUMN_1_SetDriveMode,
    COLUMN_2_SetDriveMode
};

static void (*COLUMN_x_Write[3])(uint8 value) = {
    COLUMN_0_Write,
    COLUMN_1_Write,
    COLUMN_2_Write
};

static uint8 (*ROW_x_Read[4])(void) = {
    ROW_0_Read,
    ROW_1_Read,
    ROW_2_Read,
    ROW_3_Read
};

static uint8 keys[4][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1}
};

static const uint8 button_ids[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11} // 10 = '*', 11 = '#'
};

static void initMatrix(void) {
    uint8 column_index;
    for(column_index = 0; column_index < 3; column_index++) {
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

static void readMatrix(void) {
    uint8 column_index, row_index;
    for(column_index = 0; column_index < 3; column_index++) {
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_STRONG);
        COLUMN_x_Write[column_index](0);
        
        for(row_index = 0; row_index < 4; row_index++) {
            keys[row_index][column_index] = ROW_x_Read[row_index]();
        }
        
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

int main(void) {
    uint8 r, c; 
    uint8 last_pressed = 99;
    uint8 current_pressed;
    
    // 0 - режим цифр (0-9), 1 - режим літер (A-F)
    uint8 letter_mode = 0; 

    CyGlobalIntEnable; 
    
    // Ініціалізуємо клавіатуру
    initMatrix();
    
    // Очищуємо дисплей при старті (відправляємо позицію >= 8)
    FourDigit74HC595_sendOneDigit(8, 0, 0); 
    
    for(;;) {
        readMatrix();
        current_pressed = 99; 
        
        // Знаходимо натиснуту кнопку
        for(r = 0; r < 4; r++) {
            for(c = 0; c < 3; c++) {
                if(keys[r][c] == 0) {
                    current_pressed = button_ids[r][c];
                }
            }
        }
        
        // Якщо кнопку ЩОЙНО натиснули
        if(current_pressed != 99 && last_pressed == 99) {
            last_pressed = current_pressed; 
            
            // Завдання 7: Якщо натиснуто '*', перемикаємо режим Літери / Цифри
            if (current_pressed == 10) {
                if (letter_mode == 0) {
                    letter_mode = 1; // Увімкнути режим літер A-F
                } else {
                    letter_mode = 0; // Повернутись до цифр
                }
            } 
            // Виведення символу на дисплей (позиція 0)
            else {
                if (letter_mode == 0) {
                    // РЕЖИМ ЦИФР
                    // Кнопки 1-9 і 0 відображають відповідні цифри
                    if (current_pressed <= 9) {
                        FourDigit74HC595_sendOneDigit(0, current_pressed, 0); 
                    } else if (current_pressed == 11) {
                        // Якщо натиснута кнопка '#', відобразимо тире
                        FourDigit74HC595_sendOneDigit(0, 10, 0); // 10 = індекс '-' в LED_NUM
                    }
                } else {
                    // РЕЖИМ ЛІТЕР (кнопки 1-6 відповідають за A, b, C, d, E, F)
                    switch(current_pressed) {
                        case 1: FourDigit74HC595_sendOneDigit(0, 11, 0); break; // A
                        case 2: FourDigit74HC595_sendOneDigit(0, 12, 0); break; // b
                        case 3: FourDigit74HC595_sendOneDigit(0, 13, 0); break; // C
                        case 4: FourDigit74HC595_sendOneDigit(0, 14, 0); break; // d
                        case 5: FourDigit74HC595_sendOneDigit(0, 15, 0); break; // E
                        case 6: FourDigit74HC595_sendOneDigit(0, 16, 0); break; // F
                        default: FourDigit74HC595_sendOneDigit(0, 10, 0); break; // -
                    }
                }
            }
            CyDelay(50); // Антибрязкіт
        }
        // Якщо кнопку ЩОЙНО відпустили
        else if(current_pressed == 99 && last_pressed != 99) {
            last_pressed = 99;
            
            // (За бажанням) Якщо хочете, щоб дисплей відразу вимикався при відпусканні:
            // FourDigit74HC595_sendOneDigit(8, 0, 0); 
            
            CyDelay(50); // Антибрязкіт
        }
    }
}


/* [] END OF FILE */
