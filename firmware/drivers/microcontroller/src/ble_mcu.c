/**
 * @file ble_mcu.c
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2024-03-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/*==================[inclusions]=============================================*/
#include "ble_mcu.h"
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
#define TAG "ble_mcu"
#define MTU_MAX_BYTES		20	 /* GATT Maximum Transmission Unit */
#define PAYLOAD_SIZE        128  /* Maximun number of bytes transmitted in one transaction */
#define SPP_PROFILE_NUM     1       
#define SPP_PROFILE_APP_IDX 0
#define ESP_SPP_APP_ID      0x56
#define SPP_SVC_INST_ID     0
#define SPP_DATA_MAX_LEN    (128) /* Maximun number of bytes transmitted in one transaction */
/* List of attributes to be added to the service database */
enum{
    SPP_IDX_SVC,
    SPP_IDX_SPP_DATA_RECV_NOTIFY_CHAR,
    SPP_IDX_SPP_DATA_NOTIFY_VAL,
    SPP_IDX_SPP_DATA_NOTIFY_CFG,
    SPP_IDX_SPP_DATA_RECV_VAL,
    SPP_IDX_SPP_DATA_RECV_CFG,
    SPP_IDX_NB,
};
/* Characteristics UUID */
#define ESP_GATT_UUID_SPP_SERVICE               0xFFE0  /* Service ID */
#define ESP_GATT_UUID_SPP_DATA_RECEIVE_NOTIFY   0xFFE1  /* Characteristic ID */

#define ADV_CONFIG_FLAG			                (1 << 0)
#define SCAN_RSP_CONFIG_FLAG	                (1 << 1)
/*==================[typedef]================================================*/
/* Commands for handling Bluetooth events */
typedef enum {
    CMD_BLUETOOTH_CONNECT,       /* bt connection */
    CMD_BLUETOOTH_AUTH,          /* device authentification */
    CMD_BLUETOOTH_DATA,          /* data reception */
    CMD_BLUETOOTH_DISCONNECT,    /* device disconnection */
    CMD_SEND_DATA,               /* data transmission */
} comd_bt_ev_t;
/* Struct used to handle Bluetooth events */
typedef struct {
	uint16_t spp_conn_id;
	esp_gatt_if_t spp_gatts_if;
	uint16_t command;
	size_t length;
	uint8_t payload[PAYLOAD_SIZE];
	TaskHandle_t taskHandle;
} CMD_t;
/*==================[internal data declaration]==============================*/
char * device_name; /* Device name */
void (*ble_read_isr_p)(uint8_t * data, uint8_t length);  /* Pointer to callback function for reading data */
ble_status_t status = BLE_OFF;
static uint16_t spp_handle_table[SPP_IDX_NB];   /* Service database table */
/* GATT profile struct */
struct gatts_profile_inst {
	esp_gatts_cb_t gatts_cb;
	uint16_t gatts_if;
	uint16_t app_id;
	uint16_t conn_id;
	uint16_t service_handle;
	esp_gatt_srvc_id_t service_id;
	uint16_t char_handle;
	esp_bt_uuid_t char_uuid;
	esp_gatt_perm_t perm;
	esp_gatt_char_prop_t property;
	uint16_t descr_handle;
	esp_bt_uuid_t descr_uuid;
};
QueueHandle_t xQueueEvents = NULL;  /* Queue for handling Bluettoth events */
QueueHandle_t xQueueRead = NULL;    /* Queue for handling received data */

/*==================[internal functions declaration]=========================*/
static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
										esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
/*==================[internal data definition]===============================*/
static const uint16_t spp_service_uuid = ESP_GATT_UUID_SPP_SERVICE; /* Service ID */
/* Advertising data */
static const uint8_t spp_adv_data[23] = {
	/* Flags */
	0x02,0x01,0x06,
	/* Complete List of 16-bit Service Class UUIDs */
	0x03,0x03,0xF0,0xAB,
	/* Complete Local Name in advertising */
	0x0F,0x09, 'E', 'S', 'P', '_', 'E', 'D', 'U', '_', 'S', 'E', 'R','V', 'E', 'R'
};
/* Advertising parameters */
static esp_ble_adv_params_t spp_adv_params = {
	.adv_int_min		= 0x20,
	.adv_int_max		= 0x40,
	.adv_type			= ADV_TYPE_IND,
	.own_addr_type		= BLE_ADDR_TYPE_PUBLIC,
	.channel_map		= ADV_CHNL_ALL,
	.adv_filter_policy	= ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};
