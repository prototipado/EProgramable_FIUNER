/**
 * @file ble_hid_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-05-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "ble_hid_mcu.h"
#include <stdint.h>
#include <string.h>

#include "nvs_flash.h"

#include "esp_log.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
/*==================[macros and definitions]=================================*/
#define TAG "ble_hid"
/********************esp_hidd_prf_api**********************/
// HID keyboard input report length
#define HID_KEYBOARD_IN_RPT_LEN     		8
// HID LED output report length
#define HID_LED_OUT_RPT_LEN         		1
// HID mouse input report length
#define HID_MOUSE_IN_RPT_LEN        		5
// HID consumer control input report length
#define HID_CC_IN_RPT_LEN           		2
/*************************hid_dev**************************/
/* HID Report type */
#define HID_TYPE_INPUT       				1
#define HID_TYPE_OUTPUT      				2
#define HID_TYPE_FEATURE     				3

/*************************hidd_le**************************/
/// Maximal number of HIDS that can be added in the DB
#ifndef USE_ONE_HIDS_INSTANCE
#define HIDD_LE_NB_HIDS_INST_MAX            (2)
#else
#define HIDD_LE_NB_HIDS_INST_MAX            (1)
#endif
#define HIDD_GREAT_VER   					0x01  //Version + Subversion
#define HIDD_SUB_VER     					0x00  //Version + Subversion
#define HIDD_VERSION     					((HIDD_GREAT_VER<<8)|HIDD_SUB_VER)  //Version + Subversion
#define HID_MAX_APPS                 		1
// Number of HID reports defined in the service
#define HID_NUM_REPORTS          			9
// HID Report IDs for the service
#define HID_RPT_ID_MOUSE_IN      			1   // Mouse input report ID
#define HID_RPT_ID_KEY_IN        			2   // Keyboard input report ID
#define HID_RPT_ID_CC_IN         			3   //Consumer Control input report ID
#define HID_RPT_ID_VENDOR_OUT    			4   // Vendor output report ID
#define HID_RPT_ID_LED_OUT       			2  // LED output report ID
#define HID_RPT_ID_FEATURE       			0  // Feature report ID
#define HIDD_APP_ID							0x1812//ATT_SVC_HID
#define BATTRAY_APP_ID       				0x180f
#define ATT_SVC_HID          				0x1812
/// Maximal number of Report Char. that can be added in the DB for one HIDS - Up to 11
#define HIDD_LE_NB_REPORT_INST_MAX          (5)
/// Maximal length of Report Char. Value
#define HIDD_LE_REPORT_MAX_LEN              (255)
/// Maximal length of Report Map Char. Value
#define HIDD_LE_REPORT_MAP_MAX_LEN          (512)
/// Length of Boot Report Char. Value Maximal Length
#define HIDD_LE_BOOT_REPORT_MAX_LEN         (8)
/// Boot KB Input Report Notification Configuration Bit Mask
#define HIDD_LE_BOOT_KB_IN_NTF_CFG_MASK     (0x40)
/// Boot KB Input Report Notification Configuration Bit Mask
#define HIDD_LE_BOOT_MOUSE_IN_NTF_CFG_MASK  (0x80)
/// Boot Report Notification Configuration Bit Mask
#define HIDD_LE_REPORT_NTF_CFG_MASK         (0x20)
/* HID information flags */
#define HID_FLAGS_REMOTE_WAKE           	0x01      // RemoteWake
#define HID_FLAGS_NORMALLY_CONNECTABLE  	0x02      // NormallyConnectable
/* Control point commands */
#define HID_CMD_SUSPEND                 	0x00      // Suspend
#define HID_CMD_EXIT_SUSPEND            	0x01      // Exit Suspend
/* HID protocol mode values */
#define HID_PROTOCOL_MODE_BOOT          	0x00      // Boot Protocol Mode
#define HID_PROTOCOL_MODE_REPORT        	0x01      // Report Protocol Mode
/* Attribute value lengths */
#define HID_PROTOCOL_MODE_LEN           	1         // HID Protocol Mode
#define HID_INFORMATION_LEN             	4         // HID Information
#define HID_REPORT_REF_LEN              	2         // HID Report Reference Descriptor
#define HID_EXT_REPORT_REF_LEN          	2         // External Report Reference Descriptor
// HID feature flags
#define HID_KBD_FLAGS             			HID_FLAGS_REMOTE_WAKE
/* HID Report type */
#define HID_REPORT_TYPE_INPUT       		1
#define HID_REPORT_TYPE_OUTPUT      		2
#define HID_REPORT_TYPE_FEATURE     		3
/// HID Service Attributes Indexes
enum {
    HIDD_LE_IDX_SVC,
    // Included Service
    HIDD_LE_IDX_INCL_SVC,
    // HID Information
    HIDD_LE_IDX_HID_INFO_CHAR,
    HIDD_LE_IDX_HID_INFO_VAL,
    // HID Control Point
    HIDD_LE_IDX_HID_CTNL_PT_CHAR,
    HIDD_LE_IDX_HID_CTNL_PT_VAL,
    // Report Map
    HIDD_LE_IDX_REPORT_MAP_CHAR,
    HIDD_LE_IDX_REPORT_MAP_VAL,
    HIDD_LE_IDX_REPORT_MAP_EXT_REP_REF,
    // Protocol Mode
    HIDD_LE_IDX_PROTO_MODE_CHAR,
    HIDD_LE_IDX_PROTO_MODE_VAL,
    // Report mouse input
    HIDD_LE_IDX_REPORT_MOUSE_IN_CHAR,
    HIDD_LE_IDX_REPORT_MOUSE_IN_VAL,
    HIDD_LE_IDX_REPORT_MOUSE_IN_CCC,
    HIDD_LE_IDX_REPORT_MOUSE_REP_REF,
    //Report Key input
    HIDD_LE_IDX_REPORT_KEY_IN_CHAR,
    HIDD_LE_IDX_REPORT_KEY_IN_VAL,
    HIDD_LE_IDX_REPORT_KEY_IN_CCC,
    HIDD_LE_IDX_REPORT_KEY_IN_REP_REF,
    ///Report Led output
    HIDD_LE_IDX_REPORT_LED_OUT_CHAR,
    HIDD_LE_IDX_REPORT_LED_OUT_VAL,
    HIDD_LE_IDX_REPORT_LED_OUT_REP_REF,
    HIDD_LE_IDX_REPORT_CC_IN_CHAR,
    HIDD_LE_IDX_REPORT_CC_IN_VAL,
    HIDD_LE_IDX_REPORT_CC_IN_CCC,
    HIDD_LE_IDX_REPORT_CC_IN_REP_REF,
    // Boot Keyboard Input Report
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_VAL,
    HIDD_LE_IDX_BOOT_KB_IN_REPORT_NTF_CFG,
    // Boot Keyboard Output Report
    HIDD_LE_IDX_BOOT_KB_OUT_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_KB_OUT_REPORT_VAL,
    // Boot Mouse Input Report
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_CHAR,
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_VAL,
    HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_NTF_CFG,
    // Report
    HIDD_LE_IDX_REPORT_CHAR,
    HIDD_LE_IDX_REPORT_VAL,
    HIDD_LE_IDX_REPORT_REP_REF,
    //HIDD_LE_IDX_REPORT_NTF_CFG,
    HIDD_LE_IDX_NB,
};
/// Attribute Table Indexes
enum {
    HIDD_LE_INFO_CHAR,
    HIDD_LE_CTNL_PT_CHAR,
    HIDD_LE_REPORT_MAP_CHAR,
    HIDD_LE_REPORT_CHAR,
    HIDD_LE_PROTO_MODE_CHAR,
    HIDD_LE_BOOT_KB_IN_REPORT_CHAR,
    HIDD_LE_BOOT_KB_OUT_REPORT_CHAR,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR,
    HIDD_LE_CHAR_MAX //= HIDD_LE_REPORT_CHAR + HIDD_LE_NB_REPORT_INST_MAX,
};
///att read event table Indexs
enum {
    HIDD_LE_READ_INFO_EVT,
    HIDD_LE_READ_CTNL_PT_EVT,
    HIDD_LE_READ_REPORT_MAP_EVT,
    HIDD_LE_READ_REPORT_EVT,
    HIDD_LE_READ_PROTO_MODE_EVT,
    HIDD_LE_BOOT_KB_IN_REPORT_EVT,
    HIDD_LE_BOOT_KB_OUT_REPORT_EVT,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_EVT,

