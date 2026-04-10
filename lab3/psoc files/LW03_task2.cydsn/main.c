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

/* Масиви вказівників на функції для керування пінами */
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

/* Масив для зберігання стану кнопок [ROW][COLUMN] */
static uint8 keys[4][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1}
};

/* Масив для ідентифікації кнопок: 10 - це '*', 11 - це '#' */
static const uint8 button_ids[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11}
};

/* Функція ініціалізації матриці */
static void initMatrix(void)
{
    uint8 column_index;
    for(column_index = 0; column_index < 3; column_index++)
    {
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

/* Функція зчитування стану матриці */
static void readMatrix(void)
{
    uint8 row_counter = 4;
    uint8 column_counter = 3;
    uint8 column_index, row_index;
    
    for(column_index = 0; column_index < column_counter; column_index++)
    {
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_STRONG);
        COLUMN_x_Write[column_index](0);
        
        for(row_index = 0; row_index < row_counter; row_index++)
        {
            keys[row_index][column_index] = ROW_x_Read[row_index]();
        }
        
        COLUMN_x_SetDriveMode[column_index](COLUMN_0_DM_DIG_HIZ);
    }
}

int main(void)
{
    /* Оголошення змінних */
    uint8 r, c; 
    uint8 last_state = 99; // 99 означає, що жодна кнопка не натиснута
    uint8 current_pressed;
    
    // Змінні для пароля
    uint8 input_pass[4] = {0, 0, 0, 0};
    uint8 pass_index = 0;
    uint8 correct_pass[4] = {1, 2, 3, 4}; // Еталонний пароль (1-2-3-4)
    
    // ПРАПОРЕЦЬ АВТОРИЗАЦІЇ: 0 - заблоковано, 1 - розблоковано
    uint8 is_logged_in = 0; 

    CyGlobalIntEnable; /* Enable global interrupts. */
    
    SW_Tx_UART_Start();
    SW_Tx_UART_PutCRLF();
    SW_Tx_UART_PutString("System Locked. Enter 4-digit password:");
    SW_Tx_UART_PutCRLF();
    
    initMatrix();
    
    /* Початковий стан: система заблокована, світлодіоди вимкнені (Black) */
    LED_R_Write(1);
    LED_G_Write(1);
    LED_B_Write(1);
    
    for(;;)
    {
        readMatrix();
        current_pressed = 99; 
        
        /* Скануємо матрицю, щоб знайти натиснуту кнопку */
        for(r = 0; r < 4; r++)
        {
            for(c = 0; c < 3; c++)
            {
                if(keys[r][c] == 0) 
                {
                    current_pressed = button_ids[r][c];
                }
            }
        }
        
        /* Якщо кнопку ЩОЙНО натиснули */
        if(current_pressed != 99 && last_state == 99)
        {
            last_state = current_pressed; 
            
            if (is_logged_in == 0)
            {
                // ==========================================
                // ЗАВДАННЯ 3: РЕЖИМ ВВОДУ ПАРОЛЯ
                // ==========================================
                input_pass[pass_index] = current_pressed;
                pass_index++;
                
                SW_Tx_UART_PutString("*"); 
                
                if(pass_index == 4)
                {
                    SW_Tx_UART_PutCRLF();
                    
                    if(input_pass[0] == correct_pass[0] && 
                       input_pass[1] == correct_pass[1] && 
                       input_pass[2] == correct_pass[2] && 
                       input_pass[3] == correct_pass[3])
                    {
                        SW_Tx_UART_PutString("Access allowed! System Unlocked.");
                        is_logged_in = 1; // РОЗБЛОКОВУЄМО СИСТЕМУ
                        
                        // Згідно з завданням 1, на початку після розблокування горить White
                        LED_R_Write(0); LED_G_Write(0); LED_B_Write(0);
                    }
                    else
                    {
                        SW_Tx_UART_PutString("Access denied! Try again:");
                    }
                    
                    SW_Tx_UART_PutCRLF();
                    pass_index = 0; // Скидаємо лічильник
                }
            }
            else 
            {
                // ==========================================
                // ЗАВДАННЯ 1 і 2: РЕЖИМ КЕРУВАННЯ КОЛЬОРАМИ
                // (Працює тільки після правильного пароля)
                // ==========================================
                switch(current_pressed)
                {
                    case 1: case 7: // Red
                        LED_R_Write(0); LED_G_Write(1); LED_B_Write(1); break;
                    case 2: case 8: // Green
                        LED_R_Write(1); LED_G_Write(0); LED_B_Write(1); break;
                    case 3: case 9: // Blue
                        LED_R_Write(1); LED_G_Write(1); LED_B_Write(0); break;
                    case 4: case 10: // Yellow (10 = *)
                        LED_R_Write(0); LED_G_Write(0); LED_B_Write(1); break;
                    case 5: case 0: // Purple
                        LED_R_Write(0); LED_G_Write(1); LED_B_Write(0); break;
                    case 6: case 11: // Cyan (11 = #)
                        LED_R_Write(1); LED_G_Write(0); LED_B_Write(0); break;
                    default: break;
                }
            }
            
            CyDelay(50); // Антибрязкіт
        }
        /* Якщо кнопку ЩОЙНО відпустили */
        else if(current_pressed == 99 && last_state != 99)
        {
            last_state = 99; // Скидаємо стан
            
            // Вимикаємо світлодіод (Black) при відпусканні ТІЛЬКИ якщо ми розблоковані
            if (is_logged_in == 1)
            {
                LED_R_Write(1);
                LED_G_Write(1);
                LED_B_Write(1);
            }
            
            CyDelay(50); // Антибрязкіт
        }
    }
}


/* [] END OF FILE */