/* Service UUID */
static uint8_t sec_service_uuid[16] = {
	/* LSB <--------------------------------------------------------------------------------> MSB */
	//first uuid, 16bit, [12],[13] is the value
	0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xE0, 0xFF, 0x00, 0x00,
};
/* Config advertising data */
static esp_ble_adv_data_t spp_adv_config = {
	.set_scan_rsp = false,
	.include_txpower = true,
	.min_interval = 0x0006, //slave connection min interval, Time = min_interval * 1.25 msec
	.max_interval = 0x0010, //slave connection max interval, Time = max_interval * 1.25 msec
	.appearance = 0x00,
	.manufacturer_len = 0, 
	.p_manufacturer_data =	NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = sizeof(sec_service_uuid),
	.p_service_uuid = sec_service_uuid,
	.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
/* config scan response data */
static esp_ble_adv_data_t spp_scan_rsp_config = {
	.set_scan_rsp = true,
	.include_name = true,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
};
/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst spp_profile_tab[SPP_PROFILE_NUM] = {
	[SPP_PROFILE_APP_IDX] = {
		.gatts_cb = gatts_profile_event_handler,
		.gatts_if = ESP_GATT_IF_NONE,		/* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
	},
};
/* Service and characteristics id and values */
static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_description_uuid = ESP_GATT_UUID_CHAR_DESCRIPTION;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_WRITE_NR|ESP_GATT_CHAR_PROP_BIT_READ|ESP_GATT_CHAR_PROP_BIT_NOTIFY;;
static const uint16_t spp_data_receive_uuid = ESP_GATT_UUID_SPP_DATA_RECEIVE_NOTIFY;
static const uint8_t  spp_data_receive_val[20] = {0x00};
static const uint16_t spp_data_notify_uuid = ESP_GATT_UUID_SPP_DATA_RECEIVE_NOTIFY;
static const uint8_t  spp_data_notify_val[20] = {0x00};
static const uint8_t  spp_data_notify_ccc[2] = {0x00, 0x00};
/* Full HRS Database Description - Used to add attributes into the database */
static const esp_gatts_attr_db_t spp_gatt_db[SPP_IDX_NB] = {
	/* SPP -  Service Declaration */
	[SPP_IDX_SVC]						=
	{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
	sizeof(spp_service_uuid), sizeof(spp_service_uuid), (uint8_t *)&spp_service_uuid}},

	/* SPP -  data receive and notify characteristic Declaration */
	[SPP_IDX_SPP_DATA_RECV_NOTIFY_CHAR]			=
	{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
	(sizeof(uint8_t)),(sizeof(uint8_t)), (uint8_t *)&char_prop_read_write}},

	/* SPP -  data notify characteristic Value */
	[SPP_IDX_SPP_DATA_NOTIFY_VAL]	=
	{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&spp_data_notify_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
	SPP_DATA_MAX_LEN, sizeof(spp_data_notify_val), (uint8_t *)spp_data_notify_val}},

	/* SPP -  data notify characteristic - Client Characteristic Configuration Descriptor */
	[SPP_IDX_SPP_DATA_NOTIFY_CFG]		  =
	{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
	sizeof(uint16_t),sizeof(spp_data_notify_ccc), (uint8_t *)spp_data_notify_ccc}},

	/* SPP -  data receive characteristic Value */
	[SPP_IDX_SPP_DATA_RECV_VAL]					=
	{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&spp_data_receive_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
	SPP_DATA_MAX_LEN,sizeof(spp_data_receive_val), (uint8_t *)spp_data_receive_val}},
	
	/* SPP -  data notify characteristic - Client Characteristic Configuration Descriptor */
	[SPP_IDX_SPP_DATA_RECV_CFG]		  =
	{{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_description_uuid, ESP_GATT_PERM_READ|ESP_GATT_PERM_WRITE,
	sizeof(uint16_t),sizeof(spp_data_notify_ccc), (uint8_t *)spp_data_notify_ccc}},
};
/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
	CMD_t cmdBuf;
	static uint8_t adv_config_done = 0;
	switch (event) {
		case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
			adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
			if (adv_config_done == 0){
				esp_ble_gap_start_advertising(&spp_adv_params);
				status = BLE_DISCONNECTED;
			}
			break;
		case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
			adv_config_done &= (~ADV_CONFIG_FLAG);
			if (adv_config_done == 0){
				esp_ble_gap_start_advertising(&spp_adv_params);
				status = BLE_DISCONNECTED;
			}
			break;
		case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
			//advertising start complete event to indicate advertising start successfully or failed
			if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
				ESP_LOGE(__FUNCTION__, "advertising start failed, error status = %x", param->adv_start_cmpl.status);
				break;
			}
			ESP_LOGI(TAG, "Advertising start");
			break;
		case ESP_GAP_BLE_PASSKEY_REQ_EVT:							/* passkey request event */
			
            break;
		case ESP_GAP_BLE_OOB_REQ_EVT: {
			uint8_t tk[16] = {1};   //If you paired with OOB, both devices need to use the same tk
			esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
			break;
		}
		case ESP_GAP_BLE_LOCAL_IR_EVT:								 /* BLE local IR event */
			
			break;
		case ESP_GAP_BLE_LOCAL_ER_EVT:								 /* BLE local ER event */

			break;
		case ESP_GAP_BLE_NC_REQ_EVT:
			/* The app will receive this evt when the IO has DisplayYesNO capability and the peer device IO also has DisplayYesNo capability.
			show the passkey number to the user to confirm it with the number displayed by peer device. */
			esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
			//ESP_LOGI(__FUNCTION__, "ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number:%"PRIu32, param->ble_security.key_notif.passkey);
			break;
		case ESP_GAP_BLE_SEC_REQ_EVT:
			/* send the positive(true) security response to the peer device to accept the security request.
			If not accept the security request, should send the security response with negative(false) accept value*/
			esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
			break;
		case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:  

			break;
		case ESP_GAP_BLE_KEY_EVT:

			break;
		case ESP_GAP_BLE_AUTH_CMPL_EVT: {
			cmdBuf.command = CMD_BLUETOOTH_AUTH;
			xQueueSend(xQueueEvents, &cmdBuf, 0);
			break;
	}
	case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: {
		ESP_LOGD(__FUNCTION__, "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT status = %d", param->remove_bond_dev_cmpl.status);
		ESP_LOGI(__FUNCTION__, "ESP_GAP_BLE_REMOVE_BOND_DEV");
		ESP_LOGI(__FUNCTION__, "-----ESP_GAP_BLE_REMOVE_BOND_DEV----");
		esp_log_buffer_hex(__FUNCTION__, (void *)param->remove_bond_dev_cmpl.bd_addr, sizeof(esp_bd_addr_t));
		ESP_LOGI(__FUNCTION__, "------------------------------------");
		break;
	}
	case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT:
		if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS){
			ESP_LOGE(__FUNCTION__, "config local privacy failed, error status = %x", param->local_privacy_cmpl.status);
			break;
		}
		esp_err_t ret = esp_ble_gap_config_adv_data(&spp_adv_config);
		if (ret){
			ESP_LOGE(__FUNCTION__, "config adv data failed, error code = %x", ret);
		}else{
			adv_config_done |= ADV_CONFIG_FLAG;
		}
		ret = esp_ble_gap_config_adv_data(&spp_scan_rsp_config);
		if (ret){
			ESP_LOGE(__FUNCTION__, "config adv data failed, error code = %x", ret);
		}else{
			adv_config_done |= SCAN_RSP_CONFIG_FLAG;
		}
		break;
	default:
		break;
	}
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
										esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *) param;
	CMD_t cmdBuf;

	switch (event) {
		case ESP_GATTS_REG_EVT:
			esp_ble_gap_set_device_name(device_name);
			//generate a resolvable random address
			esp_ble_gap_config_local_privacy(true);
			esp_ble_gap_config_adv_data_raw((uint8_t *)spp_adv_data, sizeof(spp_adv_data));
			esp_ble_gatts_create_attr_tab(spp_gatt_db, gatts_if, SPP_IDX_NB, SPP_SVC_INST_ID);
			break;
		case ESP_GATTS_READ_EVT:
			break;
		case ESP_GATTS_WRITE_EVT:
			cmdBuf.command = CMD_BLUETOOTH_DATA;
			memcpy(cmdBuf.payload, param->write.value, param->write.len);
			cmdBuf.length = param->write.len;
			xQueueSend(xQueueRead, &cmdBuf, 0);
			break;
		case ESP_GATTS_EXEC_WRITE_EVT:
			break;
		case ESP_GATTS_MTU_EVT:
			break;
		case ESP_GATTS_CONF_EVT:
			break;
		case ESP_GATTS_UNREG_EVT:
			break;
		case ESP_GATTS_DELETE_EVT:
			break;
		case ESP_GATTS_START_EVT:
			break;
		case ESP_GATTS_STOP_EVT:
			break;
		case ESP_GATTS_CONNECT_EVT:
			/* start security connect with peer device when receive the connect event sent by the master */
			esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
			cmdBuf.command = CMD_BLUETOOTH_CONNECT;
			cmdBuf.spp_conn_id = p_data->connect.conn_id;
			cmdBuf.spp_gatts_if = gatts_if;
			xQueueSend(xQueueEvents, &cmdBuf, portMAX_DELAY);
			break;
		case ESP_GATTS_DISCONNECT_EVT:
			cmdBuf.command = CMD_BLUETOOTH_DISCONNECT;
			status = BLE_DISCONNECTED;
			xQueueSend(xQueueEvents, &cmdBuf, portMAX_DELAY);
			/* start advertising again when missing the connect */
			esp_ble_gap_start_advertising(&spp_adv_params);
			break;
		case ESP_GATTS_OPEN_EVT:
			break;
		case ESP_GATTS_CANCEL_OPEN_EVT:
			break;
		case ESP_GATTS_CLOSE_EVT:
			break;
		case ESP_GATTS_LISTEN_EVT:
			break;
		case ESP_GATTS_CONGEST_EVT:
			break;
		case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
			if (param->create.status == ESP_GATT_OK){
				if(param->add_attr_tab.num_handle == SPP_IDX_NB) {
					memcpy(spp_handle_table, param->add_attr_tab.handles,
					sizeof(spp_handle_table));
					esp_ble_gatts_start_service(spp_handle_table[SPP_IDX_SVC]);
				}else{
					ESP_LOGE(__FUNCTION__, "Create attribute table abnormally, num_handle (%d) doesn't equal to SPP_IDX_NB(%d)",
						param->add_attr_tab.num_handle, SPP_IDX_NB);
				}
			}else{
				ESP_LOGE(__FUNCTION__, " Create attribute table failed, error code = %x", param->create.status);
			}
		break;
	}
		default:
		   break;
	}
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
								esp_ble_gatts_cb_param_t *param) {
	/* If event is register event, store the gatts_if for each profile */
	if (event == ESP_GATTS_REG_EVT) {
		if (param->reg.status == ESP_GATT_OK) {
			spp_profile_tab[SPP_PROFILE_APP_IDX].gatts_if = gatts_if;
		} else {
			return;
		}
	}
	do {
		int idx;
		for (idx = 0; idx < SPP_PROFILE_NUM; idx++) {
			if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
					gatts_if == spp_profile_tab[idx].gatts_if) {
				if (spp_profile_tab[idx].gatts_cb) {
					spp_profile_tab[idx].gatts_cb(event, gatts_if, param);
				}
			}
		}
	} while (0);
}