    HID_LE_EVT_MAX
};
/// Client Characteristic Configuration Codes
enum {
    HIDD_LE_DESC_MASK = 0x10,

    HIDD_LE_BOOT_KB_IN_REPORT_CFG     = HIDD_LE_BOOT_KB_IN_REPORT_CHAR | HIDD_LE_DESC_MASK,
    HIDD_LE_BOOT_MOUSE_IN_REPORT_CFG  = HIDD_LE_BOOT_MOUSE_IN_REPORT_CHAR | HIDD_LE_DESC_MASK,
    HIDD_LE_REPORT_CFG                = HIDD_LE_REPORT_CHAR | HIDD_LE_DESC_MASK,
};
/// Features Flag Values
enum {
    HIDD_LE_CFG_KEYBOARD      = 0x01,
    HIDD_LE_CFG_MOUSE         = 0x02,
    HIDD_LE_CFG_PROTO_MODE    = 0x04,
    HIDD_LE_CFG_MAP_EXT_REF   = 0x08,
    HIDD_LE_CFG_BOOT_KB_WR    = 0x10,
    HIDD_LE_CFG_BOOT_MOUSE_WR = 0x20,
};
/// Report Char. Configuration Flag Values
enum {
    HIDD_LE_CFG_REPORT_IN     = 0x01,
    HIDD_LE_CFG_REPORT_OUT    = 0x02,
    //HOGPD_CFG_REPORT_FEAT can be used as a mask to check Report type
    HIDD_LE_CFG_REPORT_FEAT   = 0x03,
    HIDD_LE_CFG_REPORT_WR     = 0x10,
};
/// Pointer to the connection clean-up function
#define HIDD_LE_CLEANUP_FNCT	(NULL)
/// Battery Service Attributes Indexes
enum{
    BAS_IDX_SVC,

    BAS_IDX_BATT_LVL_CHAR,
    BAS_IDX_BATT_LVL_VAL,
    BAS_IDX_BATT_LVL_NTF_CFG,
    BAS_IDX_BATT_LVL_PRES_FMT,

    BAS_IDX_NB,
};
#define HI_UINT16(a) 			(((a) >> 8) & 0xFF)
#define LO_UINT16(a) 			((a) & 0xFF)
#define PROFILE_NUM            	1
#define PROFILE_APP_IDX        	0
#define CHAR_DECLARATION_SIZE   (sizeof(uint8_t))

/***************************hidd****************************/

/*==================[typedef]================================================*/
/********************esp_hidd_prf_api**********************/
typedef enum {
    ESP_HIDD_EVENT_REG_FINISH = 0,
    ESP_BAT_EVENT_REG,
    ESP_HIDD_EVENT_DEINIT_FINISH,
    ESP_HIDD_EVENT_BLE_CONNECT,
    ESP_HIDD_EVENT_BLE_DISCONNECT,
    ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT,
    ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT,
} esp_hidd_cb_event_t;
/// HID config status
typedef enum {
    ESP_HIDD_STA_CONN_SUCCESS = 0x00,
    ESP_HIDD_STA_CONN_FAIL    = 0x01,
} esp_hidd_sta_conn_state_t;
/// HID init status
typedef enum {
    ESP_HIDD_INIT_OK = 0,
    ESP_HIDD_INIT_FAILED = 1,
} esp_hidd_init_state_t;
/// HID deinit status
typedef enum {
    ESP_HIDD_DEINIT_OK = 0,
    ESP_HIDD_DEINIT_FAILED = 0,
} esp_hidd_deinit_state_t;
/**
 * @brief HIDD callback parameters union
 */
typedef union {
    /**
	 * @brief ESP_HIDD_EVENT_INIT_FINISH
	 */
    struct hidd_init_finish_evt_param {
        esp_hidd_init_state_t state;				/*!< Initial status */
        esp_gatt_if_t gatts_if;
    } init_finish;							      /*!< HID callback param of ESP_HIDD_EVENT_INIT_FINISH */
    /**
	 * @brief ESP_HIDD_EVENT_DEINIT_FINISH
	 */
    struct hidd_deinit_finish_evt_param {
        esp_hidd_deinit_state_t state;				/*!< De-initial status */
    } deinit_finish;								/*!< HID callback param of ESP_HIDD_EVENT_DEINIT_FINISH */
    /**
     * @brief ESP_HIDD_EVENT_CONNECT
	 */
    struct hidd_connect_evt_param {
        uint16_t conn_id;
        esp_bd_addr_t remote_bda;                   /*!< HID Remote bluetooth connection index */
    } connect;									    /*!< HID callback param of ESP_HIDD_EVENT_CONNECT */
    /**
     * @brief ESP_HIDD_EVENT_DISCONNECT
	 */
    struct hidd_disconnect_evt_param {
        esp_bd_addr_t remote_bda;                   /*!< HID Remote bluetooth device address */
    } disconnect;									/*!< HID callback param of ESP_HIDD_EVENT_DISCONNECT */
    /**
     * @brief ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT
	 */
    struct hidd_vendor_write_evt_param {
        uint16_t conn_id;                           /*!< HID connection index */
        uint16_t report_id;                         /*!< HID report index */
        uint16_t length;                            /*!< data length */
        uint8_t  *data;                             /*!< The pointer to the data */
    } vendor_write;									/*!< HID callback param of ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT */
    /**
     * @brief ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT
     */
    struct hidd_led_write_evt_param {
        uint16_t conn_id;
        uint8_t report_id;
        uint8_t length;
        uint8_t *data;
    } led_write;
} esp_hidd_cb_param_t;
/**
 * @brief HID device event callback function type
 * @param event : Event type
 * @param param : Point to callback parameter, currently is union type
 */
typedef void (*esp_hidd_event_cb_t) (esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);

/*************************hid_dev**************************/
// HID report mapping table
typedef struct{
  uint16_t    handle;           // Handle of report characteristic
  uint16_t    cccdHandle;       // Handle of CCCD for report characteristic
  uint8_t     id;               // Report ID
  uint8_t     type;             // Report type
  uint8_t     mode;             // Protocol mode (report or boot)
} hid_report_map_t;
// HID dev configuration structure
typedef struct{
  uint32_t    idleTimeout;      // Idle timeout in milliseconds
  uint8_t     hidFlags;         // HID feature flags
} hid_dev_cfg_t;

