#include "DMA_HAL.h"





void DMA_Stream_Init( DMA_TypeDef* dma , DMA_Stream_TypeDef* stream,
    DMA_Stream_Config_t* config)
{

    DMA_Enable_Clock(dma);

    stream->CR  |= (config->channel << DMA_SxCR_CHSEL_Pos) ; // connect stream to channel 
    stream->CR |=(config->priority << DMA_SxCR_PL_Pos); 

    
    stream->CR |=(config->direction << DMA_SxCR_DIR_Pos); // transfer direction 
    

    stream->CR |=(config->circular_mode<< DMA_SxCR_CIRC_Pos);

    // pointer increment (variables or ararays)
    stream->CR |=(config->memory_increment << DMA_SxCR_MINC_Pos);
    stream->CR |=(config->periph_increment << DMA_SxCR_PINC_Pos);

    //size  , number of data 
    stream->CR |=(config->size << DMA_SxCR_MSIZE_Pos);
    stream->CR |=(config->size << DMA_SxCR_PSIZE_Pos);
    stream->NDTR = config->number_data ;


    //addresses 

    stream->M0AR =  config->memory_address ;
    stream->PAR  = config->peripheral_address;
    
    //interrupts
    if (config->interrupt_mode == TRANSFER_COMPLETE)   
    {
        stream->CR |= (1<<DMA_SxCR_TCIE_Pos);
    }
    else if (config->interrupt_mode == TRANSFER_ERROR_COMPLETE)   
    {
        stream->CR |= (1<<DMA_SxCR_TCIE_Pos);
        stream->CR |= DMA_SxCR_TEIE ;
    }

    // ENable stream

    stream->CR |= (1<< DMA_SxCR_EN_Pos);

}

void DMA_Enable_Clock (DMA_TypeDef* dma)
{
    if (dma==DMA1)     {RCC ->AHB1ENR |= (RCC_AHB1ENR_DMA1EN);}
    else RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN ;

}