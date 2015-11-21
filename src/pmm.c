
#include "main.h"

#define _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_DISABLE_FULL_PERFORMANCE_


#ifdef _HAL_PMM_DISABLE_SVML_
#define _HAL_PMM_SVMLE SVMLE
#else
#define _HAL_PMM_SVMLE 0
#endif /* _HAL_PMM_DISABLE_SVML_ */

#ifdef _HAL_PMM_DISABLE_SVSL_
#define _HAL_PMM_SVSLE SVSLE
#else
#define _HAL_PMM_SVSLE 0
#endif /* _HAL_PMM_DISABLE_SVSL_ */

#ifdef _HAL_PMM_DISABLE_FULL_PERFORMANCE_
#define _HAL_PMM_SVSFP SVSLFP
#else
#define _HAL_PMM_SVSFP 0
#endif /* _HAL_PMM_DISABLE_FULL_PERFORMANCE_ */



static uint8_t set_vcore_up( uint8_t level )
{
    uint32_t pmmrie_backup;
    uint32_t svsmhctl_backup;

    /* Open PMM registers for write access */
    PMMCTL0_H = 0xA5;

    /* Disable dedicated interrupts to prevent that
     * needed flags will be cleared */
    pmmrie_backup = PMMRIE;
    PMMRIE &= ~( SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE );
    /* Set SVM highside to new level and check if
     * a VCore increase is possible */
    svsmhctl_backup = SVSMHCTL;
    PMMIFG &= ~( SVMHIFG | SVSMHDLYIFG );
    SVSMHCTL = SVMHE | SVMHFP | ( SVSMHRRL0 * level );

    /* Wait until SVM highside is settled */
    while( !( PMMIFG & SVSMHDLYIFG ) ) {
        ;
    }

    /* Disable full-performance mode to save energy */
    SVSMHCTL &= ~_HAL_PMM_SVSFP ;

    /* Check if a VCore increase is possible */
    if( ( PMMIFG & SVMHIFG ) == SVMHIFG ) { /* Vcc is too low */
        /* Recover the previous settings */
        PMMIFG &= ~SVSMHDLYIFG;
        SVSMHCTL = svsmhctl_backup;

        /* Wait until SVM highside is settled */
        while( !( PMMIFG & SVSMHDLYIFG ) ) {
            ;
        }

        /* Clear all flags */
        PMMIFG &= ~( SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG |
                     SVSMLDLYIFG );
        /* Backup PMM-Interrupt-Register */
        PMMRIE = pmmrie_backup;

        /* Lock PMM registers for write access */
        PMMCTL0_H = 0x00;

        return PMM_STATUS_ERROR; /* Voltage not set */
    }

    /* Set also SVS highside to new level */
    SVSMHCTL |= SVSHE | ( SVSHRVL0 * level );
    /* Set SVM low side to new level */
    SVSMLCTL = SVMLE | SVMLFP | ( SVSMLRRL0 * level );

    /* Wait until SVM low side is settled */
    while( !( PMMIFG & SVSMLDLYIFG ) ) {
        ;
    }

    /* Clear already set flags */
    PMMIFG &= ~( SVMLVLRIFG | SVMLIFG );
    /* Set VCore to new level */
    PMMCTL0_L = PMMCOREV0 * level;

    /* Wait until new level reached */
    if( PMMIFG & SVMLIFG ) {
        while( !( PMMIFG & SVMLVLRIFG ) ) {
            ;
        }
    }

    /* Set also SVS/SVM low side to new level */
    PMMIFG &= ~SVSMLDLYIFG;
    SVSMLCTL |= SVSLE | ( SVSLRVL0 * level );

    /* Wait for lowside delay flags */
    while( !( PMMIFG & SVSMLDLYIFG ) ) {
        ;
    }

    /* Disable SVS/SVM Low
     * Disable full-performance mode to save energy */
    SVSMLCTL &= ~( _HAL_PMM_DISABLE_SVSL_ +
                   _HAL_PMM_DISABLE_SVML_ +
                   _HAL_PMM_SVSFP );

    /* Clear all flags */
    PMMIFG &= ~( SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG |
                 SVSMLDLYIFG );
    /* Backup PMM-Interrupt-Register */
    PMMRIE = pmmrie_backup;

    /* Lock PMM registers for write access */
    PMMCTL0_H = 0x00;

    return PMM_STATUS_OK;
}