/*************************hidd_le**************************/
/// HIDD Features structure
typedef struct {
    /// Service Features
    uint8_t svc_features;
    /// Number of Report Char. instances to add in the database
    uint8_t report_nb;
    /// Report Char. Configuration
    uint8_t report_char_cfg[HIDD_LE_NB_REPORT_INST_MAX];
} hidd_feature_t;
//
typedef struct {
    bool                        in_use;
    bool                        congest;
    uint16_t                  conn_id;
    bool                        connected;
    esp_bd_addr_t         remote_bda;
    uint32_t                  trans_id;
    uint8_t                    cur_srvc_id;
} hidd_clcb_t;
// HID report mapping table
typedef struct {
    uint16_t    handle;           // Handle of report characteristic
    uint16_t    cccdHandle;       // Handle of CCCD for report characteristic
    uint8_t     id;               // Report ID
    uint8_t     type;             // Report type
    uint8_t     mode;             // Protocol mode (report or boot)
} hidRptMap_t;
//
typedef struct {
    /// hidd profile id
    uint8_t app_id;
    /// Notified handle
    uint16_t ntf_handle;
    ///Attribute handle Table
    uint16_t att_tbl[HIDD_LE_IDX_NB];
    /// Supported Features
    hidd_feature_t   hidd_feature[HIDD_LE_NB_HIDS_INST_MAX];
    /// Current Protocol Mode
    uint8_t proto_mode[HIDD_LE_NB_HIDS_INST_MAX];
    /// Number of HIDS added in the database
    uint8_t hids_nb;
    uint8_t pending_evt;
    uint16_t pending_hal;
} hidd_inst_t;
/// Report Reference structure
typedef struct{
    ///Report ID
    uint8_t report_id;
    ///Report Type
    uint8_t report_type;
}hids_report_ref_t;
/// HID Information structure
typedef struct{
    /// bcdHID
    uint16_t bcdHID;
    /// bCountryCode
    uint8_t bCountryCode;
    /// Flags
    uint8_t flags;
}hids_hid_info_t;
/* service engine control block */
typedef struct {
    hidd_clcb_t                  hidd_clcb[HID_MAX_APPS];          /* connection link*/
    esp_gatt_if_t                gatt_if;
    bool                         enabled;
    bool                         is_take;
    bool                         is_primery;
    hidd_inst_t                  hidd_inst;
    esp_hidd_event_cb_t          hidd_cb;
    uint8_t                      inst_id;
} hidd_le_env_t;

/***************************hidd****************************/

/*==================[internal data declaration]==============================*/
/********************esp_hidd_prf_api**********************/


/*************************hid_dev**************************/
static hid_report_map_t *hid_dev_rpt_tbl;
static uint8_t hid_dev_rpt_tbl_Len;

/*************************hidd_le**************************/
extern hidd_le_env_t hidd_le_env;
extern uint8_t hidProtocolMode;
/// characteristic presentation information
struct prf_char_pres_fmt{
    /// Unit (The Unit is a UUID)
    uint16_t unit;
    /// Description
    uint16_t description;
    /// Format
    uint8_t format;
    /// Exponent
    uint8_t exponent;
    /// Name space
    uint8_t name_space;
};
// HID report mapping table
static hid_report_map_t hid_rpt_map[HID_NUM_REPORTS];
//
struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
};
hidd_le_env_t hidd_le_env;
char * device_name;         /* Device name */

/*==================[internal functions declaration]=========================*/
/********************esp_hidd_prf_api**********************/
/**
 * @brief           This function is called to receive hid device callback event
 */
esp_err_t esp_hidd_register_callbacks(esp_hidd_event_cb_t callbacks);
/**
 * @brief           This function is called to initialize hid device profile
 */
esp_err_t esp_hidd_profile_init(void);
/**
 * @brief           This function is called to de-initialize hid device profile
 */
esp_err_t esp_hidd_profile_deinit(void);
/**
 * @brief           Get hidd profile version
 */
uint16_t esp_hidd_get_version(void);

/*************************hid_dev**************************/
/**
 * @brief           
 */
void hid_dev_register_reports(uint8_t num_reports, hid_report_map_t *p_report);
/**
 * @brief           
 */
void hid_dev_send_report(esp_gatt_if_t gatts_if, uint16_t conn_id,
    					uint8_t id, uint8_t type, uint8_t length, uint8_t *data);

/*************************hidd_le**************************/
/**
 * @brief           
 */
void hidd_clcb_alloc (uint16_t conn_id, esp_bd_addr_t bda);
/**
 * @brief           
 */
bool hidd_clcb_dealloc (uint16_t conn_id);
/**
 * @brief           
 */
void hidd_le_create_service(esp_gatt_if_t gatts_if);
/**
 * @brief           
 */
void hidd_set_attr_value(uint16_t handle, uint16_t val_len, const uint8_t *value);
/**
 * @brief           
 */
void hidd_get_attr_value(uint16_t handle, uint16_t *length, uint8_t **value);
/**
 * @brief           
 */
esp_err_t hidd_register_cb(void);
/**
 * @brief           
 */
static void hid_add_id_tbl(void);

/***************************hidd****************************/
/**
 * @brief           
 */
static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param);

/*==================[internal data definition]===============================*/
/********************esp_hidd_prf_api**********************/


/*************************hid_dev**************************/


