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

/*
#include "project.h"
int main(void)
{
    CyGlobalIntEnable;
    SW_Tx_UART_Start();
    SW_Tx_UART_PutCRLF();
    SW_Tx_UART_PutString("Yaroslav Senyk - Variant 13");
    SW_Tx_UART_PutCRLF();
    uint8_t last_state = 0;
    for(;;)
    {
        // button == 0 -> Button pressed
        // button == 1 -> Button released
        // LED_R_Write(0); -> Turn ON
        // LED_R_Write(1); -> Turn OFF
        if(Button_Read() == 1 && last_state == 0) //is button released
        {
            last_state = 1;
            SW_Tx_UART_PutString("Button released");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(0); // Turn ON
            LED_G_Write(0);
            LED_B_Write(0);
        }
        else if(Button_Read() == 0 && last_state == 1) //is button pressed
        {
            last_state = 0;
            SW_Tx_UART_PutString("Button pressed");
            SW_Tx_UART_PutCRLF();
            LED_R_Write(1); // Turn OFF
            LED_G_Write(1);
            LED_B_Write(1);
        }
    }
} 
*/
#include "project.h"

int main(void)
{
    CyGlobalIntEnable;
    
    // Ініціалізація UART та вивід імені (Завдання 1)
    SW_Tx_UART_Start();
    SW_Tx_UART_PutString("Yaroslav Senyk - Variant 13\r\n");

    for(;;)
    {
        // Перевірка статусу кнопки. 0 - кнопка натиснута, 1 - відпущена
        if (Button_Read() == 0) 
        {
            // КОЛІР 1: Cyan (Блакитний) = Green (0) + Blue (0)
            SW_Tx_UART_PutString("Button Pressed: Blinking Cyan\r\n");
            
            // Вмикаємо Cyan
            LED_R_Write(1); // Червоний вимкнено
            LED_G_Write(0); // Зелений увімкнено
            LED_B_Write(0); // Синій увімкнено
            CyDelay(500);
            
            // Вимикаємо всі
            LED_R_Write(1);
            LED_G_Write(1);
            LED_B_Write(1);
            CyDelay(500);
        }
        else 
        {
            // КОЛІР 2: Green (Зелений)
            SW_Tx_UART_PutString("Button Released: Blinking Green\r\n");
            
            // Вмикаємо Green
            LED_R_Write(1); // Червоний вимкнено
            LED_G_Write(0); // Зелений увімкнено
            LED_B_Write(1); // Синій вимкнено
            CyDelay(500);
            
            // Вимикаємо всі
            LED_R_Write(1);
            LED_G_Write(1);
            LED_B_Write(1);
            CyDelay(500);
        }
    }
}


/* [] END OF FILE */
