/********************************************************************************
 * @brief 
 ********************************************************************************/
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <gpiod_utils.h>
#include <semaphore.h>

#define SEMAPHORE_ID_PRINT 0 /* Semaphore used for print. */

//skapa en strukt som innehåller en GPIOD_line pekare till en led och en knapp samt en fördröjningstid i ms 

struct thread_args 
{
    struct gpiod_line* led_a;
    struct gpiod_line* led_b;
    struct gpiod_line* button; 
    uint16_t blink_speed_ms; 
    bool leds_enabled;
    uint8_t last_input_value;
};


/********************************************************************************
 * Runs thread, när en led blinkar vid nedtryck av knapp
 * arg: Referens till argument som innehåller alla trådparametrar 
 * return: en nullpekare , eftersom vi måste retunera något. 
 
 När pthread skapades, så vill de kunna retunera vadd som helst och skicka vad som helst. 
 Därför kom de på att man kan ha en void pekare, då kan man skicka en pekare till vad som helst. 

 ********************************************************************************/

void* run_thread(void* arg) 
{
    struct thread_args* self = (struct thread_args*)(arg);
    while (1) 
    {
        if (gpiod_line_event_detected(self->button, GPIOD_LINE_EDGE_RISING, &self->last_input_value))
        {
            self->leds_enabled = !self->leds_enabled;
            semaphore_reserve(SEMAPHORE_ID_PRINT);

            if(self->leds_enabled)
            {
                printf("LEDs connected to pins %hu %hu are active \n", 
                        gpiod_line_offset(self->led_a), 
                        gpiod_line_offset(self->led_b));
            }
            else
            {
                printf("LEDs connected to pins %hu %hu are inactive \n", 
                        gpiod_line_offset(self->led_a), 
                        gpiod_line_offset(self->led_b));
            }        
            delay_ms(10);
            semaphore_release(SEMAPHORE_ID_PRINT);
        }

        if (self->leds_enabled)
        {
            gpiod_line_set_value(self->led_a, 1);
            gpiod_line_set_value(self->led_b, 0);
            delay_ms(self->blink_speed_ms);
            gpiod_line_set_value(self->led_a, 0);
            gpiod_line_set_value(self->led_b, 1);
            delay_ms(self->blink_speed_ms);
        } 
        else 
        { 
            gpiod_line_set_value(self->led_a, 0);
            gpiod_line_set_value(self->led_b, 0);
        }
    }
    return 0; 
}


int main()
{
    struct gpiod_line* led1 = gpiod_line_new(17, GPIOD_LINE_DIRECTION_OUT);     //Tråd1 
    struct gpiod_line* led2 = gpiod_line_new(22, GPIOD_LINE_DIRECTION_OUT);     //Tråd1 
    struct gpiod_line* led3 = gpiod_line_new(23, GPIOD_LINE_DIRECTION_OUT);     //Tråd2
    struct gpiod_line* led4 = gpiod_line_new(24, GPIOD_LINE_DIRECTION_OUT);     //Tråd2 

    struct gpiod_line* button1 = gpiod_line_new(27, GPIOD_LINE_DIRECTION_IN); 
    struct gpiod_line* button2 = gpiod_line_new(25, GPIOD_LINE_DIRECTION_IN); 

    struct thread_args args1 = { led1, led2, button1, 100, false, 0 };
    struct thread_args args2 = { led3, led4, button2, 500, false, 0 };
    pthread_t t1, t2;   // trådobjekt ( själva trådarna) 

    pthread_create (&t1, 0, run_thread, &args1); 
    pthread_create (&t2, 0, run_thread, &args2);
    pthread_join(t1, 0); 
    pthread_join(t2, 0);
    return 0; 
}