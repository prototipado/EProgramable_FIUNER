#ifndef ICONS_H_
#define ICONS_H_
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Devices Drivers devices
 ** @{ */
/** \addtogroup ICONS Icons
 ** @{ */

/** \brief Monochromatic icons for LCD display.
 *
 * @note Available sizes: 22x22 pixels, 30x30 pixels, 59x59 pixels, 89x89 pixels.
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
 * @brief Icons list
 */
typedef enum {
	ICON_BAT_0 = 0,			/*!< Battery discharged */
	ICON_BAT_1,				/*!< Battery low */
	ICON_BAT_2,				/*!< Battery medium */
	ICON_BAT_3,				/*!< Battery high */
	ICON_BAT_4,				/*!< Battery full */
	ICON_BAT_5,				/*!< Battery charging */
	ICON_SIGNAL_0,			/*!< Signal none */
	ICON_SIGNAL_1,			/*!< Signal low */
	ICON_SIGNAL_2,			/*!< Signal medium */
	ICON_SIGNAL_3,			/*!< Signal high */
	ICON_GPS,				/*!< GPS */
	ICON_WIFI_0,			/*!< WiFi none */
	ICON_WIFI_1,			/*!< WiFi low */
	ICON_WIFI_2,			/*!< WiFi medium */
	ICON_WIFI_3,			/*!< WiFi high */
	ICON_WIFI_4,			/*!< WiFi off */
	ICON_BLUETOOTH,			/*!< Bluetooth */
	ICON_LAN_0,				/*!< LAN connected */
	ICON_LAN_1,				/*!< LAN disconnected */
	ICON_USB,				/*!< USB */
	ICON_ARROW_0,			/*!< Arrow right */
	ICON_ARROW_1,			/*!< Arrow left */
	ICON_ARROW_2,			/*!< Arrow down */
	ICON_ARROW_3,			/*!< Arrow up */
	ICON_PLAY,				/*!< Play */
	ICON_PAUSE,				/*!< Pause */
	ICON_EJECT,				/*!< Eject */
	ICON_FAST_FOWARD,		/*!< Fast foward */
	ICON_REWIND,			/*!< Rewind */
	ICON_JUMP_END,			/*!< Jump end */
	ICON_JUMP_START,		/*!< Jump start */
	ICON_POWER_OFF,			/*!< Power off */
	ICON_SETTINGS,			/*!< Settings */
	ICON_REFRESH,			/*!< Refresh */
	ICON_DOWNLOAD,			/*!< Download */
	ICON_SAVE,				/*!< Save */
	ICON_TOOLS,				/*!< Tools */
	ICON_FILE,				/*!< File */
	ICON_FOLDER,			/*!< Folder */
	ICON_HOME,				/*!< Home */
	ICON_USER,				/*!< User */
	ICON_CHECK,				/*!< Check */
	ICON_CANCEL,			/*!< Cancel */
	ICON_WARNING,			/*!< Warning */
	ICON_INFO,				/*!< Info */
	ICON_HELP,				/*!< Help */
	ICON_FORBBIDEN,			/*!< Forbidden */
	ICON_ICON_THUMB,		/*!< Thum up */
	ICON_CLOCK,				/*!< Clock */
	ICON_LIGHT_ON,			/*!< Light on */
	ICON_LIGHT_OFF,			/*!< Licht off */
	ICON_TEMPERATURE,		/*!< Temperature */
	ICON_HUMIDITY,			/*!< Humidity */
	ICON_FIRE,				/*!< Fire */
	ICON_LOCK,				/*!< Lock */
	ICON_UNLOCK,			/*!< Unlock */
	ICON_HEART,				/*!< Heart */
	ICON_BRAIN,				/*!< Brain */
	ICON_MUSCLE,			/*!< Muscle */
	ICON_EYE,				/*!< Eye */
	ICON_HIDE,				/*!< Hide */
	ICON_CLOUD,				/*!< Cloud */
	ICON_SUN,				/*!< Sun */
	ICON_RAIN,				/*!< Rain */
} icon_t;

/**
 * @brief  Icon font structure
 */
typedef struct{
	uint8_t 		height;   		/*!< Icon height in pixels */
	uint8_t 		width;			/*!< Icon width in pixels */
	uint16_t 		offset;			/*!< Offset between icons in data array */
	const uint8_t 	*data; 			/*!< Icon data array */
} icon_font_t;

/*==================[external data declaration]==============================*/
/**
 * @brief  22x22 pixels icon structure
 */
extern icon_font_t icon_22;

/**
 * @brief  30x30 pixels icon structure
 */
extern icon_font_t icon_30;

/**
 * @brief  59x59 pixels icon structure
 */
extern icon_font_t icon_59;

/**
 * @brief  89x89 pixels icon structure
 */
extern icon_font_t icon_89;

/*==================[external functions declaration]=========================*/

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* ICONS_H_ */

/*==================[end of file]============================================*/
