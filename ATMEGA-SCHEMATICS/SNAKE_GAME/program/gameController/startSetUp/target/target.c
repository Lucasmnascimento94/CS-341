#include "target.h"



void targetSpiUpdate(SPI *spi, TARGET_CONF *target){
    // Reboot SPI if necessary
    if(!((spi->cs_reg->CS_PORT == target->cs_reg->CS_PORT) && \
        spi->cs_reg->CS_PIN == target->cs_reg->CS_PIN)){
        spi->cs_reg = target->cs_reg;
        spi->conf = target->spi_conf;
        spiInit(spi);
    }
}

void targetFlashInitVars(SPI *spi, TARGET_CONF *target){
    targetSpiUpdate(spi, target);
}

void targetGridInitVars(SPI *spi, TARGET_CONF *target){
    targetSpiUpdate(spi, target);
}

void targetGameInitVars(SPI *spi, TARGET_CONF *target){
    targetSpiUpdate(spi, target);
}

void emitSignal(TARGET_CONF *target){
    *target->signal->SIGNAL_DDR  |= (1<<target->signal->SIGNAL_PIN);
    *target->signal->SIGNAL_PORT |= (1<<target->signal->SIGNAL_PIN);
}

void targetSram(SPI *spi, TARGET_CONF *target){
    targetSpiUpdate(spi, target);
    emitSignal(target);
}