/*************************hidd_le**************************/
// HID Report Map characteristic value
// Keyboard report descriptor (using format for Boot interface descriptor)
static const uint8_t hidReportMap[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop)
    0x09, 0x02,  // Usage (Mouse)
    0xA1, 0x01,  // Collection (Application)
    0x85, 0x01,  // Report Id (1)
    0x09, 0x01,  //   Usage (Pointer)
    0xA1, 0x00,  //   Collection (Physical)
    0x05, 0x09,  //     Usage Page (Buttons)
    0x19, 0x01,  //     Usage Minimum (01) - Button 1
    0x29, 0x03,  //     Usage Maximum (03) - Button 3
    0x15, 0x00,  //     Logical Minimum (0)
    0x25, 0x01,  //     Logical Maximum (1)
    0x75, 0x01,  //     Report Size (1)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x02,  //     Input (Data, Variable, Absolute) - Button states
    0x75, 0x05,  //     Report Size (5)
    0x95, 0x01,  //     Report Count (1)
    0x81, 0x01,  //     Input (Constant) - Padding or Reserved bits
    0x05, 0x01,  //     Usage Page (Generic Desktop)
    0x09, 0x30,  //     Usage (X)
    0x09, 0x31,  //     Usage (Y)
    0x09, 0x38,  //     Usage (Wheel)
    0x15, 0x81,  //     Logical Minimum (-127)
    0x25, 0x7F,  //     Logical Maximum (127)
    0x75, 0x08,  //     Report Size (8)
    0x95, 0x03,  //     Report Count (3)
    0x81, 0x06,  //     Input (Data, Variable, Relative) - X & Y coordinate
    0xC0,        //   End Collection
    0xC0,        // End Collection

    0x05, 0x01,  // Usage Pg (Generic Desktop)
    0x09, 0x06,  // Usage (Keyboard)
    0xA1, 0x01,  // Collection: (Application)
    0x85, 0x02,  // Report Id (2)
    //
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0xE0,  //   Usage Min (224)
    0x29, 0xE7,  //   Usage Max (231)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x01,  //   Log Max (1)
    //
    //   Modifier byte
    0x75, 0x01,  //   Report Size (1)
    0x95, 0x08,  //   Report Count (8)
    0x81, 0x02,  //   Input: (Data, Variable, Absolute)
    //
    //   Reserved byte
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x08,  //   Report Size (8)
    0x81, 0x01,  //   Input: (Constant)
    //
    //   LED report
    0x05, 0x08,  //   Usage Pg (LEDs)
    0x19, 0x01,  //   Usage Min (1)
    0x29, 0x05,  //   Usage Max (5)
    0x95, 0x05,  //   Report Count (5)
    0x75, 0x01,  //   Report Size (1)
    0x91, 0x02,  //   Output: (Data, Variable, Absolute)
    //
    //   LED report padding
    0x95, 0x01,  //   Report Count (1)
    0x75, 0x03,  //   Report Size (3)
    0x91, 0x01,  //   Output: (Constant)
    //
    //   Key arrays (6 bytes)
    0x95, 0x06,  //   Report Count (6)
    0x75, 0x08,  //   Report Size (8)
    0x15, 0x00,  //   Log Min (0)
    0x25, 0x65,  //   Log Max (101)
    0x05, 0x07,  //   Usage Pg (Key Codes)
    0x19, 0x00,  //   Usage Min (0)
    0x29, 0x65,  //   Usage Max (101)
    0x81, 0x00,  //   Input: (Data, Array)
    //
    0xC0,        // End Collection
    //
    0x05, 0x0C,   // Usage Pg (Consumer Devices)
    0x09, 0x01,   // Usage (Consumer Control)
    0xA1, 0x01,   // Collection (Application)
    0x85, 0x03,   // Report Id (3)
    0x09, 0x02,   //   Usage (Numeric Key Pad)
    0xA1, 0x02,   //   Collection (Logical)
    0x05, 0x09,   //     Usage Pg (Button)
    0x19, 0x01,   //     Usage Min (Button 1)
    0x29, 0x0A,   //     Usage Max (Button 10)
    0x15, 0x01,   //     Logical Min (1)
    0x25, 0x0A,   //     Logical Max (10)
    0x75, 0x04,   //     Report Size (4)
    0x95, 0x01,   //     Report Count (1)
    0x81, 0x00,   //     Input (Data, Ary, Abs)
    0xC0,         //   End Collection
    0x05, 0x0C,   //   Usage Pg (Consumer Devices)
    0x09, 0x86,   //   Usage (Channel)
    0x15, 0xFF,   //   Logical Min (-1)
    0x25, 0x01,   //   Logical Max (1)
    0x75, 0x02,   //   Report Size (2)
    0x95, 0x01,   //   Report Count (1)
    0x81, 0x46,   //   Input (Data, Var, Rel, Null)
    0x09, 0xE9,   //   Usage (Volume Up)
    0x09, 0xEA,   //   Usage (Volume Down)
    0x15, 0x00,   //   Logical Min (0)
    0x75, 0x01,   //   Report Size (1)
    0x95, 0x02,   //   Report Count (2)
    0x81, 0x02,   //   Input (Data, Var, Abs)
    0x09, 0xE2,   //   Usage (Mute)
    0x09, 0x30,   //   Usage (Power)
    0x09, 0x83,   //   Usage (Recall Last)
    0x09, 0x81,   //   Usage (Assign Selection)
    0x09, 0xB0,   //   Usage (Play)
    0x09, 0xB1,   //   Usage (Pause)
    0x09, 0xB2,   //   Usage (Record)
    0x09, 0xB3,   //   Usage (Fast Forward)
    0x09, 0xB4,   //   Usage (Rewind)
    0x09, 0xB5,   //   Usage (Scan Next)
    0x09, 0xB6,   //   Usage (Scan Prev)
    0x09, 0xB7,   //   Usage (Stop)
    0x15, 0x01,   //   Logical Min (1)
    0x25, 0x0C,   //   Logical Max (12)
    0x75, 0x04,   //   Report Size (4)
    0x95, 0x01,   //   Report Count (1)
    0x81, 0x00,   //   Input (Data, Ary, Abs)
    0x09, 0x80,   //   Usage (Selection)
    0xA1, 0x02,   //   Collection (Logical)
    0x05, 0x09,   //     Usage Pg (Button)
    0x19, 0x01,   //     Usage Min (Button 1)
    0x29, 0x03,   //     Usage Max (Button 3)
    0x15, 0x01,   //     Logical Min (1)
    0x25, 0x03,   //     Logical Max (3)
    0x75, 0x02,   //     Report Size (2)
    0x81, 0x00,   //     Input (Data, Ary, Abs)
    0xC0,           //   End Collection
    0x81, 0x03,   //   Input (Const, Var, Abs)
    0xC0,            // End Collectionq
};
// HID report map length
uint8_t hidReportMapLen = sizeof(hidReportMap);
uint8_t hidProtocolMode = HID_PROTOCOL_MODE_REPORT;
// HID Information characteristic value
static const uint8_t hidInfo[HID_INFORMATION_LEN] = {
    LO_UINT16(0x0111), HI_UINT16(0x0111),             // bcdHID (USB HID version)
    0x00,                                             // bCountryCode
    HID_KBD_FLAGS                                     // Flags
};
// HID External Report Reference Descriptor
static uint16_t hidExtReportRefDesc = ESP_GATT_UUID_BATTERY_LEVEL;
// HID Report Reference characteristic descriptor, mouse input
static uint8_t hidReportRefMouseIn[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT };
// HID Report Reference characteristic descriptor, key input
static uint8_t hidReportRefKeyIn[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT };
// HID Report Reference characteristic descriptor, LED output
static uint8_t hidReportRefLedOut[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_LED_OUT, HID_REPORT_TYPE_OUTPUT };
// HID Report Reference characteristic descriptor, Feature
static uint8_t hidReportRefFeature[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_FEATURE, HID_REPORT_TYPE_FEATURE };
// HID Report Reference characteristic descriptor, consumer control input
static uint8_t hidReportRefCCIn[HID_REPORT_REF_LEN] =
             { HID_RPT_ID_CC_IN, HID_REPORT_TYPE_INPUT };
