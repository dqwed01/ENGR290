/*
 * avr_reference.h: hover documentation for the ATmega328P registers used in this project.
 *
 * EDITOR-ONLY. VS Code force-includes this file for IntelliSense (see
 * .vscode/c_cpp_properties.json -> "forcedInclude"). The Arduino build never sees it.
 *
 * How it works: it includes the real avr-libc headers, then #undef's each documented
 * macro and re-#define's it with the EXACT SAME definition, generated from avr-gcc -dM,
 * so the hover tooltip shows the doc comment above it. Because the sketch's own
 * #include <avr/io.h> is then skipped by the header guard, these are the definitions
 * IntelliSense sees. Values are unchanged; only comments are added.
 *
 * Source: ATmega328P datasheet (DS40002061) + avr-libc 2.0 headers. MCU: ATmega328P @ 16 MHz.
 */
#ifndef AVR_REFERENCE_H
#define AVR_REFERENCE_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/twi.h>

/* ==========================================================================
 * Core: SREG, interrupts, helpers
 * ========================================================================== */

#undef SREG
/**
 * AVR Status Register.
 * bit 7 I : Global Interrupt Enable. sei() sets it, cli() clears it.
 *           Cleared automatically on ISR entry, restored by RETI on exit.
 * bits 6..0 T H S V N Z C : arithmetic/branch flags.
 * Critical-section pattern: uint8_t s = SREG; cli(); ...; SREG = s;
 * (restores the previous I-bit instead of blindly calling sei()).
 */
#define SREG _SFR_IO8(0x3F)

#undef sei
/**
 * Set Global Interrupt Enable (SREG bit I = 1). Interrupts can now fire.
 * Also a compiler memory barrier.
 */
#define sei() __asm__ __volatile__ ("sei" ::: "memory")

#undef cli
/**
 * Clear Global Interrupt Enable (SREG bit I = 0). No interrupt can fire until sei().
 * Pending flags stay set and fire once interrupts are re-enabled. Also a memory barrier.
 */
#define cli() __asm__ __volatile__ ("cli" ::: "memory")

#undef ISR
/**
 * Defines an interrupt service routine: ISR(TIMER2_COMPB_vect) { ... }
 * The compiler saves/restores registers and ends with RETI.
 * Runs with interrupts disabled. Keep it short; shared variables must be volatile.
 */
#define ISR(vector,...) void vector (void) __attribute__ ((signal,__INTR_ATTRS)) __VA_ARGS__; void vector (void)

#undef _BV
/**
 * Bit Value: _BV(n) == (1 << n). Turns a bit number into a mask.
 * Example: TWCR = _BV(TWINT) | _BV(TWEN);
 */
#define _BV(bit) (1 << (bit))

#undef TIMER2_COMPB_vect
/**
 * Interrupt vector 8: Timer/Counter2 Compare Match B.
 * Fires when TCNT2 == OCR2B, if TIMSK2.OCIE2B = 1 and interrupts are enabled.
 * This project: the 1 ms tick in timer.cpp (OCR2B += 250; ms_ticks++).
 */
#define TIMER2_COMPB_vect _VECTOR(8)

/* ==========================================================================
 * GPIO port registers
 * ========================================================================== */

#undef DDRB
/**
 * Port B Data Direction Register.
 * Bit n = 1 -> PBn is an OUTPUT. Bit n = 0 -> PBn is an INPUT (reset default).
 * Nano pins: PB0=D8  PB1=D9  PB2=D10  PB3=D11  PB4=D12  PB5=D13 (on-board LED).
 * PB6/PB7 are wired to the 16 MHz crystal and are not usable.
 * Peripherals that drive a pin (timer PWM outputs) still need the DDR bit set to output.
 */
#define DDRB _SFR_IO8(0x04)

#undef PORTB
/**
 * Port B Data Register (the output latch).
 * Output pin: 1 drives the pin HIGH, 0 drives it LOW.
 * Input pin: 1 enables the internal pull-up (~20-50 kOhm), 0 = floating input.
 * Reading PORTB returns what you last wrote, NOT the pin voltage. Use PINB for that.
 */
#define PORTB _SFR_IO8(0x05)

#undef PINB
/**
 * Port B Input Pins register.
 * Reading gives the actual logic level on each pin right now (input or output).
 * Writing a 1 to a bit TOGGLES the corresponding PORTB bit.
 * This project: read_pulse() reads PINB to time the ultrasonic echo on PB0.
 */
#define PINB _SFR_IO8(0x03)

#undef DDRC
/**
 * Port C Data Direction Register (1 = output, 0 = input).
 * Nano pins: PC0..PC5 = A0..A5. PC4 = A4 = SDA, PC5 = A5 = SCL (taken over by TWI when TWEN=1).
 * PC6 is RESET. A6/A7 are analog-only and have no port bits.
 */
#define DDRC _SFR_IO8(0x07)

#undef PORTC
/**
 * Port C Data Register. Output: 1 = high, 0 = low. Input: 1 = pull-up on.
 * PC0..PC5 = A0..A5.
 */
#define PORTC _SFR_IO8(0x08)

#undef PINC
/**
 * Port C Input Pins. Read = actual pin levels. Write 1 = toggle PORTC bit.
 * PC0..PC5 = A0..A5.
 */
#define PINC _SFR_IO8(0x06)

#undef DDRD
/**
 * Port D Data Direction Register (1 = output, 0 = input).
 * Nano pins: PD0..PD7 = D0..D7. PD0 = RX, PD1 = TX (taken over by the UART when RXEN0/TXEN0 are set).
 * PD2 = INT0, PD3 = INT1 / OC2B, PD5 = OC0B, PD6 = OC0A.
 */
#define DDRD _SFR_IO8(0x0A)

#undef PORTD
/**
 * Port D Data Register. Output: 1 = high, 0 = low. Input: 1 = pull-up on.
 * PD0..PD7 = D0..D7.
 */
#define PORTD _SFR_IO8(0x0B)

#undef PIND
/**
 * Port D Input Pins. Read = actual pin levels. Write 1 = toggle PORTD bit.
 * Unused in this project.
 */
