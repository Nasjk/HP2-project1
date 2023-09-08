#pragma once

#include <stdint.h>
#include <stdbool.h>

/***********************************
Allt mellan "C" { och } är kompilerad som C kod.. 
***********************************/
#ifdef __cplusplus 
extern "C" {
#endif

#define SEM_ID_MIN 0
#define SEM_ID_MAX 31 

/***********************************
 *  
***********************************/

bool semaphore_reserve(const uint8_t sem_id); 

bool semaphore_release(const uint8_t sem_id); 




#ifdef __cplusplus
}
#endif