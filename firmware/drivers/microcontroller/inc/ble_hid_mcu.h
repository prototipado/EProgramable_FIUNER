#ifndef BLE_MCU_H
#define BLE_MCU_H
/** \addtogroup Drivers_Programable Drivers Programable
 ** @{ */
/** \addtogroup Drivers_Microcontroller Drivers microcontroller
 ** @{ */
/** \addtogroup BLE Bluetooth Low Energy
 ** @{ */

/** \brief BLE driver for the ESP-EDU Board.
 *
 * @note This driver emulates HM-10 functionalities (same services and characteristics),
 * so it can be used to communicate with common Android apps, like "Bluetooth Electronics"
 * (https://play.google.com/store/apps/details?id=com.keuwl.arduinobluetooth)
 * 
 * @author Albano Peñalva
 *
 * @section changelog
 *
 * |   Date	    | Description                                    						|
 * |:----------:|:----------------------------------------------------------------------|
 * | 22/03/2024 | Document creation		                         						|
 * 
 **/

/*==================[inclusions]=============================================*/
#include <stdbool.h>
#include <stdint.h>
/*==================[macros]=================================================*/


/*==================[typedef]================================================*/
/**
 * @brief BLE connection status
 */
typedef enum ble_status {
	BLE_OFF,				/*!< BLE inactive */
	BLE_DISCONNECTED,		/*!< BLE device disconnected */
	BLE_CONNECTED			/*!< BLE device connected */
} ble_status_t;
/**
 * @brief Keyboard/Keypad Usage IDs
 */