#define PIND _SFR_IO8(0x09)

/* ==========================================================================
 * Pin numbers -> Nano labels and alternate functions
 * ========================================================================== */

#undef PB0
/**
 * Port B bit 0 = Nano D8. Also ICP1 (Timer1 input capture pin).
 * This project: US1_ECHO.
 */
#define PB0 PORTB0

#undef PB1
/**
 * Port B bit 1 = Nano D9. Also OC1A (Timer1 compare A PWM output).
 * This project: servo signal (SERVO_PWM).
 */
#define PB1 PORTB1

#undef PB2
/**
 * Port B bit 2 = Nano D10. Also OC1B (Timer1 compare B output) and SPI SS.
 * Unused in this project. As a PWM pin it would share Timer1's 50 Hz servo frame.
 */
#define PB2 PORTB2

#undef PB3
/**
 * Port B bit 3 = Nano D11. Also OC2A (Timer2 compare A PWM output) and SPI MOSI.
 * Unused in this project (Timer2 runs the 1 ms clock, with OC2A disconnected).
 */
#define PB3 PORTB3

#undef PB4
/**
 * Port B bit 4 = Nano D12. Also SPI MISO.
 * This project: US1_TRIG.
 */
#define PB4 PORTB4

#undef PB5
/**
 * Port B bit 5 = Nano D13, the on-board LED. Also SPI SCK.
 * Unused in this project.
 */
#define PB5 PORTB5

#undef PC0
/**
 * Port C bit 0 = Nano A0 = ADC0.
 * This project: IR distance sensor 1 input (IR_SENSOR1_PIN).
 */
#define PC0 PORTC0

#undef PC1
/**
 * Port C bit 1 = Nano A1 = ADC1.
 * This project: IR distance sensor 2 input (IR_SENSOR2_PIN).
 */
#define PC1 PORTC1

#undef PC4
/**
 * Port C bit 4 = Nano A4 = I2C SDA when TWI is enabled.
 * This project: MPU6050 data line.
 */
#define PC4 PORTC4

#undef PC5
/**
 * Port C bit 5 = Nano A5 = I2C SCL when TWI is enabled.
 * This project: MPU6050 clock line.
 */
#define PC5 PORTC5

#undef PD0
/**
 * Port D bit 0 = Nano D0 = UART RXD (USB-serial chip TX goes here).
 */
#define PD0 PORTD0

#undef PD1
/**
 * Port D bit 1 = Nano D1 = UART TXD (goes to the USB-serial chip -> Serial Monitor).
 */
#define PD1 PORTD1

#undef PD2
/**
 * Port D bit 2 = Nano D2. Also INT0 external interrupt.
 * Unused in this project.
 */
#define PD2 PORTD2

#undef PD3
/**
 * Port D bit 3 = Nano D3. Also OC2B (Timer2 compare B output) and INT1.
 * Unused in this project (Timer2 runs the 1 ms clock, with OC2B disconnected).
 */
#define PD3 PORTD3

#undef PD5
/**
 * Port D bit 5 = Nano D5. Also OC0B (Timer0 compare B PWM output).
 * This project: THRUST_FAN_PWM (OC0B, phase-correct PWM at 31.4 kHz).
 */
#define PD5 PORTD5

#undef PD6
/**
 * Port D bit 6 = Nano D6. Also OC0A (Timer0 compare A PWM output).
 * This project: LIFT_FAN_PWM (OC0A, phase-correct PWM at 31.4 kHz).
 */
#define PD6 PORTD6

/* ==========================================================================
 * Timer/Counter0 (8-bit): lift and thrust fan PWM
 * ========================================================================== */

#undef TCCR0A
/**
 * Timer/Counter0 Control Register A.
 * bits 7:6 COM0A1:0 : what pin OC0A (PD6/D6) does on compare match.
 * bits 5:4 COM0B1:0 : what pin OC0B (PD5/D5) does on compare match.
 * bits 1:0 WGM01:0  : waveform mode (low bits; WGM02 is in TCCR0B).
 * Modes WGM02:0: 0 Normal (0..0xFF)  1 Phase-correct PWM  2 CTC (TOP=OCR0A)
 *                3 Fast PWM (TOP=0xFF)  5 Phase-correct (TOP=OCR0A)  7 Fast PWM (TOP=OCR0A)
 * PWM COM bits: 10 non-inverting, 11 inverting, 00 pin disconnected.
 * Phase-correct non-inverting: clear on match counting up, set on match counting down.
 * This project: COM0A1 | COM0B1 | WGM00 -> mode 1, non-inverting PWM on both fan pins
 * (PD6 lift, PD5 thrust). Modes 5/7 would use OCR0A as TOP, leaving only OC0B for a fan.
 */
#define TCCR0A _SFR_IO8(0x24)

#undef TCCR0B
/**
 * Timer/Counter0 Control Register B.
 * bits 7:6 FOC0A/FOC0B : force output compare (non-PWM modes only).
 * bit 3    WGM02       : waveform mode high bit.
 * bits 2:0 CS02:0 clock select: 0 STOPPED  1 clk/1  2 clk/8  3 clk/64  4 clk/256
 *                               5 clk/1024  6 ext T0 falling  7 ext T0 rising
 * Tick at 16 MHz: /8 = 0.5 us, /64 = 4 us, /256 = 16 us, /1024 = 64 us.
 * Phase-correct period = 2*TOP ticks (up 255 steps, down 255 steps); Fast PWM = TOP+1 ticks.
 * This project: CS00 -> /1 -> phase-correct PWM at 16 MHz / (2*255) = 31.4 kHz.
 * Note: Arduino core's millis()/delay() use Timer0. This project doesn't call them.
 */
#define TCCR0B _SFR_IO8(0x25)

#undef TCNT0
/**
 * Timer/Counter0 counter value (8-bit, read/write).
 * Counts up once per prescaled tick. In Normal mode it wraps 255 -> 0 and sets TOV0.
 */
#define TCNT0 _SFR_IO8(0x26)

