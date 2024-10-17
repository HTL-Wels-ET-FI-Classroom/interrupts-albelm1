/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

volatile int timer_select = 0;
volatile int farbe_select = 0;

void EXTI0_IRQHandler(void){

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);

	timer_select = !timer_select;

}

void  EXTI2_IRQHandler(void){

	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);

	farbe_select = !farbe_select;
}


/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* MCU Configuration--------------------------------------------------------*/
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize LCD and touch screen */
	LCD_Init();
	TS_Init(LCD_GetXSize(), LCD_GetYSize());
	/* touch screen calibration */
	//	TS_Calibration();

	/* Clear the LCD and display basic starter text */
	LCD_Clear(LCD_COLOR_BLACK);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_SetBackColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font20);
	// There are 2 ways to print text to screen: using printf or LCD_* functions
	LCD_DisplayStringAtLine(0, "    HTL Wels");
	// printf Alternative
	LCD_SetPrintPosition(1, 0);
	printf(" Fischergasse 30");
	LCD_SetPrintPosition(2, 0);
	printf("   A-4600 Wels");

	LCD_SetFont(&Font8);
	LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
	LCD_DisplayStringAtLineMode(39, "Abazi Elmi", CENTER_MODE);


	GPIO_InitTypeDef LED;

	LED.Alternate = 0;
	LED.Mode = GPIO_MODE_AF_PP;
	LED.Pin = GPIO_PIN_13;
	LED.Pull = GPIO_NOPULL;
	LED.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOG, &LED);


	HAL_NVIC_EnableIRQ(EXTI2_IRQn);

	GPIO_InitTypeDef farbe;

	farbe.Alternate = 0;
	farbe.Mode = GPIO_MODE_IT_RISING;
	farbe.Pin = GPIO_PIN_2;
	farbe.Pull = GPIO_NOPULL;
	farbe.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOC, &farbe);



	int cnt1 = 0;
	int cnt2 = 0;


	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	GPIO_InitTypeDef hugo;

	hugo.Alternate = 0;
	hugo.Mode = GPIO_MODE_IT_RISING;
	hugo.Pin = GPIO_PIN_0;
	hugo.Pull = GPIO_NOPULL;
	hugo.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOA, &hugo);

	/* Infinite loop */
	while (1)
	{
		//execute main loop every 100ms
		HAL_Delay(100);

		// display timer
		cnt1++;
		LCD_SetFont(&Font20);
		LCD_SetTextColor(LCD_COLOR_RED);
		LCD_SetPrintPosition(5, 0);

		HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);
		printf("   Timer: %.1f", cnt1/10.0);

		LCD_SetPrintPosition(7, 0);
		printf("   Timer2: %.1f", cnt2/10.0);


		if(timer_select == 0){
			cnt1++;
		}else if(timer_select == 1){
			cnt2++;
		}
		// test touch interface
		int x, y;
		if (GetTouchState(&x, &y)) {
			LCD_FillCircle(x, y, 5);
		}


	}
//return cnt1;
}

/**
 * Check if User Button has been pressed
 * @param none
 * @return 1 if user button input (PA0) is high
 */
static int GetUserButtonPressed(void) {
	return (GPIOA->IDR & 0x0001);
}

/**
 * Check if touch interface has been used
 * @param xCoord x coordinate of touch event in pixels
 * @param yCoord y coordinate of touch event in pixels
 * @return 1 if touch event has been detected
 */
static int GetTouchState (int* xCoord, int* yCoord) {
	void    BSP_TS_GetState(TS_StateTypeDef *TsState);
	TS_StateTypeDef TsState;
	int touchclick = 0;

	TS_GetState(&TsState);
	if (TsState.TouchDetected) {
		*xCoord = TsState.X;
		*yCoord = TsState.Y;
		touchclick = 1;
		if (TS_IsCalibrationDone()) {
			*xCoord = TS_Calibration_GetX(*xCoord);
			*yCoord = TS_Calibration_GetY(*yCoord);
		}
	}

	return touchclick;
}


