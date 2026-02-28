#include "stm32f407xx.h"


typedef enum
{
    PRIORITY_LOW,
    PRIORITY_MEDIUM,
    PRIORITY_HIGH,
    PRIORITY_VeryHIGH


} DMA_Priority_t ;

typedef enum
{
    BYTE,
    HALF_WORD,
    WORD,

} DMA_Data_Size_t ;



typedef enum

{
    PERIPH_TO_MEMORY,
    MEMORY_TO_PERIPH ,
    MEMORY_TO_MEMORY,
    


}DMA_Memory_Direction_t;

typedef enum

{
    FIXED ,
    INCREMENT,


}DMA_Periph_Increment_t;



typedef enum

{
    DISABLED ,
    ENABLED,


}DMA_Circular_Mode_t;

typedef enum

{
    TRANSFER_COMPLETE ,
    TRANSFER_ERROR_COMPLETE,   // honestly you either want TC or TC AND TE , YOU WANT OTHER NICHE SHIT GO USE HAL


}DMA_Interrupt_Mode_t;



typedef struct 
{
    uint8_t channel  ;
    DMA_Priority_t priority;
    DMA_Data_Size_t   size;
    uint16_t number_data;
    DMA_Memory_Direction_t direction;
    uint32_t memory_address;
    uint32_t peripheral_address;
    DMA_Periph_Increment_t  periph_increment;
    DMA_Periph_Increment_t  memory_increment;
    DMA_Interrupt_Mode_t interrupt_mode;
    DMA_Circular_Mode_t  circular_mode;
    
 

} DMA_Stream_Config_t;


void DMA_Stream_Init( DMA_TypeDef* dma ,  DMA_Stream_TypeDef* stream  , 
    DMA_Stream_Config_t* config);
void DMA_Enable_Clock(DMA_TypeDef* dma);