#undef OCR0A
/**
 * Output Compare Register 0A (8-bit): PWM duty for OC0A (PD6/D6).
 * Phase-correct non-inverting: duty = OCR0A / 255. 0 = solid low, 255 = solid high.
 * (Fast PWM gives (OCR0A + 1) / 256 instead, so 0 still leaves a 1-tick spike.)
 * In modes 5 and 7 it is TOP instead of a duty value.
 * This project: lift fan duty, written by pwm_set_duty(&OCR0A, FAN_PWM_TOP, percent).
 */
#define OCR0A _SFR_IO8(0x27)

#undef OCR0B
/**
 * Output Compare Register 0B (8-bit): PWM duty for OC0B (PD5/D5).
 * Phase-correct non-inverting: duty = OCR0B / 255. 0 = solid low, 255 = solid high.
 * This project: thrust fan duty, written by pwm_set_duty(&OCR0B, FAN_PWM_TOP, percent).
 */
#define OCR0B _SFR_IO8(0x28)

#undef TIMSK0
/**
 * Timer/Counter0 Interrupt Mask Register (1 = interrupt enabled).
 * bit 2 OCIE0B : compare match B -> TIMER0_COMPB_vect
 * bit 1 OCIE0A : compare match A -> TIMER0_COMPA_vect
 * bit 0 TOIE0  : overflow        -> TIMER0_OVF_vect
 * This project: none (TIMSK0 = 0). Timer0 only generates PWM.
 */
#define TIMSK0 _SFR_MEM8(0x6E)

#undef TIFR0
/**
 * Timer/Counter0 Interrupt Flag Register.
 * bit 2 OCF0B : compare B matched   bit 1 OCF0A : compare A matched   bit 0 TOV0 : overflowed
 * Hardware sets the flags. Cleared automatically when the ISR runs, or by WRITING A 1:
 *    TIFR0 = (1 << TOV0);    // clears only TOV0
 * Don't use |= : it reads every set flag back as 1 and clears all of them.
 */
#define TIFR0 _SFR_IO8(0x15)

#undef COM0A1
/**
 * TCCR0A bit 7. COM0A1:0 = 10 -> non-inverting PWM on OC0A (PD6/D6), the lift fan.
 */
#define COM0A1 7

#undef COM0A0
/**
 * TCCR0A bit 6. With COM0A1 selects OC0A (PD6) pin behaviour.
 */
#define COM0A0 6

#undef COM0B1
/**
 * TCCR0A bit 5. COM0B1:0 = 10 -> non-inverting PWM on OC0B (PD5/D5), the thrust fan.
 */
#define COM0B1 5

#undef COM0B0
/**
 * TCCR0A bit 4. With COM0B1 selects OC0B (PD5) pin behaviour.
 */
#define COM0B0 4

#undef WGM01
/**
 * TCCR0A bit 1. Waveform Generation Mode bit 1 (see TCCR0A).
 */
#define WGM01 1

#undef WGM00
/**
 * TCCR0A bit 0. Waveform Generation Mode bit 0. WGM02:0 = 001 -> Phase-correct PWM, TOP 0xFF (this project).
 */
#define WGM00 0

#undef WGM02
/**
 * TCCR0B bit 3. Waveform Generation Mode bit 2 (see TCCR0A).
 */
#define WGM02 3

#undef CS02
/**
 * TCCR0B bit 2. Clock Select bit 2. CS02:0 = 100 -> clk/256 (16 us/tick).
 */
#define CS02 2

#undef CS01
/**
 * TCCR0B bit 1. Clock Select bit 1. CS02:0 = 010 -> clk/8, 011 -> clk/64.
 */
#define CS01 1

#undef CS00
/**
 * TCCR0B bit 0. Clock Select bit 0. CS02:0 = 001 -> clk/1 (this project), 011 -> clk/64.
 */
#define CS00 0

#undef OCIE0B
/**
 * TIMSK0 bit 2. Enable the Timer0 Compare Match B interrupt (TIMER0_COMPB_vect).
 */
#define OCIE0B 2

#undef OCIE0A
/**
 * TIMSK0 bit 1. Enable the Timer0 Compare Match A interrupt (TIMER0_COMPA_vect).
 */
#define OCIE0A 1

#undef TOIE0
/**
 * TIMSK0 bit 0. Enable the Timer0 Overflow interrupt (TIMER0_OVF_vect).
 */
#define TOIE0 0

#undef OCF0B
/**
 * TIFR0 bit 2. Set when TCNT0 == OCR0B. Clear by writing 1.
 */
#define OCF0B 2

#undef OCF0A
/**
 * TIFR0 bit 1. Set when TCNT0 == OCR0A. Clear by writing 1.
 */
#define OCF0A 1

#undef TOV0
/**
 * TIFR0 bit 0. Set when TCNT0 overflows (255 -> 0). Clear by writing 1.
 */
#define TOV0 0

/* ==========================================================================
 * Timer/Counter1 (16-bit): servo PWM on D9
 * ========================================================================== */

#undef TCCR1A
/**
 * Timer/Counter1 Control Register A.
 * bits 7:6 COM1A1:0 : OC1A pin (PB1/D9) behaviour.   bits 5:4 COM1B1:0 : OC1B pin (PB2/D10).
 * bits 1:0 WGM11:10 : waveform mode low bits (WGM13:12 are in TCCR1B).
 * In PWM modes: COM = 10 non-inverting (set at BOTTOM, clear on match), 11 inverting, 00 pin disconnected.
 * This project: COM1A1 | WGM11 -> non-inverting servo pulse on D9 (mode 14 with TCCR1B).
 */
#define TCCR1A _SFR_MEM8(0x80)

#undef TCCR1B
/**
 * Timer/Counter1 Control Register B.
 * bit 7    ICNC1    : input capture noise canceller.
 * bit 6    ICES1    : input capture edge (1 = rising).
 * bits 4:3 WGM13:12 : waveform mode high bits.
 * bits 2:0 CS12:10  : clock select, same table as Timer0 (1=/1 2=/8 3=/64 4=/256 5=/1024, 0=stopped).
 * Mode 14 (WGM13:0 = 1110) = Fast PWM with TOP = ICR1.
 * This project: mode 14, /8 -> 0.5 us per tick.
 */