static uint8_t set_vcore_down( uint8_t level )
{
    uint32_t pmmrie_backup;

    /* Open PMM registers for write access */
    PMMCTL0_H = 0xA5;

    /* Disable dedicated interrupts to prevent that
     * needed flags will be cleared */
    pmmrie_backup = PMMRIE;
    PMMRIE &= ~( SVSMHDLYIE | SVSMLDLYIE | SVMLVLRIE | SVMHVLRIE | SVMHVLRPE );

    /* Set SVM high side and SVM low side to new level */
    PMMIFG &= ~( SVMHIFG | SVSMHDLYIFG | SVMLIFG | SVSMLDLYIFG );
    SVSMHCTL = SVMHE | SVMHFP | ( SVSMHRRL0 * level );
    SVSMLCTL = SVMLE | SVMLFP | ( SVSMLRRL0 * level );

    /* Wait until SVM high side and SVM low side is settled */
    while( !( PMMIFG & SVSMHDLYIFG ) || !( PMMIFG & SVSMLDLYIFG ) ) {
        ;
    }

    /* Set VCore to new level */
    PMMCTL0_L = PMMCOREV0 * level;

    /* Set also SVS highside and SVS low side to new level */
    PMMIFG &= ~( SVSHIFG | SVSMHDLYIFG | SVSLIFG | SVSMLDLYIFG );
    SVSMHCTL |= SVSHE | SVSHFP | ( SVSHRVL0 * level );
    SVSMLCTL |= SVSLE | SVSLFP | ( SVSLRVL0 * level );

    /* Wait until SVS high side and SVS low side is settled */
    while( !( PMMIFG & SVSMHDLYIFG ) || !( PMMIFG & SVSMLDLYIFG ) ) {
        ;
    }

    /* Disable full-performance mode to save energy */
    SVSMHCTL &= ~_HAL_PMM_SVSFP;
    /* Disable SVS/SVM Low */
    /* Disable full-performance mode to save energy */
    SVSMLCTL &= ~( _HAL_PMM_DISABLE_SVSL_ +
                   _HAL_PMM_DISABLE_SVML_ +
                   _HAL_PMM_SVSFP );

    /* Clear all flags */
    PMMIFG &= ~( SVMHVLRIFG | SVMHIFG | SVSMHDLYIFG | SVMLVLRIFG | SVMLIFG |
                 SVSMLDLYIFG );
    /* Backup PMM-Interrupt-Register */
    PMMRIE = pmmrie_backup;
    /* Lock PMM registers for write access */
    PMMCTL0_H = 0x00;

    if( ( PMMIFG & SVMHIFG ) == SVMHIFG ) {
        /* Highside is still to low for the adjusted VCore level */
        return PMM_STATUS_ERROR;
    }

    return PMM_STATUS_OK;
}

uint8_t set_vcore( uint8_t level )
{
    uint16_t    actlevel;
    uint8_t     status;

    level &= PMMCOREV_3;                /* Set mask for max. level */
    actlevel = PMMCTL0 & PMMCOREV_3;    /* Get actual VCore */
    status = 0;

    /* Step by step increase or decrease */
    while( ( ( level != actlevel ) && !status ) || ( level < actlevel ) ) {
        if( level > actlevel ) {
            status = set_vcore_up( ++actlevel );
        } else {
            status = set_vcore_down( --actlevel );
        }
    }

    return status;
}