typedef enum keyboard_cmd {
    HID_KEY_RESERVED       = 0,    // No event inidicated
    HID_KEY_A              = 4,    // Keyboard a and A
    HID_KEY_B              = 5,    // Keyboard b and B
    HID_KEY_C              = 6,    // Keyboard c and C
    HID_KEY_D              = 7,    // Keyboard d and D
    HID_KEY_E              = 8,    // Keyboard e and E
    HID_KEY_F              = 9,    // Keyboard f and F
    HID_KEY_G              = 10,   // Keyboard g and G
    HID_KEY_H              = 11,   // Keyboard h and H
    HID_KEY_I              = 12,   // Keyboard i and I
    HID_KEY_J              = 13,   // Keyboard j and J
    HID_KEY_K              = 14,   // Keyboard k and K
    HID_KEY_L              = 15,   // Keyboard l and L
    HID_KEY_M              = 16,   // Keyboard m and M
    HID_KEY_N              = 17,   // Keyboard n and N
    HID_KEY_O              = 18,   // Keyboard o and O
    HID_KEY_P              = 19,   // Keyboard p and p
    HID_KEY_Q              = 20,   // Keyboard q and Q
    HID_KEY_R              = 21,   // Keyboard r and R
    HID_KEY_S              = 22,   // Keyboard s and S
    HID_KEY_T              = 23,   // Keyboard t and T
    HID_KEY_U              = 24,   // Keyboard u and U
    HID_KEY_V              = 25,   // Keyboard v and V
    HID_KEY_W              = 26,   // Keyboard w and W
    HID_KEY_X              = 27,   // Keyboard x and X
    HID_KEY_Y              = 28,   // Keyboard y and Y
    HID_KEY_Z              = 29,   // Keyboard z and Z
    HID_KEY_1              = 30,   // Keyboard 1 and !
    HID_KEY_2              = 31,   // Keyboard 2 and @
    HID_KEY_3              = 32,   // Keyboard 3 and #
    HID_KEY_4              = 33,   // Keyboard 4 and %
    HID_KEY_5              = 34,   // Keyboard 5 and %
    HID_KEY_6              = 35,   // Keyboard 6 and ^
    HID_KEY_7              = 36,   // Keyboard 7 and &
    HID_KEY_8              = 37,   // Keyboard 8 and *
    HID_KEY_9              = 38,   // Keyboard 9 and (
    HID_KEY_0              = 39,   // Keyboard 0 and )
    HID_KEY_RETURN         = 40,   // Keyboard Return (ENTER)
    HID_KEY_ESCAPE         = 41,   // Keyboard ESCAPE
    HID_KEY_DELETE         = 42,   // Keyboard DELETE (Backspace)
    HID_KEY_TAB            = 43,   // Keyboard Tab
    HID_KEY_SPACEBAR       = 44,   // Keyboard Spacebar
    HID_KEY_MINUS          = 45,   // Keyboard - and (underscore)
    HID_KEY_EQUAL          = 46,   // Keyboard = and +
    HID_KEY_LEFT_BRKT      = 47,   // Keyboard [ and {
    HID_KEY_RIGHT_BRKT     = 48,   // Keyboard ] and }
    HID_KEY_BACK_SLASH     = 49,   // Keyboard \ and |
    HID_KEY_SEMI_COLON     = 51,   // Keyboard ; and :
    HID_KEY_SGL_QUOTE      = 52,   // Keyboard ' and "
    HID_KEY_GRV_ACCENT     = 53,   // Keyboard Grave Accent and Tilde
    HID_KEY_COMMA          = 54,   // Keyboard , and <
    HID_KEY_DOT            = 55,   // Keyboard . and >
    HID_KEY_FWD_SLASH      = 56,   // Keyboard / and ?
    HID_KEY_CAPS_LOCK      = 57,   // Keyboard Caps Lock
    HID_KEY_F1             = 58,   // Keyboard F1
    HID_KEY_F2             = 59,   // Keyboard F2
    HID_KEY_F3             = 60,   // Keyboard F3
    HID_KEY_F4             = 61,   // Keyboard F4
    HID_KEY_F5             = 62,   // Keyboard F5
    HID_KEY_F6             = 63,   // Keyboard F6
    HID_KEY_F7             = 64,   // Keyboard F7
    HID_KEY_F8             = 65,   // Keyboard F8
    HID_KEY_F9             = 66,   // Keyboard F9
    HID_KEY_F10            = 67,   // Keyboard F10
    HID_KEY_F11            = 68,   // Keyboard F11
    HID_KEY_F12            = 69,   // Keyboard F12
    HID_KEY_PRNT_SCREEN    = 70,   // Keyboard Print Screen
    HID_KEY_SCROLL_LOCK    = 71,   // Keyboard Scroll Lock
    HID_KEY_PAUSE          = 72,   // Keyboard Pause
    HID_KEY_INSERT         = 73,   // Keyboard Insert
    HID_KEY_HOME           = 74,   // Keyboard Home
    HID_KEY_PAGE_UP        = 75,   // Keyboard PageUp
    HID_KEY_DELETE_FWD     = 76,   // Keyboard Delete Forward
    HID_KEY_END            = 77,   // Keyboard End
    HID_KEY_PAGE_DOWN      = 78,   // Keyboard PageDown
    HID_KEY_RIGHT_ARROW    = 79,   // Keyboard RightArrow
    HID_KEY_LEFT_ARROW     = 80,   // Keyboard LeftArrow
    HID_KEY_DOWN_ARROW     = 81,   // Keyboard DownArrow
    HID_KEY_UP_ARROW       = 82,   // Keyboard UpArrow
    HID_KEY_NUM_LOCK       = 83,   // Keypad Num Lock and Clear
    HID_KEY_DIVIDE         = 84,   // Keypad /
    HID_KEY_MULTIPLY       = 85,   // Keypad *
    HID_KEY_SUBTRACT       = 86,   // Keypad -
    HID_KEY_ADD            = 87,   // Keypad +
    HID_KEY_ENTER          = 88,   // Keypad ENTER
    HID_KEYPAD_1           = 89,   // Keypad 1 and End
    HID_KEYPAD_2           = 90,   // Keypad 2 and Down Arrow
    HID_KEYPAD_3           = 91,   // Keypad 3 and PageDn
    HID_KEYPAD_4           = 92,   // Keypad 4 and Lfet Arrow
    HID_KEYPAD_5           = 93,   // Keypad 5
    HID_KEYPAD_6           = 94,   // Keypad 6 and Right Arrow
    HID_KEYPAD_7           = 95,   // Keypad 7 and Home
    HID_KEYPAD_8           = 96,   // Keypad 8 and Up Arrow
    HID_KEYPAD_9           = 97,   // Keypad 9 and PageUp
    HID_KEYPAD_0           = 98,   // Keypad 0 and Insert
    HID_KEYPAD_DOT         = 99,   // Keypad . and Delete
    HID_KEY_MUTE           = 127,  // Keyboard Mute
    HID_KEY_VOLUME_UP      = 128,  // Keyboard Volume up
    HID_KEY_VOLUME_DOWN    = 129,  // Keyboard Volume down
    HID_KEY_LEFT_CTRL      = 224,  // Keyboard LeftContorl
    HID_KEY_LEFT_SHIFT     = 225,  // Keyboard LeftShift
    HID_KEY_LEFT_ALT       = 226,  // Keyboard LeftAlt
    HID_KEY_LEFT_GUI       = 227,  // Keyboard LeftGUI
    HID_KEY_RIGHT_CTRL     = 228,  // Keyboard RightContorl
    HID_KEY_RIGHT_SHIFT    = 229,  // Keyboard RightShift
    HID_KEY_RIGHT_ALT      = 230,  // Keyboard RightAlt
    HID_KEY_RIGHT_GUI      = 231  // Keyboard RightGUI
} keyboard_cmd_t;
/**
 * @brief Mask of modifier keys to be pressed together with any key
 */