#define TCCR1B _SFR_MEM8(0x81)

#undef TCNT1
/**
 * Timer/Counter1 counter value (16-bit).
 * 16-bit access goes through a shared TEMP register; the compiler orders the bytes correctly,
 * but it is not atomic if an ISR also touches 16-bit Timer1 registers.
 */
#define TCNT1 _SFR_MEM16(0x84)

#undef OCR1A
/**
 * Output Compare Register 1A (16-bit).
 * In PWM modes it is double-buffered: a new value takes effect at BOTTOM, so pulses never glitch.
 * This project: servo pulse width in 0.5 us ticks. SERVO_PWM_CENTER (3000) = 1.5 ms = centre.
 * servo_set_angle() writes SERVO_PWM_CENTER + 20*angle (10 us per degree, clamped to +/-80).
 */
#define OCR1A _SFR_MEM16(0x88)

#undef OCR1B
/**
 * Output Compare Register 1B (16-bit). Drives OC1B (PB2/D10). Unused in this project.
 */
#define OCR1B _SFR_MEM16(0x8A)

#undef ICR1
/**
 * Input Capture Register 1 (16-bit).
 * Normally latches TCNT1 on an edge at ICP1 (PB0/D8), which is useful for timing pulses.
 * In WGM modes 8, 10, 12 and 14 it sets the TOP value instead.
 * This project: TOP = SERVO_PWM_TOP (39999) -> period 40000 * 0.5 us = 20 ms (50 Hz servo frame).
 * Because of that, input capture can't time the US1 echo on PB0.
 */
#define ICR1 _SFR_MEM16(0x86)

#undef TIMSK1
/**
 * Timer/Counter1 Interrupt Mask: bit 5 ICIE1 (capture), bit 2 OCIE1B, bit 1 OCIE1A, bit 0 TOIE1.
 * None enabled in this project.
 */
#define TIMSK1 _SFR_MEM8(0x6F)

#undef TIFR1
/**
 * Timer/Counter1 Interrupt Flags: bit 5 ICF1, bit 2 OCF1B, bit 1 OCF1A, bit 0 TOV1.
 * Clear a flag by writing 1 to it.
 */
#define TIFR1 _SFR_IO8(0x16)

#undef COM1A1
/**
 * TCCR1A bit 7. COM1A1:0 = 10 -> non-inverting PWM on OC1A (PB1/D9).
 */
#define COM1A1 7

#undef COM1A0
/**
 * TCCR1A bit 6. COM1A1:0 = 11 -> inverting PWM on OC1A (PB1/D9).
 */
#define COM1A0 6

#undef COM1B1
/**
 * TCCR1A bit 5. COM1B1:0 controls OC1B (PB2/D10).
 */
#define COM1B1 5

#undef COM1B0
/**
 * TCCR1A bit 4. COM1B1:0 controls OC1B (PB2/D10).
 */
#define COM1B0 4

#undef WGM11
/**
 * TCCR1A bit 1. Waveform mode bit 1. Mode 14 = WGM13:0 1110.
 */
#define WGM11 1

#undef WGM10
/**
 * TCCR1A bit 0. Waveform mode bit 0.
 */
#define WGM10 0

#undef WGM13
/**
 * TCCR1B bit 4. Waveform mode bit 3. Mode 14 = WGM13:0 1110.
 */
#define WGM13 4

#undef WGM12
/**
 * TCCR1B bit 3. Waveform mode bit 2. Mode 14 = WGM13:0 1110.
 */
#define WGM12 3

#undef CS12
/**
 * TCCR1B bit 2. Clock Select bit 2.
 */
#define CS12 2

#undef CS11
/**
 * TCCR1B bit 1. Clock Select bit 1. CS12:10 = 010 -> clk/8 (0.5 us/tick).
 */
#define CS11 1

#undef CS10
/**
 * TCCR1B bit 0. Clock Select bit 0.
 */
#define CS10 0

#undef ICNC1
/**
 * TCCR1B bit 7. Input Capture Noise Canceller: needs 4 equal samples before accepting an edge.
 */
#define ICNC1 7

#undef ICES1
/**
 * TCCR1B bit 6. Input Capture Edge Select: 1 = rising edge, 0 = falling edge.
 */
#define ICES1 6

/* ==========================================================================
 * Timer/Counter2 (8-bit): 1 ms system clock (timer_millis / timer_micros)
 * ========================================================================== */

#undef TCCR2A
/**
 * Timer/Counter2 Control Register A.
 * bits 7:6 COM2A1:0 : OC2A pin (PB3/D11) behaviour.   bits 5:4 COM2B1:0 : OC2B pin (PD3/D3).
 * bits 1:0 WGM21:20 : waveform mode low bits (WGM22 in TCCR2B).
 * Modes WGM22:0: 0 Normal (0..0xFF)  1 Phase-correct PWM (TOP=0xFF)  2 CTC (TOP=OCR2A)
 *                3 Fast PWM (TOP=0xFF)  5 Phase-correct (TOP=OCR2A)  7 Fast PWM (TOP=OCR2A)
 * This project: TCCR2A = 0 -> Normal mode, pins disconnected.
 */
#define TCCR2A _SFR_MEM8(0xB0)

#undef TCCR2B
/**
 * Timer/Counter2 Control Register B.
 * bit 3    WGM22  : waveform mode high bit.
 * bits 2:0 CS22:20: clock select. NOTE: Timer2's table differs from Timer0/1:
 *    0 stopped  1 /1  2 /8  3 /32  4 /64  5 /128  6 /256  7 /1024   (no external clock)
 * This project: CS22 -> /64 -> 4 us per tick for the ms clock.
 */
#define TCCR2B _SFR_MEM8(0xB1)

#undef TCNT2
/**
 * Timer/Counter2 counter value (8-bit). In Normal mode it wraps 255 -> 0 and sets TOV2.
 * This project: timer_micros() reads it to get the ticks since the last OCR2B match.
 */
#define TCNT2 _SFR_MEM8(0xB2)

