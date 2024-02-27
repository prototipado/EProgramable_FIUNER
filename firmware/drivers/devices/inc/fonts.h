/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup Fonts Fonts
 ** @{
 * @brief  Fonts library for LCD
 *
 * @note Currently, these fonts are supported:
 *  - 7 x 10 pixels
 *  - 11 x 18 pixels
 *  - 16 x 26 pixels
 *
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 21/11/2018 | Document creation		                         |
 *
 */

#ifndef FONTS_H_
#define FONTS_H_


 
#include <stdint.h>

/*****************************************************************************
 * Public macros/types/enumerations/variables definitions
 ****************************************************************************/

/**
 * @brief  Font structure
 */
typedef struct
{
	uint8_t FontWidth;    /*!< Font width in pixels */
	uint8_t FontHeight;   /*!< Font height in pixels */
	const uint16_t *data; /*!< Pointer to data font data array */
} Font_t;

/**
 * @brief  7 x 10 pixels font size structure
 */
extern Font_t font_7x10;

/**
 * @brief  11 x 18 pixels font size structure
 */
extern Font_t font_11x18;

/**
 * @brief  16 x 26 pixels font size structure
 */
extern Font_t font_16x26;

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */

#endif /* FONTS_H_ */
