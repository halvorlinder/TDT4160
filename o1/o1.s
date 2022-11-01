.thumb
.syntax unified

.include "gpio_constants.s"     // Register-adresser og konstanter for GPIO

.text
	.global Start
	
Start:
	//Loads the address of GPIO Port E DOUT to R4
	LDR R4, = GPIO_BASE + 9*4*4+12

	//Loads the address of GPIO Port B DIN to R0
	LDR R0, = GPIO_BASE + 9*4*1+28

	//Loads the value
	LDR R1, = #512
	Loop:
		LDR R2, [R0]
		AND R3, R2, R1
		CMP R3, R1
		BNE False
			LDR R5, = #0
			STR R5, [R4]
			B EndIf
		False:
			LDR R5, = #1
			LSL R5, R5, #2
			STR R5, [R4]
		EndIf:

		B Loop


NOP // Behold denne på bunnen av fila