#undef OCR2A
/**
 * Output Compare Register 2A (8-bit). Compared with TCNT2 every tick.
 * In modes 2, 5 and 7 it is TOP instead of a compare value.
 * Unused in this project.
 */
#define OCR2A _SFR_MEM8(0xB3)

#undef OCR2B
/**
 * Output Compare Register 2B (8-bit). Compared with TCNT2 every tick.
 * On match: sets OCF2B, fires TIMER2_COMPB_vect if OCIE2B = 1, can drive OC2B (PD3).
 * This project: the ISR does OCR2B += 250. Being 8-bit, that is -6 mod 256,
 * so the next match is exactly 250 ticks (1.000 ms) later.
 */
#define OCR2B _SFR_MEM8(0xB4)

#undef TIMSK2
/**
 * Timer/Counter2 Interrupt Mask Register (1 = interrupt enabled).
 * bit 2 OCIE2B : compare match B -> TIMER2_COMPB_vect
 * bit 1 OCIE2A : compare match A -> TIMER2_COMPA_vect
 * bit 0 TOIE2  : overflow        -> TIMER2_OVF_vect
 * This project: only OCIE2B.
 */
#define TIMSK2 _SFR_MEM8(0x70)

#undef TIFR2
/**
 * Timer/Counter2 Interrupt Flag Register.
 * bit 2 OCF2B : compare B matched   bit 1 OCF2A : compare A matched   bit 0 TOV2 : overflowed
 * Hardware sets the flags. Cleared automatically when the ISR runs, or by WRITING A 1.
 * Don't use |= : it reads every set flag back as 1 and clears all of them.
 * This project: timer_micros() checks OCF2B to catch a match whose ISR hasn't run yet.
 */
#define TIFR2 _SFR_IO8(0x17)

#undef COM2A1
/**
 * TCCR2A bit 7. With COM2A0 selects OC2A (PB3/D11) pin behaviour.
 */
#define COM2A1 7

#undef COM2A0
/**
 * TCCR2A bit 6. With COM2A1 selects OC2A (PB3/D11) pin behaviour.
 */
#define COM2A0 6

#undef COM2B1
/**
 * TCCR2A bit 5. With COM2B0 selects OC2B (PD3/D3) pin behaviour.
 */
#define COM2B1 5

#undef COM2B0
/**
 * TCCR2A bit 4. With COM2B1 selects OC2B (PD3/D3) pin behaviour.
 */
#define COM2B0 4

#undef WGM21
/**
 * TCCR2A bit 1. Waveform mode bit 1 (see TCCR2A).
 */
#define WGM21 1

#undef WGM20
/**
 * TCCR2A bit 0. Waveform mode bit 0 (see TCCR2A).
 */
#define WGM20 0

#undef WGM22
/**
 * TCCR2B bit 3. Waveform mode bit 2.
 */
#define WGM22 3

#undef CS22
/**
 * TCCR2B bit 2. Clock Select bit 2. CS22:20 = 100 -> clk/64 (this project, 4 us/tick).
 * Timer2 table: 4 = /64, 5 = /128, 6 = /256, 7 = /1024.
 */
#define CS22 2

#undef CS21
/**
 * TCCR2B bit 1. Clock Select bit 1. CS22:20 = 010 -> clk/8.
 */
#define CS21 1

#undef CS20
/**
 * TCCR2B bit 0. Clock Select bit 0. CS22:20 = 001 -> clk/1, 011 -> clk/32.
 */
#define CS20 0

#undef OCIE2B
/**
 * TIMSK2 bit 2. Enable the Timer2 Compare Match B interrupt (TIMER2_COMPB_vect).
 */
#define OCIE2B 2

#undef OCIE2A
/**
 * TIMSK2 bit 1. Enable the Timer2 Compare Match A interrupt (TIMER2_COMPA_vect).
 */
#define OCIE2A 1

#undef TOIE2
/**
 * TIMSK2 bit 0. Enable the Timer2 Overflow interrupt (TIMER2_OVF_vect).
 */
#define TOIE2 0

#undef OCF2B
/**
 * TIFR2 bit 2. Set when TCNT2 == OCR2B. Clear by writing 1.
 */
#define OCF2B 2

#undef OCF2A
/**
 * TIFR2 bit 1. Set when TCNT2 == OCR2A. Clear by writing 1.
 */
#define OCF2A 1

#undef TOV2
/**
 * TIFR2 bit 0. Set when TCNT2 overflows (255 -> 0). Clear by writing 1.
 */
#define TOV2 0

/* ==========================================================================
 * USART0: serial to the PC
 * ========================================================================== */

#undef UBRR0H
/**
 * USART0 Baud Rate Register HIGH (bits 11:8 of UBRR0).
 * Write it BEFORE UBRR0L: writing UBRR0L updates the baud generator immediately.
 */
#define UBRR0H _SFR_MEM8(0xC5)

#undef UBRR0L
/**
 * USART0 Baud Rate Register LOW (bits 7:0 of UBRR0).
 * baud = F_CPU / (16 * (UBRR0 + 1))   ->   UBRR0 = F_CPU / (16 * baud) - 1   (U2X0 = 0)
 * 9600 at 16 MHz -> 103 -> actual 9615 baud (+0.16 %). Under ~2 % error is fine.
 */
#define UBRR0L _SFR_MEM8(0xC4)

#undef UBRR0
/**
 * USART0 Baud Rate Register as one 16-bit access (12 bits used).
 * UBRR0 = F_CPU / (16 * baud) - 1.
 */
#define UBRR0 _SFR_MEM16(0xC4)

#undef UCSR0A
/**
 * USART0 Control and Status Register A (mostly status flags).
 * bit 7 RXC0  : receive complete, an unread byte is waiting in UDR0.
 * bit 6 TXC0  : transmit complete, the last frame fully left and nothing is queued. Clear by writing 1.
 * bit 5 UDRE0 : data register empty, UDR0 can take the next byte (1 after reset).
 * bit 4 FE0   : frame error, stop bit read as 0 (usually a baud mismatch).
 * bit 3 DOR0  : data overrun, a received byte was lost because UDR0 wasn't read in time.
 * bit 2 UPE0  : parity error.
 * bit 1 U2X0  : double-speed mode (divisor 8 instead of 16).
 * bit 0 MPCM0 : multi-processor communication mode.
 * Read FE0/DOR0/UPE0 BEFORE reading UDR0.
 */