/// hid Service uuid
static uint16_t hid_le_svc = ATT_SVC_HID;
uint16_t            hid_count = 0;
esp_gatts_incl_svc_desc_t incl_svc = {0};
///the uuid definition
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t include_service_uuid = ESP_GATT_UUID_INCLUDE_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint16_t hid_info_char_uuid = ESP_GATT_UUID_HID_INFORMATION;
static const uint16_t hid_report_map_uuid    = ESP_GATT_UUID_HID_REPORT_MAP;
static const uint16_t hid_control_point_uuid = ESP_GATT_UUID_HID_CONTROL_POINT;
static const uint16_t hid_report_uuid = ESP_GATT_UUID_HID_REPORT;
static const uint16_t hid_proto_mode_uuid = ESP_GATT_UUID_HID_PROTO_MODE;
static const uint16_t hid_kb_input_uuid = ESP_GATT_UUID_HID_BT_KB_INPUT;
static const uint16_t hid_kb_output_uuid = ESP_GATT_UUID_HID_BT_KB_OUTPUT;
static const uint16_t hid_mouse_input_uuid = ESP_GATT_UUID_HID_BT_MOUSE_INPUT;
static const uint16_t hid_repot_map_ext_desc_uuid = ESP_GATT_UUID_EXT_RPT_REF_DESCR;
static const uint16_t hid_report_ref_descr_uuid = ESP_GATT_UUID_RPT_REF_DESCR;
///the propoty definition
//static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_write_nr = ESP_GATT_CHAR_PROP_BIT_WRITE_NR;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE|ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ|ESP_GATT_CHAR_PROP_BIT_NOTIFY;
//static const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ|ESP_GATT_CHAR_PROP_BIT_WRITE|ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read_write_write_nr = ESP_GATT_CHAR_PROP_BIT_READ|ESP_GATT_CHAR_PROP_BIT_WRITE|ESP_GATT_CHAR_PROP_BIT_WRITE_NR;
/// battary Service
static const uint16_t battary_svc = ESP_GATT_UUID_BATTERY_SERVICE_SVC;
static const uint16_t bat_lev_uuid = ESP_GATT_UUID_BATTERY_LEVEL;
static const uint8_t   bat_lev_ccc[2] ={ 0x00, 0x00};
static const uint16_t char_format_uuid = ESP_GATT_UUID_CHAR_PRESENT_FORMAT;
static uint8_t battary_lev = 50;
/// Full HRS Database Description - Used to add attributes into the database
static const esp_gatts_attr_db_t bas_att_db[BAS_IDX_NB] = {
    // Battary Service Declaration
    [BAS_IDX_SVC]               =  {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
                                            sizeof(uint16_t), sizeof(battary_svc), (uint8_t *)&battary_svc}},

    // Battary level Characteristic Declaration
    [BAS_IDX_BATT_LVL_CHAR]    = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
                                                   CHAR_DECLARATION_SIZE,CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_notify}},

    // Battary level Characteristic Value
    [BAS_IDX_BATT_LVL_VAL]             	= {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&bat_lev_uuid, ESP_GATT_PERM_READ,
                                                                sizeof(uint8_t),sizeof(uint8_t), &battary_lev}},

    // Battary level Characteristic - Client Characteristic Configuration Descriptor
    [BAS_IDX_BATT_LVL_NTF_CFG]     	=  {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
                                                          sizeof(uint16_t),sizeof(bat_lev_ccc), (uint8_t *)bat_lev_ccc}},

    // Battary level report Characteristic Declaration
    [BAS_IDX_BATT_LVL_PRES_FMT]  = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&char_format_uuid, ESP_GATT_PERM_READ,
                                                        sizeof(struct prf_char_pres_fmt), 0, NULL}},
};
/// Full Hid device Database Description - Used to add attributes into the database
static esp_gatts_attr_db_t hidd_le_gatt_db[HIDD_LE_IDX_NB] = {
            // HID Service Declaration
    [HIDD_LE_IDX_SVC]                       = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid,
                                                             ESP_GATT_PERM_READ_ENCRYPTED, sizeof(uint16_t), sizeof(hid_le_svc),
                                                            (uint8_t *)&hid_le_svc}},

    // HID Service Declaration
    [HIDD_LE_IDX_INCL_SVC]               = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&include_service_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            sizeof(esp_gatts_incl_svc_desc_t), sizeof(esp_gatts_incl_svc_desc_t),
                                                            (uint8_t *)&incl_svc}},

    // HID Information Characteristic Declaration
    [HIDD_LE_IDX_HID_INFO_CHAR]     = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                            (uint8_t *)&char_prop_read}},
    // HID Information Characteristic Value
    [HIDD_LE_IDX_HID_INFO_VAL]       = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_info_char_uuid,
                                                            ESP_GATT_PERM_READ,
                                                            sizeof(hids_hid_info_t), sizeof(hidInfo),
                                                            (uint8_t *)&hidInfo}},

    // HID Control Point Characteristic Declaration
    [HIDD_LE_IDX_HID_CTNL_PT_CHAR]  = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                              ESP_GATT_PERM_READ,
                                                              CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                              (uint8_t *)&char_prop_write_nr}},
    // HID Control Point Characteristic Value
    [HIDD_LE_IDX_HID_CTNL_PT_VAL]    = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_control_point_uuid,
                                                             ESP_GATT_PERM_WRITE,
                                                             sizeof(uint8_t), 0,
                                                             NULL}},

    // Report Map Characteristic Declaration
    [HIDD_LE_IDX_REPORT_MAP_CHAR]   = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                              ESP_GATT_PERM_READ,
                                                              CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                              (uint8_t *)&char_prop_read}},
    // Report Map Characteristic Value
    [HIDD_LE_IDX_REPORT_MAP_VAL]     = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_map_uuid,
                                                              ESP_GATT_PERM_READ,
                                                              HIDD_LE_REPORT_MAP_MAX_LEN, sizeof(hidReportMap),
                                                              (uint8_t *)&hidReportMap}},

    // Report Map Characteristic - External Report Reference Descriptor
    [HIDD_LE_IDX_REPORT_MAP_EXT_REP_REF]  = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_repot_map_ext_desc_uuid,
                                                                        ESP_GATT_PERM_READ,
                                                                        sizeof(uint16_t), sizeof(uint16_t),
                                                                        (uint8_t *)&hidExtReportRefDesc}},

    // Protocol Mode Characteristic Declaration
    [HIDD_LE_IDX_PROTO_MODE_CHAR]            = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                        ESP_GATT_PERM_READ,
                                                                        CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                        (uint8_t *)&char_prop_read_write}},
    // Protocol Mode Characteristic Value
    [HIDD_LE_IDX_PROTO_MODE_VAL]               = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_proto_mode_uuid,
                                                                        (ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE),
                                                                        sizeof(uint8_t), sizeof(hidProtocolMode),
                                                                        (uint8_t *)&hidProtocolMode}},

    [HIDD_LE_IDX_REPORT_MOUSE_IN_CHAR]       = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                         ESP_GATT_PERM_READ,
                                                                         CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                         (uint8_t *)&char_prop_read_notify}},

    [HIDD_LE_IDX_REPORT_MOUSE_IN_VAL]        = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       HIDD_LE_REPORT_MAX_LEN, 0,
                                                                       NULL}},

    [HIDD_LE_IDX_REPORT_MOUSE_IN_CCC]        = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
                                                                      (ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE),
                                                                      sizeof(uint16_t), 0,
                                                                      NULL}},

    [HIDD_LE_IDX_REPORT_MOUSE_REP_REF]       = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_ref_descr_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       sizeof(hidReportRefMouseIn), sizeof(hidReportRefMouseIn),
                                                                       hidReportRefMouseIn}},
    // Report Characteristic Declaration
    [HIDD_LE_IDX_REPORT_KEY_IN_CHAR]         = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                         ESP_GATT_PERM_READ,
                                                                         CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                         (uint8_t *)&char_prop_read_notify}},
    // Report Characteristic Value
    [HIDD_LE_IDX_REPORT_KEY_IN_VAL]            = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       HIDD_LE_REPORT_MAX_LEN, 0,
                                                                       NULL}},
    // Report KEY INPUT Characteristic - Client Characteristic Configuration Descriptor
    [HIDD_LE_IDX_REPORT_KEY_IN_CCC]              = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
                                                                      (ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE),
                                                                      sizeof(uint16_t), 0,
                                                                      NULL}},
     // Report Characteristic - Report Reference Descriptor
    [HIDD_LE_IDX_REPORT_KEY_IN_REP_REF]       = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_ref_descr_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       sizeof(hidReportRefKeyIn), sizeof(hidReportRefKeyIn),
                                                                       hidReportRefKeyIn}},
     // Report Characteristic Declaration
    [HIDD_LE_IDX_REPORT_LED_OUT_CHAR]         = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                         ESP_GATT_PERM_READ,
                                                                         CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                         (uint8_t *)&char_prop_read_write_write_nr}},
    [HIDD_LE_IDX_REPORT_LED_OUT_VAL]            = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_uuid,
                                                                       ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
                                                                       HIDD_LE_REPORT_MAX_LEN, 0,
                                                                       NULL}},
    [HIDD_LE_IDX_REPORT_LED_OUT_REP_REF]      =  {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_ref_descr_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       sizeof(hidReportRefLedOut), sizeof(hidReportRefLedOut),
                                                                       hidReportRefLedOut}},
    // Report Characteristic Declaration
    [HIDD_LE_IDX_REPORT_CC_IN_CHAR]         = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                         ESP_GATT_PERM_READ,
                                                                         CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                         (uint8_t *)&char_prop_read_notify}},
    // Report Characteristic Value
    [HIDD_LE_IDX_REPORT_CC_IN_VAL]            = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       HIDD_LE_REPORT_MAX_LEN, 0,
                                                                       NULL}},
    // Report KEY INPUT Characteristic - Client Characteristic Configuration Descriptor
    [HIDD_LE_IDX_REPORT_CC_IN_CCC]              = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
                                                                      (ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE_ENCRYPTED),
                                                                      sizeof(uint16_t), 0,
                                                                      NULL}},
     // Report Characteristic - Report Reference Descriptor
    [HIDD_LE_IDX_REPORT_CC_IN_REP_REF]       = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_ref_descr_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       sizeof(hidReportRefCCIn), sizeof(hidReportRefCCIn),
                                                                       hidReportRefCCIn}},
    // Boot Keyboard Input Report Characteristic Declaration
    [HIDD_LE_IDX_BOOT_KB_IN_REPORT_CHAR] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                        ESP_GATT_PERM_READ,
                                                                        CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                        (uint8_t *)&char_prop_read_notify}},
    // Boot Keyboard Input Report Characteristic Value
    [HIDD_LE_IDX_BOOT_KB_IN_REPORT_VAL]   = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_kb_input_uuid,
                                                                        ESP_GATT_PERM_READ,
                                                                        HIDD_LE_BOOT_REPORT_MAX_LEN, 0,
                                                                        NULL}},
    // Boot Keyboard Input Report Characteristic - Client Characteristic Configuration Descriptor
    [HIDD_LE_IDX_BOOT_KB_IN_REPORT_NTF_CFG]  = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
                                                                              (ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE),
                                                                              sizeof(uint16_t), 0,
                                                                              NULL}},
    // Boot Keyboard Output Report Characteristic Declaration
    [HIDD_LE_IDX_BOOT_KB_OUT_REPORT_CHAR]    = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                              ESP_GATT_PERM_READ,
                                                                              CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                              (uint8_t *)&char_prop_read_write}},
    // Boot Keyboard Output Report Characteristic Value
    [HIDD_LE_IDX_BOOT_KB_OUT_REPORT_VAL]      = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_kb_output_uuid,
                                                                              (ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE),
                                                                              HIDD_LE_BOOT_REPORT_MAX_LEN, 0,
                                                                              NULL}},
    // Boot Mouse Input Report Characteristic Declaration
    [HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_CHAR] = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                              ESP_GATT_PERM_READ,
                                                                              CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                              (uint8_t *)&char_prop_read_notify}},
    // Boot Mouse Input Report Characteristic Value
    [HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_VAL]   = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_mouse_input_uuid,
                                                                              ESP_GATT_PERM_READ,
                                                                              HIDD_LE_BOOT_REPORT_MAX_LEN, 0,
                                                                              NULL}},
    // Boot Mouse Input Report Characteristic - Client Characteristic Configuration Descriptor
    [HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_NTF_CFG]    = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid,
                                                                                      (ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE),
                                                                                      sizeof(uint16_t), 0,
                                                                                      NULL}},

    // Report Characteristic Declaration
    [HIDD_LE_IDX_REPORT_CHAR]                    = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
                                                                         ESP_GATT_PERM_READ,
                                                                         CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
                                                                         (uint8_t *)&char_prop_read_write}},
    // Report Characteristic Value
    [HIDD_LE_IDX_REPORT_VAL]                      = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       HIDD_LE_REPORT_MAX_LEN, 0,
                                                                       NULL}},
    // Report Characteristic - Report Reference Descriptor
    [HIDD_LE_IDX_REPORT_REP_REF]               = {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&hid_report_ref_descr_uuid,
                                                                       ESP_GATT_PERM_READ,
                                                                       sizeof(hidReportRefFeature), sizeof(hidReportRefFeature),
                                                                       hidReportRefFeature}},
};

