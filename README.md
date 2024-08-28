# AD7606 on STM32 Nucleo-F429ZI

Directory library contains the .h file and .c file, while directory AD7606_lib contains a simple example.

## Thought
![image](https://hackmd.io/_uploads/ry7EjAOo0.png)


## Configurations

### GPIO

- PE10：GPIO_EXTI10 (name:AD7606Irq)
    - GPIO mode: External Interrupt Mode with Falling Edge trigger detection
    - GPIO pull-up/pull-down: Pull up
    - remember to EXTI interrupt (checkbox)!
- PE8：GPIO_Output (name:AD7606Cs)
- PE7：GPIO_Output (name:AD7606Rst)
- PF14：GPIO_Output (name:AD7606Range)
- PF15：GPIO_Output (name:AD7606Os0)
- PG14：GPIO_Output (name:AD7606Os1)
- PG9：GPIO_Output (name:AD7606Os2)

![image](https://hackmd.io/_uploads/ryf4gtsoR.png)
![image](https://hackmd.io/_uploads/r1gIgFos0.png)


### TIM4
- Desired frequency: 10kHz (changeable, up to 20kHz, but have to do calculation yourself)
- PSC: 84 - 1
- ARR: 100 - 1
- Pulse: 99 - 1 (can be lower)
- Clock source: internal clock
- Channel 2: PWM Generation CH2
![image](https://hackmd.io/_uploads/SkjReKis0.png)

![image](https://hackmd.io/_uploads/HJHeZKjj0.png)

![image](https://hackmd.io/_uploads/BJyG-Yij0.png)


### SPI2
- Mode: Full-Duplex Master
- Hardware NSS Signal: Disable
- Data Size: 16 bits
- First bit: MSB first
- CPOL: High
- CPHA: 1 Edge
![image](https://hackmd.io/_uploads/H1JC0T_i0.png)
![image](https://hackmd.io/_uploads/ByE1JRusC.png)

## Wiring

![457021405_819058840434908_1811233188942302775_n (2)](https://hackmd.io/_uploads/B17zSFos0.jpg)



- CVA || CVB -> PD13 (TIM4 CH2 PWM output)
- RD -> PB10 (SPI2 SCK)
- BUSY -> PE10 (AD7606Irq)
- CS -> PE8 (AD7606Cs)
- RST -> PE7 (AD7606Rst)
- DB7 -> PC2 (SPI2 MISO)
- RAGE -> PF14 (AD7606Range)
- OS0 -> PF15 (AD7606Os0)
- OS1 -> PG14 (AD7606Os1)
- OS2 -> PG9 (AD7606Os2)
- VIO -> 3V3
- 5V -> 5V
- GND -> GND

## Code

### Custom Functions

```
/*============ GPIO control for CS and RST pins ============*/

void AD7606Cs_High();
void AD7606Cs_Low();
void AD7606Rst_High();
void AD7606Rst_Low();

/*=================== Implementation =======================*/

void AD7606Init(void);                     //initialize the converter (calls reset)
void AD7606Reset(void);                    //reset the converter

void AD7606Start(TIM_HandleTypeDef* htim4, unsigned int PWM_channel, uint8_t OverSamplingState, uint8_t range); //start conversion
void AD7606Stop(TIM_HandleTypeDef* htim4, unsigned int PWM_channel);                                            //stop conversion

float AD7606ConvValue(uint16_t bin, uint8_t range);  //convert the value raw binary data to voltage according to i/p voltage range
void AD7606BusyIrqCallback(uint16_t *ad7606Val, uint8_t ad7606Chl, SPI_HandleTypeDef* hspi2); //called by external interrupt (falling edge of BUSY)

void AD7606SetOversampling(uint8_t);  	    //set oversampling value
void AD7606SetRange(uint8_t range);         //set ADC reading range

```

To initialize the ADC, call
```
AD7606Init();
```

To reset ADC, call 
```
AD7606Reset();
```
To start conversion, call
```
AD7606Start(&htim4, PWM_CHANNEL, ad7606_OversamplingState, ad7606_Range);
// arguments (in order): 
// your timer (address, need prefix &)
// your PWM output channel connected to CVA and CVB, defined in AD7606.h (configured in IDE, channel)
// oversampling rate, should be 1, 2, 4, 8, 16, 32, 64. Invalid number yields no oversampling (same as 1)
// measuring range(in voltage, 5 means +-5V and 10 means +-10V). invalid number gives a measuring range of +-10V
```
To stop conversion, call
```
void AD7606Stop(&htim4, PWM_CHANNEL); 
// arguments (in order):
// your timer (address, need prefix &)
// your PWM output channel connected to CVA and CVB, defined in AD7606.h (configured in IDE, channel)
```
To turn the raw reading data into voltage, call
```
AD7606ConvValue(ad7606Buff[i], ad7606_Range);
// arguments (in order):
// raw data read from ADC
// measuring range
```
To set oversampling rate, call
```
AD7606SetOversampling(ad7606_OversamplingState);
// argument: oversampling state, should be 1, 2, 4, 8, 16, 32, 64. Invalid number yields no oversampling (same as 1)
```
To set measuring range, call
```
AD7606SetRange(ad7606_Range);
// argument: measuring range(in voltage, 5 means +-5V and 10 means +-10V). invalid number gives a measuring range of +-10V
```

### A Quick Start
- Follow the above instructions to configure the board in STM32CubeIDE
- Put AD7606.h and AD7606.c in Inc and Src respectively
- ![image](https://hackmd.io/_uploads/rkbpg5ssA.png)

- Include AD7606.h under ```/* USER CODE BEGIN Includes */```
```
#include "AD7606.h"
```
- Define PWM_CHANNEL according to your implementation in AD7606.h file, under ```# define SAMPLING_CHANNEL 8```, in my case: 
```
# define PWM_CHANNEL TIM_CHANNEL_2
```
- Declare some global variables under ```/* USER CODE BEGIN PV */```
```
uint16_t ad7606Buff[SAMPLING_CHANNEL] = {0};
float final_value[8];
uint8_t ad7606_OversamplingState = 1;    //change this value to your oversampling rate (1, 2, 4... 64, invalid number gives a result of no oversampling, which is 1)
uint8_t ad7606_Range = 10;               //change this value to your measuring range, 5 for +-5V; 10 for +-10V, invalid number gives a range of +-10V
```

- If you want to use debug mode to inspect the variables, try putting the following code in main, under ```/* USER CODE BEGIN 2 */```
```
  /* ============== for debug mode =============== */
  ad7606Buff[0] = 0;
  final_value[0] = 0;
  ad7606_OversamplingState = 1;    //changeable
  ad7606_Range = 10;               //changeable
```

- Initialize the ADC and start conversion. Under ```/* USER CODE BEGIN 2 */```
```
AD7606Init();
AD7606Start(&htim4, PWM_CHANNEL, ad7606_OversamplingState, ad7606_Range);
```
- If you want to change oversampling rate or range in runtime, try under ```/* USER CODE BEGIN 3 */```
```
AD7606SetOversampling(ad7606_OversamplingState);
AD7606SetRange(ad7606_Range);
```
- Set the callback procedure upon external interrupt (falling edge of BUSY), under ```/* USER CODE BEGIN 4 */```
```
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == AD7606Irq_Pin){
		AD7606BusyIrqCallback(ad7606Buff, SAMPLING_CHANNEL, &hspi2);
	}
	for(int i = 0; i < 8; i++){
		final_value[i] = AD7606ConvValue(ad7606Buff[i], ad7606_Range);
	}
}
```
- It should work now. (at least in my case)