#define UCSR0A _SFR_MEM8(0xC0)

#undef UCSR0B
/**
 * USART0 Control and Status Register B (enables).
 * bit 7 RXCIE0 : interrupt on RXC0      -> USART_RX_vect
 * bit 6 TXCIE0 : interrupt on TXC0      -> USART_TX_vect
 * bit 5 UDRIE0 : interrupt while UDRE0  -> USART_UDRE_vect (keeps firing while the buffer is empty)
 * bit 4 RXEN0  : enable receiver, takes over PD0/RX.
 * bit 3 TXEN0  : enable transmitter, takes over PD1/TX.
 * bit 2 UCSZ02 : character size bit 2 (see UCSR0C).
 * bit 1 RXB80 / bit 0 TXB80 : 9th data bit in 9-bit mode.
 * This project: RXEN0 | TXEN0, no interrupts (polled).
 */
#define UCSR0B _SFR_MEM8(0xC1)

#undef UCSR0C
/**
 * USART0 Control and Status Register C (frame format).
 * bits 7:6 UMSEL01:00 : mode. 00 asynchronous UART, 01 synchronous, 11 master SPI.
 * bits 5:4 UPM01:00   : parity. 00 none, 10 even, 11 odd.
 * bit 3    USBS0      : stop bits. 0 = 1 bit, 1 = 2 bits.
 * bits 2:1 UCSZ01:00  : with UCSZ02 -> 000=5 001=6 010=7 011=8 111=9 data bits.
 * bit 0    UCPOL0     : clock polarity (synchronous mode only).
 * This project: UCSZ01 | UCSZ00 -> 8N1 (also the reset value, 0x06).
 */
#define UCSR0C _SFR_MEM8(0xC2)

#undef UDR0
/**
 * USART0 I/O Data Register. Two separate hardware buffers share one address:
 * WRITE -> transmit buffer. Only write when UCSR0A.UDRE0 = 1 (otherwise the byte is ignored).
 * READ  -> oldest byte from the 2-byte receive FIFO (clears RXC0 once empty).
 * Each 8N1 byte at 9600 baud takes 1.04 ms to send.
 */
#define UDR0 _SFR_MEM8(0xC6)

#undef RXC0
/**
 * UCSR0A bit 7. USART Receive Complete: an unread byte is in UDR0. Cleared by reading UDR0.
 */
#define RXC0 7

#undef TXC0
/**
 * UCSR0A bit 6. USART Transmit Complete: shift register empty and no new data. Clear by writing 1.
 */
#define TXC0 6

#undef UDRE0
/**
 * UCSR0A bit 5. USART Data Register Empty: UDR0 can accept a new byte.
 * Goes high again as soon as a byte moves into the shift register, so you can queue the next
 * byte while the current one is still being sent.
 */
#define UDRE0 5

#undef FE0
/**
 * UCSR0A bit 4. Frame Error: the received stop bit was 0. Usually a baud-rate mismatch.
 */
#define FE0 4

#undef DOR0
/**
 * UCSR0A bit 3. Data OverRun: receive FIFO full and a new start bit arrived, so a byte was lost.
 */
#define DOR0 3

#undef UPE0
/**
 * UCSR0A bit 2. USART Parity Error in the received byte (only if parity is enabled).
 */
#define UPE0 2

#undef U2X0
/**
 * UCSR0A bit 1. Double USART speed: baud divisor becomes 8 instead of 16.
 */
#define U2X0 1

#undef MPCM0
/**
 * UCSR0A bit 0. Multi-processor Communication Mode: ignore frames that aren't address frames.
 */
#define MPCM0 0

#undef RXCIE0
/**
 * UCSR0B bit 7. Enable the RX Complete interrupt (USART_RX_vect).
 */
#define RXCIE0 7

#undef TXCIE0
/**
 * UCSR0B bit 6. Enable the TX Complete interrupt (USART_TX_vect).
 */
#define TXCIE0 6

#undef UDRIE0
/**
 * UCSR0B bit 5. Enable the Data Register Empty interrupt (USART_UDRE_vect).
 * The usual way to send a buffer in the background.
 */
#define UDRIE0 5

#undef RXEN0
/**
 * UCSR0B bit 4. Receiver Enable: PD0 becomes the UART RX input.
 */
#define RXEN0 4

#undef TXEN0
/**
 * UCSR0B bit 3. Transmitter Enable: PD1 becomes the UART TX output.
 */
#define TXEN0 3

#undef UCSZ02
/**
 * UCSR0B bit 2. Character Size bit 2 (only set for 9-bit frames).
 */
#define UCSZ02 2

#undef UMSEL01
/**
 * UCSR0C bit 7. USART Mode Select bit 1 (00 = asynchronous).
 */
#define UMSEL01 7

#undef UMSEL00
/**
 * UCSR0C bit 6. USART Mode Select bit 0 (00 = asynchronous).
 */
#define UMSEL00 6

#undef UPM01
/**
 * UCSR0C bit 5. Parity Mode bit 1 (10 = even, 11 = odd).
 */
#define UPM01 5

#undef UPM00
/**
 * UCSR0C bit 4. Parity Mode bit 0.
 */
#define UPM00 4

#undef USBS0
/**
 * UCSR0C bit 3. Stop Bit Select: 0 = 1 stop bit, 1 = 2 stop bits.
 */
#define USBS0 3

#undef UCSZ01
/**
 * UCSR0C bit 2. Character Size bit 1. UCSZ02:0 = 011 -> 8 data bits.
 */
#define UCSZ01 2

#undef UCSZ00
/**
 * UCSR0C bit 1. Character Size bit 0. UCSZ02:0 = 011 -> 8 data bits.
 */
#define UCSZ00 1

#undef UCPOL0
/**
 * UCSR0C bit 0. Clock Polarity (synchronous mode only). Leave 0 for a normal UART.
 */
#define UCPOL0 0