/***************************hidd****************************/
static uint8_t hidd_service_uuid128[] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    //first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x12, 0x18, 0x00, 0x00,
};
static esp_ble_adv_data_t hidd_adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x03c0,       //HID Generic,
    .manufacturer_len = 0,
    .p_manufacturer_data =  NULL,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(hidd_service_uuid128),
    .p_service_uuid = hidd_service_uuid128,
    .flag = 0x6,
};
static esp_ble_adv_params_t hidd_adv_params = {
    .adv_int_min        = 0x20,
    .adv_int_max        = 0x30,
    .adv_type           = ADV_TYPE_IND,
    .own_addr_type      = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map        = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
static uint16_t hid_conn_id = 0;
static bool sec_conn = false;
ble_status_t status = BLE_OFF;

/*==================[external data definition]===============================*/
/********************esp_hidd_prf_api**********************/


/*************************hid_dev**************************/


/*************************hidd_le**************************/

/***************************hidd****************************/

/*==================[internal functions definition]==========================*/
/********************esp_hidd_prf_api**********************/
/**
 * @brief 
 */
esp_err_t esp_hidd_register_callbacks(esp_hidd_event_cb_t callbacks){
    esp_err_t hidd_status;
    if(callbacks != NULL) {
   	    hidd_le_env.hidd_cb = callbacks;
    } else {
        return ESP_FAIL;
    }
    if((hidd_status = hidd_register_cb()) != ESP_OK) {
        return hidd_status;
    }
    esp_ble_gatts_app_register(BATTRAY_APP_ID);
    if((hidd_status = esp_ble_gatts_app_register(HIDD_APP_ID)) != ESP_OK) {
        return hidd_status;
    }
    return hidd_status;
}
/**
 * @brief 
 */
esp_err_t esp_hidd_profile_init(void){
     if (hidd_le_env.enabled) {
        ESP_LOGE(TAG, "HID device profile already initialized");
        return ESP_FAIL;
    }
    // Reset the hid device target environment
    memset(&hidd_le_env, 0, sizeof(hidd_le_env_t));
    hidd_le_env.enabled = true;
    return ESP_OK;
}
/**
 * @brief 
 */
esp_err_t esp_hidd_profile_deinit(void){
    uint16_t hidd_svc_hdl = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_SVC];
    if (!hidd_le_env.enabled) {
        ESP_LOGE(TAG, "HID device profile already initialized");
        return ESP_OK;
    }
    if(hidd_svc_hdl != 0) {
	esp_ble_gatts_stop_service(hidd_svc_hdl);
	esp_ble_gatts_delete_service(hidd_svc_hdl);
    } else {
	return ESP_FAIL;
   }
    /* register the HID device profile to the BTA_GATTS module*/
    esp_ble_gatts_app_unregister(hidd_le_env.gatt_if);
    return ESP_OK;
}
/**
 * @brief 
 */
uint16_t esp_hidd_get_version(void){
	return HIDD_VERSION;
}

/*************************hid_dev**************************/
static hid_report_map_t *hid_dev_rpt_by_id(uint8_t id, uint8_t type){
    hid_report_map_t *rpt = hid_dev_rpt_tbl;

    for (uint8_t i = hid_dev_rpt_tbl_Len; i > 0; i--, rpt++) {
        if (rpt->id == id && rpt->type == type && rpt->mode == hidProtocolMode) {
            return rpt;
        }
    }
    return NULL;
}
void hid_dev_register_reports(uint8_t num_reports, hid_report_map_t *p_report){
    hid_dev_rpt_tbl = p_report;
    hid_dev_rpt_tbl_Len = num_reports;
    return;
}
void hid_dev_send_report(esp_gatt_if_t gatts_if, uint16_t conn_id,
                                    uint8_t id, uint8_t type, uint8_t length, uint8_t *data){
    hid_report_map_t *p_rpt;
    // get att handle for report
    if ((p_rpt = hid_dev_rpt_by_id(id, type)) != NULL) {
        // if notifications are enabled
        ESP_LOGD(TAG, "%s(), send the report, handle = %d", __func__, p_rpt->handle);
        esp_ble_gatts_send_indicate(gatts_if, conn_id, p_rpt->handle, length, data, false);
    }
    return;
}