static void read_task(void* pvParameters) {
	CMD_t cmdBuf;
	while(1) {
		xQueueReceive(xQueueRead, &cmdBuf, portMAX_DELAY);
		if(ble_read_isr_p != BLE_NO_INT){
            ble_read_isr_p(cmdBuf.payload, cmdBuf.length);
        }
	} 
}

void bluetooth_events_task(void * arg) {
	CMD_t cmdBuf;
	uint16_t spp_conn_id = 0xffff;
	esp_gatt_if_t spp_gatts_if = 0xff;
	int data_sent, i;

	while(1){
		vTaskDelay(50 / portTICK_PERIOD_MS);
		xQueueReceive(xQueueEvents, &cmdBuf, portMAX_DELAY);
        switch(cmdBuf.command){
            case CMD_BLUETOOTH_CONNECT:
                spp_conn_id = cmdBuf.spp_conn_id;
                spp_gatts_if = cmdBuf.spp_gatts_if;
            break;
            case CMD_BLUETOOTH_AUTH:
                ESP_LOGI(TAG, "Device connected");
				status = BLE_CONNECTED;
            break;
            case CMD_BLUETOOTH_DISCONNECT:
                ESP_LOGI(TAG, "Device disconnected");
				status = BLE_DISCONNECTED;
            break;
            case CMD_SEND_DATA:
                if (status == BLE_CONNECTED) {
					data_sent = 0;
					i = 0;
					while(data_sent < cmdBuf.length){
						if((cmdBuf.length - data_sent) > MTU_MAX_BYTES){
							esp_ble_gatts_send_indicate(spp_gatts_if, spp_conn_id, spp_handle_table[SPP_IDX_SPP_DATA_NOTIFY_VAL], MTU_MAX_BYTES, &cmdBuf.payload[i*MTU_MAX_BYTES], false);
							data_sent += MTU_MAX_BYTES;
							i++;
						}else{
							esp_ble_gatts_send_indicate(spp_gatts_if, spp_conn_id, spp_handle_table[SPP_IDX_SPP_DATA_NOTIFY_VAL], cmdBuf.length - data_sent, &cmdBuf.payload[i*MTU_MAX_BYTES], false);
							data_sent += MTU_MAX_BYTES;
						}
					}
                }
            break;
            case CMD_BLUETOOTH_DATA:
                xQueueSend(xQueueRead, &cmdBuf, portMAX_DELAY);
            break;
        }
	} 
}