/* ==========================================================================
 * TWI / I2C: MPU6050 bus (+ util/twi.h status codes)
 * ========================================================================== */

#undef TWBR
/**
 * TWI (I2C) Bit Rate Register.
 * SCL = F_CPU / (16 + 2 * TWBR * prescaler), prescaler = 1/4/16/64 from TWSR.TWPS.
 * At 16 MHz with prescaler 1: TWBR = 72 -> 100 kHz, TWBR = 12 -> 400 kHz.
 * This project: 72 (100 kHz).
 */
#define TWBR _SFR_MEM8(0xB8)

#undef TWCR
/**
 * TWI Control Register. Each write starts the next bus action.
 * bit 7 TWINT : set by hardware when the current action is done (SCL held low meanwhile).
 *               WRITE 1 to clear it and start the next action.
 * bit 6 TWEA  : send ACK after a received byte (0 = send NACK).
 * bit 5 TWSTA : send a START (or repeated START).
 * bit 4 TWSTO : send a STOP. Hardware clears it once the STOP has gone out.
 * bit 3 TWWC  : write collision, TWDR was written while TWINT = 0.
 * bit 2 TWEN  : enable TWI. Takes over PC4/SDA and PC5/SCL.
 * bit 0 TWIE  : TWI interrupt enable (TWI_vect).
 * Typical: TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTA);  // send START
 */
#define TWCR _SFR_MEM8(0xBC)

#undef TWSR
/**
 * TWI Status Register.
 * bits 7:3 TWS7:3 : status code of the last bus event. Read as (TWSR & 0xF8) or TW_STATUS.
 * bits 1:0 TWPS1:0: bit-rate prescaler. 0 = 1, 1 = 4, 2 = 16, 3 = 64.
 * Common codes: 0x08 START  0x10 REP START  0x18/0x20 SLA+W ACK/NACK
 * 0x28/0x30 data sent ACK/NACK  0x40/0x48 SLA+R ACK/NACK  0x50/0x58 data received ACK/NACK.
 */
#define TWSR _SFR_MEM8(0xB9)

#undef TWDR
/**
 * TWI Data Register.
 * Transmit: write the byte (address+R/W, register number, or data) BEFORE clearing TWINT.
 * Receive : after TWINT sets, holds the byte that just arrived.
 * Address byte = (7-bit address << 1) | R/W. MPU6050 at 0x68 -> 0xD0 write, 0xD1 read.
 */
#define TWDR _SFR_MEM8(0xBB)

#undef TWAR
/**
 * TWI (Slave) Address Register: this chip's own address when acting as an I2C slave.
 * Unused here, since the AVR is always the master.
 */
#define TWAR _SFR_MEM8(0xBA)

#undef TWINT
/**
 * TWCR bit 7. TWI Interrupt Flag.
 * Hardware sets it when an action finishes and stretches SCL low until you respond.
 * Clear it by WRITING 1, which also starts the action described by the other TWCR bits.
 * wait_for_twi() polls this bit.
 */
#define TWINT 7

#undef TWEA
/**
 * TWCR bit 6. TWI Enable Acknowledge: 1 = ACK the next received byte ("send more"),
 * 0 = NACK it ("that was the last byte").
 */
#define TWEA 6

#undef TWSTA
/**
 * TWCR bit 5. TWI START condition: SDA falls while SCL is high. Becomes a repeated START if the bus is already ours.
 * You must clear it yourself in the next TWCR write.
 */
#define TWSTA 5

#undef TWSTO
/**
 * TWCR bit 4. TWI STOP condition: SDA rises while SCL is high, releasing the bus.
 * Hardware clears it once the STOP has been sent.
 */
#define TWSTO 4

#undef TWWC
/**
 * TWCR bit 3. TWI Write Collision: TWDR was written while TWINT was 0 (the write is ignored).
 */
#define TWWC 3

#undef TWEN
/**
 * TWCR bit 2. TWI Enable: the hardware takes over SDA/SCL (PC4/PC5) with slew-rate limiting and a spike filter.
 */
#define TWEN 2

#undef TWIE
/**
 * TWCR bit 0. TWI Interrupt Enable: fire TWI_vect while TWINT = 1. Unused (polled).
 */
#define TWIE 0

#undef TWPS1
/**
 * TWSR bit 1. TWI prescaler bit 1 (TWPS1:0 -> 1, 4, 16, 64).
 */
#define TWPS1 1

#undef TWPS0
/**
 * TWSR bit 0. TWI prescaler bit 0 (TWPS1:0 -> 1, 4, 16, 64).
 */
#define TWPS0 0

#undef TW_STATUS
/**
 * util/twi.h: the TWI status code with the prescaler bits masked off: (TWSR & 0xF8).
 */
#define TW_STATUS (TWSR & TW_STATUS_MASK)

#undef TW_STATUS_MASK
/**
 * util/twi.h: 0xF8, the mask for the TWS7:3 status bits in TWSR.
 */
#define TW_STATUS_MASK (_BV(TWS7)|_BV(TWS6)|_BV(TWS5)|_BV(TWS4)| _BV(TWS3))

#undef TW_READ
/**
 * util/twi.h: 1. R/W bit for a read. Address byte = (addr << 1) | TW_READ.
 */
#define TW_READ 1

#undef TW_WRITE
/**
 * util/twi.h: 0. R/W bit for a write. Address byte = (addr << 1) | TW_WRITE.
 */
#define TW_WRITE 0

#undef TW_START
/**
 * TWI status 0x08: a START condition has been sent. Next: load SLA+R/W into TWDR.
 */
#define TW_START 0x08

#undef TW_REP_START
/**
 * TWI status 0x10: a repeated START has been sent. Used to switch from writing to reading without a STOP.
 */
#define TW_REP_START 0x10

#undef TW_MT_SLA_ACK
/**
 * TWI status 0x18 (master transmit): SLA+W sent and the slave ACKed. It's there and listening.
 */
#define TW_MT_SLA_ACK 0x18

#undef TW_MT_SLA_NACK
/**
 * TWI status 0x20 (master transmit): SLA+W sent, no ACK.
 * Wrong address, device unpowered or disconnected, or missing pull-ups.
 */
