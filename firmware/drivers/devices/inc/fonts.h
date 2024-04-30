#ifndef FONTS_H_
#define FONTS_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup ICONS Icons
 ** @{ */

/** \brief Fonts for LCD display.
 *
 * @note Available font height: 11 pixels, 19 pixels, 22 pixels, 30 pixels, 59 pixels, 89 pixels.
 * 
 * @note Available characters from " " (ASCII: 32) to "~" (ASCII: 126)
 * 
 * @note Created with http://www.eran.io/the-dot-factory-an-lcd-font-and-image-generator/
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 05/04/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdint.h>
/*==================[macros]=================================================*/

/*==================[typedef]================================================*/
/**
 * @brief Character information
 */
typedef struct{
	uint8_t width;		/*<! Character width in pixels */
	uint16_t offset;	/*<! Chracter position in font array */
} char_info_t;
/**
 * @brief  Font structure
 */
typedef struct{
	uint8_t 		font_height;   	/*!< Font height in pixels */
	char_info_t 	*info;			/*!< Character info array */
	const uint8_t 	*data; 			/*!< Font array */
} Font_t;

/*==================[external data declaration]==============================*/
/**
 * @brief  11 pixels font height structure
 */
extern Font_t font_11;

/**
 * @brief  19 pixels font height structure
 */
extern Font_t font_19;

/**
 * @brief  22 pixels font height structure
 */
extern Font_t font_22;

/**
 * @brief  22 pixels font height structure
 */
extern Font_t font_30;

/**
 * @brief  59 pixels font height structure
 */
extern Font_t font_59;

/**
 * @brief  89 pixels font height structure
 */
extern Font_t font_89;

/*==================[external functions declaration]=========================*/

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* FONTS_H_ */

/*==================[end of file]============================================*/