/*************************hidd_le**************************/
void esp_hidd_prf_cb_hdl(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
									esp_ble_gatts_cb_param_t *param){
    switch(event) {
        case ESP_GATTS_REG_EVT: {
            esp_ble_gap_config_local_icon (ESP_BLE_APPEARANCE_GENERIC_HID);
            esp_hidd_cb_param_t hidd_param;
            hidd_param.init_finish.state = param->reg.status;
            if(param->reg.app_id == HIDD_APP_ID) {
                hidd_le_env.gatt_if = gatts_if;
                if(hidd_le_env.hidd_cb != NULL) {
                    (hidd_le_env.hidd_cb)(ESP_HIDD_EVENT_REG_FINISH, &hidd_param);
                    hidd_le_create_service(hidd_le_env.gatt_if);
                }
            }
            if(param->reg.app_id == BATTRAY_APP_ID) {
                hidd_param.init_finish.gatts_if = gatts_if;
                 if(hidd_le_env.hidd_cb != NULL) {
                    (hidd_le_env.hidd_cb)(ESP_BAT_EVENT_REG, &hidd_param);
                }

            }

            break;
        }
        case ESP_GATTS_CONF_EVT: {
            break;
        }
        case ESP_GATTS_CREATE_EVT:
            break;
        case ESP_GATTS_CONNECT_EVT: {
            esp_hidd_cb_param_t cb_param = {0};
			ESP_LOGI(TAG, "HID connection establish, conn_id = %x",param->connect.conn_id);
			memcpy(cb_param.connect.remote_bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            cb_param.connect.conn_id = param->connect.conn_id;
            hidd_clcb_alloc(param->connect.conn_id, param->connect.remote_bda);
            esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_NO_MITM);
            if(hidd_le_env.hidd_cb != NULL) {
                (hidd_le_env.hidd_cb)(ESP_HIDD_EVENT_BLE_CONNECT, &cb_param);
            }
            break;
        }
        case ESP_GATTS_DISCONNECT_EVT: {
			 if(hidd_le_env.hidd_cb != NULL) {
                    (hidd_le_env.hidd_cb)(ESP_HIDD_EVENT_BLE_DISCONNECT, NULL);
             }
            hidd_clcb_dealloc(param->disconnect.conn_id);
            break;
        }
        case ESP_GATTS_CLOSE_EVT:
            break;
        case ESP_GATTS_WRITE_EVT: {
            esp_hidd_cb_param_t cb_param = {0};
            if (param->write.handle == hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_LED_OUT_VAL]) {
                cb_param.led_write.conn_id = param->write.conn_id;
                cb_param.led_write.report_id = HID_RPT_ID_LED_OUT;
                cb_param.led_write.length = param->write.len;
                cb_param.led_write.data = param->write.value;
                (hidd_le_env.hidd_cb)(ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT, &cb_param);
            }
            break;
        }
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            if (param->add_attr_tab.num_handle == BAS_IDX_NB &&
                param->add_attr_tab.svc_uuid.uuid.uuid16 == ESP_GATT_UUID_BATTERY_SERVICE_SVC &&
                param->add_attr_tab.status == ESP_GATT_OK) {
                incl_svc.start_hdl = param->add_attr_tab.handles[BAS_IDX_SVC];
                incl_svc.end_hdl = incl_svc.start_hdl + BAS_IDX_NB -1;
                ESP_LOGI(TAG, "%s(), start added the hid service to the stack database. incl_handle = %d",
                           __func__, incl_svc.start_hdl);
                esp_ble_gatts_create_attr_tab(hidd_le_gatt_db, gatts_if, HIDD_LE_IDX_NB, 0);
            }
            if (param->add_attr_tab.num_handle == HIDD_LE_IDX_NB &&
                param->add_attr_tab.status == ESP_GATT_OK) {
                memcpy(hidd_le_env.hidd_inst.att_tbl, param->add_attr_tab.handles,
                            HIDD_LE_IDX_NB*sizeof(uint16_t));
                ESP_LOGI(TAG, "hid svc handle = %x",hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_SVC]);
                hid_add_id_tbl();
		        esp_ble_gatts_start_service(hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_SVC]);
            } else {
                esp_ble_gatts_start_service(param->add_attr_tab.handles[0]);
            }
            break;
         }
        default:
            break;
    }
}
void hidd_le_create_service(esp_gatt_if_t gatts_if){
    /* Here should added the battery service first, because the hid service should include the battery service.
       After finish to added the battery service then can added the hid service. */
    esp_ble_gatts_create_attr_tab(bas_att_db, gatts_if, BAS_IDX_NB, 0);

}
void hidd_le_init(void){
    // Reset the hid device target environment
    memset(&hidd_le_env, 0, sizeof(hidd_le_env_t));
}
void hidd_clcb_alloc (uint16_t conn_id, esp_bd_addr_t bda){
    uint8_t                   i_clcb = 0;
    hidd_clcb_t      *p_clcb = NULL;
    for (i_clcb = 0, p_clcb= hidd_le_env.hidd_clcb; i_clcb < HID_MAX_APPS; i_clcb++, p_clcb++) {
        if (!p_clcb->in_use) {
            p_clcb->in_use      = true;
            p_clcb->conn_id     = conn_id;
            p_clcb->connected   = true;
            memcpy (p_clcb->remote_bda, bda, ESP_BD_ADDR_LEN);
            break;
        }
    }
    return;
}
bool hidd_clcb_dealloc (uint16_t conn_id){
    uint8_t              i_clcb = 0;
    hidd_clcb_t      *p_clcb = NULL;
    for (i_clcb = 0, p_clcb= hidd_le_env.hidd_clcb; i_clcb < HID_MAX_APPS; i_clcb++, p_clcb++) {
            memset(p_clcb, 0, sizeof(hidd_clcb_t));
            return true;
    }
    return false;
}
static struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] = {
        .gatts_cb = esp_hidd_prf_cb_hdl,
        .gatts_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param){
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            heart_rate_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGI(TAG, "Reg app failed, app_id %04x, status %d\n",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                    gatts_if == heart_rate_profile_tab[idx].gatts_if) {
                if (heart_rate_profile_tab[idx].gatts_cb) {
                    heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}
esp_err_t hidd_register_cb(void){
	esp_err_t status;
	status = esp_ble_gatts_register_callback(gatts_event_handler);
	return status;
}
void hidd_set_attr_value(uint16_t handle, uint16_t val_len, const uint8_t *value){
    hidd_inst_t *hidd_inst = &hidd_le_env.hidd_inst;
    if(hidd_inst->att_tbl[HIDD_LE_IDX_HID_INFO_VAL] <= handle &&
        hidd_inst->att_tbl[HIDD_LE_IDX_REPORT_REP_REF] >= handle) {
        esp_ble_gatts_set_attr_value(handle, val_len, value);
    } else {
        ESP_LOGE(TAG, "%s error:Invalid handle value.",__func__);
    }
    return;
}
void hidd_get_attr_value(uint16_t handle, uint16_t *length, uint8_t **value){
    hidd_inst_t *hidd_inst = &hidd_le_env.hidd_inst;
    if(hidd_inst->att_tbl[HIDD_LE_IDX_HID_INFO_VAL] <= handle &&
        hidd_inst->att_tbl[HIDD_LE_IDX_REPORT_REP_REF] >= handle){
        esp_ble_gatts_get_attr_value(handle, length, (const uint8_t **)value);
    } else {
        ESP_LOGE(TAG, "%s error:Invalid handle value.", __func__);
    }
    return;
}
static void hid_add_id_tbl(void){
    // Mouse input report
    hid_rpt_map[0].id = hidReportRefMouseIn[0];
    hid_rpt_map[0].type = hidReportRefMouseIn[1];
    hid_rpt_map[0].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_MOUSE_IN_VAL];
    hid_rpt_map[0].cccdHandle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_MOUSE_IN_VAL];
    hid_rpt_map[0].mode = HID_PROTOCOL_MODE_REPORT;
    // Key input report
    hid_rpt_map[1].id = hidReportRefKeyIn[0];
    hid_rpt_map[1].type = hidReportRefKeyIn[1];
    hid_rpt_map[1].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_KEY_IN_VAL];
    hid_rpt_map[1].cccdHandle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_KEY_IN_CCC];
    hid_rpt_map[1].mode = HID_PROTOCOL_MODE_REPORT;
    // Consumer Control input report
    hid_rpt_map[2].id = hidReportRefCCIn[0];
    hid_rpt_map[2].type = hidReportRefCCIn[1];
    hid_rpt_map[2].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_CC_IN_VAL];
    hid_rpt_map[2].cccdHandle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_CC_IN_CCC];
    hid_rpt_map[2].mode = HID_PROTOCOL_MODE_REPORT;
    // LED output report
    hid_rpt_map[3].id = hidReportRefLedOut[0];
    hid_rpt_map[3].type = hidReportRefLedOut[1];
    hid_rpt_map[3].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_LED_OUT_VAL];
    hid_rpt_map[3].cccdHandle = 0;
    hid_rpt_map[3].mode = HID_PROTOCOL_MODE_REPORT;
    // Boot keyboard input report
    // Use same ID and type as key input report
    hid_rpt_map[4].id = hidReportRefKeyIn[0];
    hid_rpt_map[4].type = hidReportRefKeyIn[1];
    hid_rpt_map[4].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_BOOT_KB_IN_REPORT_VAL];
    hid_rpt_map[4].cccdHandle = 0;
    hid_rpt_map[4].mode = HID_PROTOCOL_MODE_BOOT;
    // Boot keyboard output report
    // Use same ID and type as LED output report
    hid_rpt_map[5].id = hidReportRefLedOut[0];
    hid_rpt_map[5].type = hidReportRefLedOut[1];
    hid_rpt_map[5].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_BOOT_KB_OUT_REPORT_VAL];
    hid_rpt_map[5].cccdHandle = 0;
    hid_rpt_map[5].mode = HID_PROTOCOL_MODE_BOOT;
    // Boot mouse input report
    // Use same ID and type as mouse input report
    hid_rpt_map[6].id = hidReportRefMouseIn[0];
    hid_rpt_map[6].type = hidReportRefMouseIn[1];
    hid_rpt_map[6].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_BOOT_MOUSE_IN_REPORT_VAL];
    hid_rpt_map[6].cccdHandle = 0;
    hid_rpt_map[6].mode = HID_PROTOCOL_MODE_BOOT;
    // Feature report
    hid_rpt_map[7].id = hidReportRefFeature[0];
    hid_rpt_map[7].type = hidReportRefFeature[1];
    hid_rpt_map[7].handle = hidd_le_env.hidd_inst.att_tbl[HIDD_LE_IDX_REPORT_VAL];
    hid_rpt_map[7].cccdHandle = 0;
    hid_rpt_map[7].mode = HID_PROTOCOL_MODE_REPORT;
    // Setup report ID map
    hid_dev_register_reports(HID_NUM_REPORTS, hid_rpt_map);
}