#define TW_MT_SLA_NACK 0x20

#undef TW_MT_DATA_ACK
/**
 * TWI status 0x28 (master transmit): data byte sent and the slave ACKed.
 */
#define TW_MT_DATA_ACK 0x28

#undef TW_MT_DATA_NACK
/**
 * TWI status 0x30 (master transmit): data byte sent and the slave NACKed (refused it).
 */
#define TW_MT_DATA_NACK 0x30

#undef TW_MT_ARB_LOST
/**
 * TWI status 0x38: arbitration lost, another master drove the bus.
 * Shouldn't happen with a single master.
 */
#define TW_MT_ARB_LOST 0x38

#undef TW_MR_SLA_ACK
/**
 * TWI status 0x40 (master receive): SLA+R sent and the slave ACKed. It's about to send data.
 */
#define TW_MR_SLA_ACK 0x40

#undef TW_MR_SLA_NACK
/**
 * TWI status 0x48 (master receive): SLA+R sent, no ACK.
 */
#define TW_MR_SLA_NACK 0x48

#undef TW_MR_DATA_ACK
/**
 * TWI status 0x50 (master receive): byte received and we ACKed it (asked for more).
 */
#define TW_MR_DATA_ACK 0x50

#undef TW_MR_DATA_NACK
/**
 * TWI status 0x58 (master receive): byte received and we NACKed it (last byte). Send STOP next.
 */
#define TW_MR_DATA_NACK 0x58

/* ==========================================================================
 * ADC: IR sensor
 * ========================================================================== */

#undef ADMUX
/**
 * ADC Multiplexer Selection Register.
 * bits 7:6 REFS1:0 : reference. 00 external AREF pin, 01 AVcc (5 V, cap on AREF), 11 internal 1.1 V.
 * bit 5    ADLAR   : left-adjust the result (then read only ADCH for 8 bits).
 * bits 3:0 MUX3:0  : input channel. 0..7 = ADC0..ADC7 (A0..A7), 1000 temp sensor, 1110 1.1 V, 1111 GND.
 * This project: adc_read() writes ADMUX = pin -> REFS = 00 (external AREF), channel ADC0 or ADC1.
 */
#define ADMUX _SFR_MEM8(0x7C)

#undef ADCSRA
/**
 * ADC Control and Status Register A.
 * bit 7 ADEN  : enable the ADC.
 * bit 6 ADSC  : write 1 to start a conversion. Reads 1 until it finishes.
 * bit 5 ADATE : auto-trigger (source chosen in ADCSRB).
 * bit 4 ADIF  : conversion complete flag. Clear by writing 1.
 * bit 3 ADIE  : interrupt on completion (ADC_vect).
 * bits 2:0 ADPS2:0 : prescaler. 000/001=/2 010=/4 011=/8 100=/16 101=/32 110=/64 111=/128.
 * Full 10-bit accuracy needs a 50-200 kHz ADC clock. 16 MHz / 128 = 125 kHz, about 104 us per conversion.
 */
#define ADCSRA _SFR_MEM8(0x7A)

#undef ADCSRB
/**
 * ADC Control and Status Register B: bits 2:0 ADTS auto-trigger source, bit 6 ACME (analog comparator mux).
 */
#define ADCSRB _SFR_MEM8(0x7B)

#undef ADCL
/**
 * ADC result LOW byte. Read ADCL FIRST: that locks the result until ADCH is read.
 */
#define ADCL _SFR_MEM8(0x78)

#undef ADCH
/**
 * ADC result HIGH byte (bits 9:8 when right-adjusted). Read after ADCL.
 */
#define ADCH _SFR_MEM8(0x79)

#undef ADC
/**
 * ADC result as one 16-bit read (0..1023). The compiler reads ADCL then ADCH in the right order.
 * voltage = ADC * Vref / 1023 (strictly Vref/1024).
 */
#define ADC _SFR_MEM16(0x78)

#undef DIDR0
/**
 * Digital Input Disable Register 0. Bit n = 1 turns off the digital input buffer on ADCn,
 * saving power and noise on analog pins. This project: ADC0D and ADC1D (A0, A1).
 */
#define DIDR0 _SFR_MEM8(0x7E)

#undef REFS1
/**
 * ADMUX bit 7. Reference select bit 1 (REFS1:0 = 11 -> internal 1.1 V).
 */
#define REFS1 7

#undef REFS0
/**
 * ADMUX bit 6. Reference select bit 0 (REFS1:0 = 01 -> AVcc).
 */
#define REFS0 6

#undef ADLAR
/**
 * ADMUX bit 5. ADC Left Adjust Result.
 */
#define ADLAR 5

#undef ADEN
/**
 * ADCSRA bit 7. ADC Enable.
 */
#define ADEN 7

#undef ADSC
/**
 * ADCSRA bit 6. ADC Start Conversion: write 1 to start. Hardware clears it when the result is ready.
 */
#define ADSC 6

#undef ADATE
/**
 * ADCSRA bit 5. ADC Auto Trigger Enable.
 */
#define ADATE 5

#undef ADIF
/**
 * ADCSRA bit 4. ADC Interrupt Flag: conversion complete. Clear by writing 1.
 */
#define ADIF 4

#undef ADIE
/**
 * ADCSRA bit 3. ADC Interrupt Enable (ADC_vect).
 */
#define ADIE 3

#undef ADPS2
/**
 * ADCSRA bit 2. ADC prescaler bit 2. ADPS2:0 = 111 -> /128.
 */
#define ADPS2 2

#undef ADPS1
/**
 * ADCSRA bit 1. ADC prescaler bit 1.
 */
#define ADPS1 1

#undef ADPS0
/**
 * ADCSRA bit 0. ADC prescaler bit 0.
 */
#define ADPS0 0

#undef ADC0D
/**
 * DIDR0 bit 0. Disable the digital input buffer on ADC0 (A0).
 */
#define ADC0D 0

#undef ADC1D
/**
 * DIDR0 bit 1. Disable the digital input buffer on ADC1 (A1).
 */
#define ADC1D 1

#endif /* AVR_REFERENCE_H */
