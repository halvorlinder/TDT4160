.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO
.include "sys-tick_constants.s" // Register-adresser og konstanter for SysTick

.text

.global GPIO_ODD_IRQHandler
.thumb_func
GPIO_ODD_IRQHandler:
	LDR R0, = SYSTICK_BASE
	LDR R1, [R0]
	LDR R2, = #1
	AND R1, R1, R2
	CMP R1, #0
	BNE False
		MOV R2, #111
		B EndIf
	False:
		MOV R2, #110
		B EndIf
	EndIf:
	STR R2, [R0]

	LDR R4, = GPIO_BASE + GPIO_IFC
	MOV R5, #1
	LSL R5, R5, #9
	STR R5, [R4]

	BX LR // Returner fra interrupt

.global SysTick_Handler
.thumb_func
SysTick_Handler:
	//Load tenths, increment and check if equal to 10
	LDR R4, [R7]
	ADD R4, R4, #1
	CMP R4, #10
	BNE False1
		//Toggle Led
		LDR R10, = GPIO_BASE + 9*4*4+24
		LDR R11, =#1
		LSL R11, R11, #2
		STR R11, [R10]

		//Reset the value at tenths, load seconds, increment seconds and check if equal to 60
		MOV R4, #0
		LDR R5, [R8]
		ADD R5, R5, #1
		CMP R5, #60
		BNE False2
			//Reset the value at seconds, load minutes, increment minute and store minutes
			MOV R5, #0
			LDR R6, [R9]
			ADD R6, R6, #1
			STR R6, [R9]
		False2:
			B EndIf2
		EndIf2:
		//Store seconds
		STR R5, [R8]
	False1:
		B EndIf1
	EndIf1:
	//Store tenths
	STR R4, [R7]

	BX LR

.global Start

Start:
	//Inits the ctrl of systick
	LDR R0, =SYSTICK_BASE
	MOV R1, #110
	STR R1, [R0]

	//Inits the load of systick
	LDR R2, =FREQUENCY/10
	LDR R1, =SYSTICK_LOAD
	ADD R1, R1, R0
	STR R2, [R1]

	//Pointers to memory locations of stop watch values
    LDR R7, =tenths
    LDR R8, =seconds
    LDR R9, =minutes

	//Create mask and set exitpsel
	LDR R0, = GPIO_BASE + GPIO_EXTIPSELH
	LDR R1, [R0]
	LDR R2, = #0b1111
	LSL R2, #4
	MVN R2, R2
	AND R1, R1, R2

	LDR R2, = #0b0001
	LSL R2, R2, #4
	ORR R2, R2, R1
	STR R2, [R0]

	//Set exitfall
	LDR R0, = GPIO_BASE + GPIO_EXTIFALL
	LDR R1, [R0]
	LDR R2, = #1
	LSL R2, R2, #9
	ORR R1, R1, R2
	STR R1, [R0]

	//IEN
	LDR R0, = GPIO_BASE + GPIO_IEN
	LDR R1, [R0]
	LDR R2, = #1
	LSL R2, R2, #9
	ORR R1, R1, R2
	STR R1, [R0]

	Loop:
	B Loop



NOP // Behold denne på bunnen av fila