/*==================[external functions definition]==========================*/
void BleInit(ble_config_t * ble_device){
esp_err_t ret;
    device_name = ble_device->device_name;
    ble_read_isr_p = ble_device->func_p;
	/* Initialize NVS. */
	ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ret = esp_bt_controller_init(&bt_cfg);
	if (ret) {
		ESP_LOGE(TAG, "%s init controller failed: %s", __func__, esp_err_to_name(ret));
		return;
	}
	ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (ret) {
		ESP_LOGE(TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
		return;
	}
	ret = esp_bluedroid_init();
	if (ret) {
		ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
		return;
	}
	ret = esp_bluedroid_enable();
	if (ret) {
		ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
		return;
	}
	ret = esp_ble_gatts_register_callback(gatts_event_handler);
	if (ret){
		ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
		return;
	}
	ret = esp_ble_gap_register_callback(gap_event_handler);
	if (ret){
		ESP_LOGE(TAG, "gap register error, error code = %x", ret);
		return;
	}
	ret = esp_ble_gatts_app_register(ESP_SPP_APP_ID);
	if (ret){
		ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
		return;
	}
	/* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
	esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;		//bonding with peer device after authentication
	esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE;			//set the IO capability to No output No input
	uint8_t key_size = 16;		//the key size should be 7~16 bytes
	uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
	//set static passkey
	uint32_t passkey = 123456;
	uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
	uint8_t oob_support = ESP_BLE_OOB_DISABLE;
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
	esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
	
    /* Create Queue */
	xQueueEvents = xQueueCreate(10, sizeof(CMD_t));
	configASSERT(xQueueEvents);
	xQueueRead = xQueueCreate( 10, sizeof(CMD_t) );
	configASSERT(xQueueRead);

	/* Start tasks */
	xTaskCreate(read_task, "read", 1024*4, NULL, 2, NULL);
	xTaskCreate(bluetooth_events_task, "bluetooth_events", 1024*4, NULL, 10, NULL);
}

ble_status_t BleStatus(void){
	return status;
}

void BleSendByte(const char *data){
	CMD_t cmdBuf;
	if(status == BLE_CONNECTED){
		cmdBuf.command = CMD_SEND_DATA;
		cmdBuf.length = 1;
		memcpy(cmdBuf.payload, data, cmdBuf.length);
		xQueueSend(xQueueEvents, &cmdBuf, portMAX_DELAY);
	}
}

void BleSendString(const char *msg){
	CMD_t cmdBuf;
	if(status == BLE_CONNECTED){
		cmdBuf.command = CMD_SEND_DATA;
		cmdBuf.length = 0;
		while(msg[cmdBuf.length] != '\0'){
			cmdBuf.length++;
		}
		memcpy(cmdBuf.payload, msg, cmdBuf.length);
		xQueueSend(xQueueEvents, &cmdBuf, portMAX_DELAY);
	}
}

void BleSendBuffer(const char *data, uint8_t nbytes){
	CMD_t cmdBuf;
	if(status == BLE_CONNECTED){
		cmdBuf.command = CMD_SEND_DATA;
		cmdBuf.length = nbytes;
		memcpy(cmdBuf.payload, data, cmdBuf.length);
		xQueueSend(xQueueEvents, &cmdBuf, portMAX_DELAY);
	}
}
/*==================[end of file]============================================*/