/***************************hidd****************************/
static void hidd_event_callback(esp_hidd_cb_event_t event, esp_hidd_cb_param_t *param){
    switch(event) {
        case ESP_HIDD_EVENT_REG_FINISH: {
            if (param->init_finish.state == ESP_HIDD_INIT_OK) {
                //esp_bd_addr_t rand_addr = {0x04,0x11,0x11,0x11,0x11,0x05};
                esp_ble_gap_set_device_name(device_name);
                esp_ble_gap_config_adv_data(&hidd_adv_data);

            }
            break;
        }
        case ESP_BAT_EVENT_REG: {
            break;
        }
        case ESP_HIDD_EVENT_DEINIT_FINISH:
	     break;
		case ESP_HIDD_EVENT_BLE_CONNECT: {
            ESP_LOGI(TAG, "ESP_HIDD_EVENT_BLE_CONNECT");
            hid_conn_id = param->connect.conn_id;
            break;
        }
        case ESP_HIDD_EVENT_BLE_DISCONNECT: {
            status = BLE_DISCONNECTED;
            sec_conn = false;
            ESP_LOGI(TAG, "ESP_HIDD_EVENT_BLE_DISCONNECT");
            status = BLE_DISCONNECTED;
            esp_ble_gap_start_advertising(&hidd_adv_params);
            break;
        }
        case ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT: {
            ESP_LOGI(TAG, "%s, ESP_HIDD_EVENT_BLE_VENDOR_REPORT_WRITE_EVT", __func__);
            ESP_LOG_BUFFER_HEX(TAG, param->vendor_write.data, param->vendor_write.length);
            break;
        }
        case ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT: {
            ESP_LOGI(TAG, "ESP_HIDD_EVENT_BLE_LED_REPORT_WRITE_EVT");
            ESP_LOG_BUFFER_HEX(TAG, param->led_write.data, param->led_write.length);
            break;
        }
        default:
            break;
    }
    return;
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){
    switch (event) {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
        status = BLE_DISCONNECTED;
        esp_ble_gap_start_advertising(&hidd_adv_params);
        break;
     case ESP_GAP_BLE_SEC_REQ_EVT:
        for(int i = 0; i < ESP_BD_ADDR_LEN; i++) {
             ESP_LOGD(TAG, "%x:",param->ble_security.ble_req.bd_addr[i]);
        }
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
	 break;
     case ESP_GAP_BLE_AUTH_CMPL_EVT:
        status = BLE_CONNECTED;
        sec_conn = true;
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        ESP_LOGI(TAG, "remote BD_ADDR: %08x%04x",\
                (bd_addr[0] << 24) + (bd_addr[1] << 16) + (bd_addr[2] << 8) + bd_addr[3],
                (bd_addr[4] << 8) + bd_addr[5]);
        ESP_LOGI(TAG, "address type = %d", param->ble_security.auth_cmpl.addr_type);
        ESP_LOGI(TAG, "pair status = %s",param->ble_security.auth_cmpl.success ? "success" : "fail");
        if(!param->ble_security.auth_cmpl.success) {
            ESP_LOGE(TAG, "fail reason = 0x%x",param->ble_security.auth_cmpl.fail_reason);
        }
        break;
    default:
        break;
    }
}

/*==================[external functions definition]==========================*/
void BleHidInit(char * hid_dev_name){
    esp_err_t ret;
    device_name = hid_dev_name;
    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "%s initialize controller failed\n", __func__);
        return;
    }
    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "%s enable controller failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluedroid failed\n", __func__);
        return;
    }
    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "%s init bluedroid failed\n", __func__);
        return;
    }
    if((ret = esp_hidd_profile_init()) != ESP_OK) {
        ESP_LOGE(TAG, "%s init bluedroid failed\n", __func__);
    }
    ///register the callback function to the gap module
    esp_ble_gap_register_callback(gap_event_handler);
    esp_hidd_register_callbacks(hidd_event_callback);
    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_BOND;     //bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;           //set the IO capability to No output No input
    uint8_t key_size = 16;      //the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
    esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}

ble_status_t BleHidStatus(void){
	return status;
}

void BleHidSendKeyboard(key_mask_t special_key_mask, keyboard_cmd_t *keyboard_cmd, uint8_t num_key){
    if (num_key > HID_KEYBOARD_IN_RPT_LEN - 2) {
        ESP_LOGE(TAG, "%s(), the number key should not be more than %d", __func__, HID_KEYBOARD_IN_RPT_LEN);
        return;
    }
    uint8_t buffer[HID_KEYBOARD_IN_RPT_LEN] = {0};
    if(status == BLE_CONNECTED){
        buffer[0] = special_key_mask;
        for (int i = 0; i < num_key; i++) {
            buffer[i+2] = keyboard_cmd[i];
        }
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
                            HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT, HID_KEYBOARD_IN_RPT_LEN, buffer);
        for (int i = 0; i < num_key; i++) {
            buffer[i+2] = 0;
        }
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
                            HID_RPT_ID_KEY_IN, HID_REPORT_TYPE_INPUT, HID_KEYBOARD_IN_RPT_LEN, buffer);
    }
    return;
}

void BleHidSendMouse(mouse_cmd_t mouse_button, int8_t delta_x, int8_t delta_y){
    uint8_t buffer[HID_MOUSE_IN_RPT_LEN];
    buffer[0] = mouse_button;       // Buttons
    buffer[1] = delta_x;            // X
    buffer[2] = delta_y;            // Y
    buffer[3] = 0;                  // Wheel
    buffer[4] = 0;                  // AC Pan
    if(status == BLE_CONNECTED){
        hid_dev_send_report(hidd_le_env.gatt_if, hid_conn_id,
                            HID_RPT_ID_MOUSE_IN, HID_REPORT_TYPE_INPUT, HID_MOUSE_IN_RPT_LEN, buffer);
    }
    return;
}

/*==================[end of file]============================================*/