typedef enum key_mask {
	LEFT_CONTROL_KEY_MASK   = (1 << 0),
	LEFT_SHIFT_KEY_MASK     = (1 << 1),
	LEFT_ALT_KEY_MASK       = (1 << 2),
	LEFT_GUI_KEY_MASK       = (1 << 3),
	RIGHT_CONTROL_KEY_MASK  = (1 << 4),
	RIGHT_SHIFT_KEY_MASK    = (1 << 5),
	RIGHT_ALT_KEY_MASK      = (1 << 6),
	RIGHT_GUI_KEY_MASK      = (1 << 7)
} key_mask_t;
/**
 * @brief Mouse commands
 */
typedef enum mouse_cmd {
    HID_NO_BUTTON       = 0,
    HID_MOUSE_LEFT      = 253,
    HID_MOUSE_MIDDLE    = 254,
    HID_MOUSE_RIGHT     = 255
} mouse_cmd_t;
/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
/**
 * @brief Bluetooth initialization as a HID device
 * 
 * @note It'll start advertising automatically after initialization.
 * 
 * @param hid_dev_name	Device name
 */
void BleHidInit(char * hid_dev_name);

/**
 * @brief Gets BLE connection status
 * 
 * @return ble_status_t Connection status
 */
ble_status_t BleHidStatus(void);

/**
 * @brief Send a group of keys to be pressed together
 * 
 * @param special_key_mask      Modifier keys mask
 * @param keyboard_cmd          Array with keys (max: 6)
 * @param num_key               Number of keys (in keyboard_cmd array) to be pressed together (max: 6)
 */
void BleHidSendKeyboard(key_mask_t special_key_mask, keyboard_cmd_t *keyboard_cmd, uint8_t num_key);

/**
 * @brief Send mouse position and click event
 * 
 * @param mouse_button      Button to be clicked
 * @param delta_x           X cursor relative position
 * @param delta_y           Y cursor relative position
 */
void BleHidSendMouse(mouse_cmd_t mouse_button, int8_t delta_x, int8_t delta_y);

/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
#endif /* BLE_MCU_H */

/*==================[end of file]============================================*/
