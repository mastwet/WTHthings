#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"

#define TAG "RP"
#define BLEMIDI_TAG "BLEMIDI"
#define BLEMIDI_DEVICE_NAME "RP Ble midi"
#define BLEMIDI_NUM_PORTS 1
#define BLEMIDI_OUTBUFFER_FLUSH_MS 15
#define BLEMIDI_ENABLE_CONSOLE 1

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

static int32_t blemidi_receive_packet(uint8_t blemidi_port, uint8_t *stream, size_t len, void *_callback_midi_message_received);

void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len, size_t continued_sysex_pos);
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void blemidi_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
static void blemidi_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

static int32_t blemidi_outbuffer_push(uint8_t blemidi_port, uint8_t *stream, size_t len);
int32_t blemidi_send_message(uint8_t blemidi_port, uint8_t *stream, size_t len);

void blemidi_tick(void);
int32_t blemidi_outbuffer_flush(uint8_t blemidi_port);

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define SVC_INST_ID                 0

// 定义特性值的最大长度。当GATT客户端执行写入或准备写入操作时，
// 数据长度必须小于GATTS_MIDI_CHAR_VAL_LEN_MAX。

#define GATTS_MIDI_CHAR_VAL_LEN_MAX 100
#define PREPARE_BUF_MAX_SIZE        2048
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

// 记录广播和扫描响应配置是否完成的静态变量
static uint8_t adv_config_done       = 0;

// MTU可以在运行时由客户端更改
static size_t blemidi_mtu = GATTS_MIDI_CHAR_VAL_LEN_MAX - 3;

// 应用程序通过blemidi_tick_ms()调用来不断增加的时间戳
static uint16_t blemidi_timestamp = 0;

// 缓存待发送的MIDI消息，最多保留10毫秒的数据，以避免对小消息需要排队多个BLE包
static uint8_t  blemidi_outbuffer[BLEMIDI_NUM_PORTS][GATTS_MIDI_CHAR_VAL_LEN_MAX];
static uint16_t blemidi_outbuffer_len[BLEMIDI_NUM_PORTS];
static uint16_t blemidi_outbuffer_timestamp_last_flush = 0;

// 处理连续SysEx消息的位置信息
static size_t   blemidi_continued_sysex_pos[BLEMIDI_NUM_PORTS];

/* 属性状态机定义 */
enum
{
    IDX_SVC,
    IDX_CHAR_A,
    IDX_CHAR_VAL_A,
    IDX_CHAR_CFG_A,

    HRS_IDX_NB,
};
uint16_t midi_handle_table[HRS_IDX_NB];
prepare_type_env_t prepare_write_env;

uint8_t midi_service_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0x00, 0xC7, 0xC4, 0x4E, 0xE3, 0x6C, 0x51, 0xA7, 0x33, 0x4B, 0xE8, 0xED, 0x5A, 0x0E, 0xB8, 0x03
};

const uint8_t midi_characteristics_uuid[16] = {
    /* LSB <--------------------------------------------------------------------------------> MSB */
    0xF3, 0x6B, 0x10, 0x9D, 0x66, 0xF2, 0xA9, 0xA1, 0x12, 0x41, 0x68, 0x38, 0xDB, 0xE5, 0x72, 0x77
};


/* 广播数据的长度必须小于31字节 */
static esp_ble_adv_data_t adv_data = {
	// 不设置为扫描响应数据，而是用于广播数据
    .set_scan_rsp        = false,
	 // 不包含设备名称以确保不超过31字节的限制
    .include_name        = false,
	 // 包含发射功率信息
    .include_txpower     = true,
	// 从机连接最小间隔，时间等于 min_interval * 1.25 毫秒
    .min_interval        = 0x0006,
	 // 从机连接最大间隔，时间等于 max_interval * 1.25 毫秒
    .max_interval        = 0x0010,
	// 外观属性（Appearance）值，这里设置为0x00
    .appearance          = 0x00,
	 // 制造商数据长度为0，不包含制造商特定数据
    .manufacturer_len    = 0,
	 // 制造商特定数据指针设为NULL，实际应用中可能指向具体的数据
    .p_manufacturer_data = NULL, //test_manufacturer,
	 // 服务数据长度为0，不包含服务特定数据
    .service_data_len    = 0,
	 // 服务特定数据指针设为NULL，实际应用中可能指向具体的数据
    .p_service_data      = NULL,
	// 服务UUID列表长度等于MIDI服务UUID的大小
    .service_uuid_len    = sizeof(midi_service_uuid),
	  // 指向MIDI服务的UUID值
    .p_service_uuid      = midi_service_uuid,
	// 广播标志位：通用发现模式和非支持蓝牙BR/EDR
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// 扫描响应数据结构体
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp        = true,
    .include_name        = true,
    .include_txpower     = true,
    .min_interval        = 0x0006,
    .max_interval        = 0x0010,
    .appearance          = 0x00,
    .manufacturer_len    = 0, //TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len    = 0,
    .p_service_data      = NULL,
    .service_uuid_len    = sizeof(midi_service_uuid),
    .p_service_uuid      = midi_service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// 广播参数结构体
static esp_ble_adv_params_t adv_params = {
    .adv_int_min         = 0x20,
    .adv_int_max         = 0x40,
    .adv_type            = ADV_TYPE_IND,
    .own_addr_type       = BLE_ADDR_TYPE_PUBLIC,
    .channel_map         = ADV_CHNL_ALL,
    .adv_filter_policy   = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

// GATT服务实例结构体
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

static void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


/* 对于每一个基于GATT的配置文件（profile），一个应用程序ID（app_id）和一个GATT服务接口（gatts_if），此数组将用于存储由ESP_GATTS_REG_EVT事件返回的gatts_if值。 */
static struct gatts_profile_inst midi_profile_tab[PROFILE_NUM] = {
		// 通过索引PROFILE_APP_IDX访问数组中的元素，为MIDI配置文件实例初始化结构体
    [PROFILE_APP_IDX] = {
        .gatts_cb = gatts_profile_event_handler,
		// 初始时，由于尚未获取到实际的gatt_if值，因此将其设置为ESP_GATT_IF_NONE
        .gatts_if = ESP_GATT_IF_NONE,
    },
};

/* Service */
//主服务
const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
//特性声明
const uint16_t character_declaration_uuid   = ESP_GATT_UUID_CHAR_DECLARE;

//客户端特征配置描述符
const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
const uint8_t char_prop_read                = ESP_GATT_CHAR_PROP_BIT_READ;
const uint8_t char_prop_write               = ESP_GATT_CHAR_PROP_BIT_WRITE;
const uint8_t char_prop_read_write_notify   = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
const uint8_t char_prop_read_write_writenr_notify = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_WRITE_NR;

const uint8_t char_value[3]                 = {0x80, 0x80, 0xfe};
const uint8_t blemidi_ccc[2]                = {0x00, 0x00};

void (*blemidi_callback_midi_message_received)(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len, size_t continued_sysex_pos);

// 定义BLE GATT服务的属性数据库结构体数组gatt_db，用于存储BLE MIDI服务的各种属性信息
static const esp_gatts_attr_db_t gatt_db[HRS_IDX_NB] =
{
	 // 1.服务声明：定义MIDI服务（Service Declaration）
    [IDX_SVC]        =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
      16, sizeof(midi_service_uuid), (uint8_t *)&midi_service_uuid}},

	  // 2. 特性声明：定义一个具有读写、写通知特性的MIDI特性（Characteristic Declaration）
    [IDX_CHAR_A]     =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write_writenr_notify}},

	  // 3. 特性值：定义MIDI特性对应的值（Characteristic Value）
    [IDX_CHAR_VAL_A] =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_128, (uint8_t *)&midi_characteristics_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      GATTS_MIDI_CHAR_VAL_LEN_MAX, sizeof(char_value), (uint8_t *)char_value}},

	  // 4. 客户端特征配置描述符（Client Characteristic Configuration Descriptor）
	  // 这是一个标准的BLE 2902描述符，用于控制对特性值的通知或指示功能
    [IDX_CHAR_CFG_A]  =
    {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
      sizeof(uint16_t), sizeof(blemidi_ccc), (uint8_t *)blemidi_ccc}},
};





static int32_t blemidi_init(void *_callback_midi_message_received);
#define MIDI_C4 60

int note_pos = 0;


void app_main()
{
  // 初始化BLE设备
  int status = blemidi_init(callback_midi_message_received);
  if( status < 0 ) {
    ESP_LOGE(TAG, "BLE MIDI Driver returned status=%d", status);
  } else {
    ESP_LOGI(TAG, "BLE MIDI Driver initialized successfully");
  }

  while(1){
	    ESP_LOGI(TAG, "Sending MIDI Note...");

	    note_pos ++;
	    if(note_pos == 24) note_pos = 0;
	    int pos = MIDI_C4 + note_pos;

	    {
	      uint8_t message[3] = { 0x90, pos, 0x7f };
	      blemidi_send_message(0, message, sizeof(message));
	    }


	    blemidi_tick(); // for timestamp and output buffer handling
	    {
	      uint8_t message[3] = { 0x90, pos, 0x00 };
	      blemidi_send_message(0, message, sizeof(message));
	    }


	    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Timestamp handling
////////////////////////////////////////////////////////////////////////////////////////////////////
void blemidi_tick(void)
{
  struct timeval tv;
  _gettimeofday_r(NULL,&tv, NULL);
  blemidi_timestamp = (tv.tv_sec * 1000 + (tv.tv_usec / 1000)); // 1 mS per increment

  if( (blemidi_outbuffer_timestamp_last_flush > blemidi_timestamp) ||
      (blemidi_timestamp > (blemidi_outbuffer_timestamp_last_flush + BLEMIDI_OUTBUFFER_FLUSH_MS)) ) {
    uint8_t blemidi_port;
    for(blemidi_port=0; blemidi_port < BLEMIDI_NUM_PORTS; ++blemidi_port) {
      blemidi_outbuffer_flush(blemidi_port);
    }

    blemidi_outbuffer_timestamp_last_flush = blemidi_timestamp;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Flush Output Buffer (normally done by blemidi_tick_ms each 15 mS)
////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t blemidi_outbuffer_flush(uint8_t blemidi_port)
{
  if( blemidi_port >= BLEMIDI_NUM_PORTS )
    return -1; // invalid port

  if( blemidi_outbuffer_len[blemidi_port] > 0 ) {
    esp_ble_gatts_send_indicate(midi_profile_tab[PROFILE_APP_IDX].gatts_if, midi_profile_tab[PROFILE_APP_IDX].conn_id, midi_handle_table[IDX_CHAR_VAL_A], blemidi_outbuffer_len[blemidi_port], blemidi_outbuffer[blemidi_port], false);
    blemidi_outbuffer_len[blemidi_port] = 0;
  }
  return 0; // no error
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// 初始化BLE-MIDI 服务
////////////////////////////////////////////////////////////////////////////////////////////////////
int32_t blemidi_init(void *_callback_midi_message_received)
{
  esp_err_t ret;

  // 成功启动后，回调函数会被安装
  blemidi_callback_midi_message_received = NULL;

  /* 初始化 NVS. */
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK( ret );

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  /* 初始化bluedroid. */
  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(BLEMIDI_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
    return -1;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
    ESP_LOGE(BLEMIDI_TAG, "%s enable controller failed: %s", __func__, esp_err_to_name(ret));
    return -2;
  }

  ret = esp_bluedroid_init();
  if (ret) {
    ESP_LOGE(BLEMIDI_TAG, "%s init bluetooth failed: %s", __func__, esp_err_to_name(ret));
    return -3;
  }

  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(BLEMIDI_TAG, "%s enable bluetooth failed: %s", __func__, esp_err_to_name(ret));
    return -4;
  }

  //注册GATTS服务。提供服务和特性供客户端查询或修改
  ret = esp_ble_gatts_register_callback(gatts_event_handler);
  if (ret){
    ESP_LOGE(BLEMIDI_TAG, "gatts register error, error code = %x", ret);
    return -5;
  }

  //注册gap服务。发现其他设备、建立和管理连接
  ret = esp_ble_gap_register_callback(gap_event_handler);
  if (ret){
    ESP_LOGE(BLEMIDI_TAG, "gap register error, error code = %x", ret);
    return -6;
  }

  ret = esp_ble_gatts_app_register(ESP_APP_ID);
  if (ret){
    ESP_LOGE(BLEMIDI_TAG, "gatts app register error, error code = %x", ret);
    return -7;
  }
  //MTU：设定数据链路层帧内能够传输的最大数据量
  esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(GATTS_MIDI_CHAR_VAL_LEN_MAX);
  if (local_mtu_ret){
    ESP_LOGE(BLEMIDI_TAG, "set local  MTU failed, error code = %x", local_mtu_ret);
    return -8;
  }

  //输出缓存
  {
    uint32_t blemidi_port;
    for(blemidi_port=0; blemidi_port<BLEMIDI_NUM_PORTS; ++blemidi_port) {
      blemidi_outbuffer_len[blemidi_port] = 0;
      blemidi_continued_sysex_pos[blemidi_port] = 0;
    }
  }

  //最终回调函数
  blemidi_callback_midi_message_received = _callback_midi_message_received;

  esp_log_level_set(BLEMIDI_TAG, ESP_LOG_WARN); // can be changed with the "blemidi_debug on" console command

  return 0; // no error
}

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~ADV_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
            adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
            if (adv_config_done == 0){
                esp_ble_gap_start_advertising(&adv_params);
            }
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            /* advertising start complete event to indicate advertising start successfully or failed */
            if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(BLEMIDI_TAG, "advertising start failed");
            }else{
                ESP_LOGI(BLEMIDI_TAG, "advertising start successfully");
            }
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
                ESP_LOGE(BLEMIDI_TAG, "Advertising stop failed");
            }
            else {
                ESP_LOGI(BLEMIDI_TAG, "Stop adv successfully\n");
            }
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            ESP_LOGI(BLEMIDI_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                  param->update_conn_params.status,
                  param->update_conn_params.min_int,
                  param->update_conn_params.max_int,
                  param->update_conn_params.conn_int,
                  param->update_conn_params.latency,
                  param->update_conn_params.timeout);
            break;
        default:
            break;
    }
}


static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{

    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT) {
        if (param->reg.status == ESP_GATT_OK) {
            midi_profile_tab[PROFILE_APP_IDX].gatts_if = gatts_if;
        } else {
            ESP_LOGE(BLEMIDI_TAG, "reg app failed, app_id %04x, status %d",
                    param->reg.app_id,
                    param->reg.status);
            return;
        }
    }
    do {
        int idx;
        for (idx = 0; idx < PROFILE_NUM; idx++) {
            /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == midi_profile_tab[idx].gatts_if) {
                if (midi_profile_tab[idx].gatts_cb) {
                    midi_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void callback_midi_message_received(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len, size_t continued_sysex_pos)
{
  ESP_LOGI(TAG, "CALLBACK blemidi_port=%d, timestamp=%d, midi_status=0x%02x, len=%d, continued_sysex_pos=%d, remaining_message:", blemidi_port, timestamp, midi_status, len, continued_sysex_pos);
  esp_log_buffer_hex(TAG, remaining_message, len);

  // loopback received message
  {
    // TODO: more comfortable packet creation via special APIs

    // Note: by intention we create new packets for each incoming message
    // this shows that running status is maintained, and that SysEx streams work as well

    if( midi_status == 0xf0 && continued_sysex_pos > 0 ) {
      //blemidi_send_message(0, remaining_message, len); // just forward
    } else {
      size_t loopback_message_len = 1 + len; // includes MIDI status and remaining bytes
      uint8_t *loopback_message = (uint8_t *)malloc(loopback_message_len * sizeof(uint8_t));
      if( loopback_message == NULL ) {
        // no memory...
      } else {
        loopback_message[0] = midi_status;
        memcpy(&loopback_message[1], remaining_message, len);

        //blemidi_send_message(0, loopback_message, loopback_message_len);

        free(loopback_message);
      }
    }
  }
}



int32_t blemidi_receive_packet(uint8_t blemidi_port, uint8_t *stream, size_t len, void *_callback_midi_message_received)
    {
      void (*callback_midi_message_received)(uint8_t blemidi_port, uint16_t timestamp, uint8_t midi_status, uint8_t *remaining_message, size_t len, size_t continued_sysex_pos) = _callback_midi_message_received;

      if( blemidi_port >= BLEMIDI_NUM_PORTS )
        return -1; // invalid port

      ESP_LOGI(BLEMIDI_TAG, "receive_packet blemidi_port=%d, len=%d, stream:", blemidi_port, len);
      esp_log_buffer_hex(BLEMIDI_TAG, stream, len);

      // detect continued SysEx
      uint8_t continued_sysex = 0;
      if( len > 2 && (stream[0] & 0x80) && !(stream[1] & 0x80)) {
        continued_sysex = 1;
      } else {
        blemidi_continued_sysex_pos[blemidi_port] = 0;
      }


      if( len < 3 ) {
        ESP_LOGE(BLEMIDI_TAG, "stream length should be >=3");
        return -1;
      } else if( !(stream[0] & 0x80) ) {
        ESP_LOGE(BLEMIDI_TAG, "missing timestampHigh");
        return -2;
      } else {
        size_t pos = 0;

        // getting timestamp
        uint16_t timestamp = (stream[pos++] & 0x3f) << 7;

        // parsing stream
        {
          //! Number if expected bytes for a common MIDI event - 1
          const uint8_t midi_expected_bytes_common[8] = {
            2, // Note On
            2, // Note Off
            2, // Poly Preasure
            2, // Controller
            1, // Program Change
            1, // Channel Preasure
            2, // Pitch Bender
            0, // System Message - must be zero, so that mios32_midi_expected_bytes_system[] will be used
          };

          //! Number if expected bytes for a system MIDI event - 1
          const uint8_t midi_expected_bytes_system[16] = {
            1, // SysEx Begin (endless until SysEx End F7)
            1, // MTC Data frame
            2, // Song Position
            1, // Song Select
            0, // Reserved
            0, // Reserved
            0, // Request Tuning Calibration
            0, // SysEx End

            // Note: just only for documentation, Realtime Messages don't change the running status
            0, // MIDI Clock
            0, // MIDI Tick
            0, // MIDI Start
            0, // MIDI Continue
            0, // MIDI Stop
            0, // Reserved
            0, // Active Sense
            0, // Reset
          };

          uint8_t midi_status = continued_sysex ? 0xf0 : 0x00;

          while( pos < len ) {
            if( !(stream[pos] & 0x80) ) {
              if( !continued_sysex ) {
                ESP_LOGE(BLEMIDI_TAG, "missing timestampLow in parsed message");
                return -3;
              }
            } else {
              timestamp &= ~0x7f;
              timestamp |= stream[pos++] & 0x7f;
              continued_sysex = 0;
              blemidi_continued_sysex_pos[blemidi_port] = 0;
            }

            if( stream[pos] & 0x80 ) {
              midi_status = stream[pos++];
            }

            if( midi_status == 0xf0 ) {
              size_t num_bytes;
              for(num_bytes=0; stream[pos+num_bytes] < 0x80; ++num_bytes) {
                if( (pos+num_bytes) >= len ) {
                  break;
                }
              }
              if( _callback_midi_message_received ) {
                callback_midi_message_received(blemidi_port, timestamp, midi_status, &stream[pos], num_bytes, blemidi_continued_sysex_pos[blemidi_port]);
              }
              pos += num_bytes;
              blemidi_continued_sysex_pos[blemidi_port] += num_bytes; // we expect another packet with the remaining SysEx stream
            } else {
              uint8_t num_bytes = midi_expected_bytes_common[(midi_status >> 4) & 0x7];
              if( num_bytes == 0 ) { // System Message
                num_bytes = midi_expected_bytes_system[midi_status & 0xf];
              }

              if( (pos+num_bytes) > len ) {
                ESP_LOGE(BLEMIDI_TAG, "missing %d bytes in parsed message", num_bytes);
                return -5;
              } else {
                if( _callback_midi_message_received ) {
                  callback_midi_message_received(blemidi_port, timestamp, midi_status, &stream[pos], num_bytes, 0);
                }
                pos += num_bytes;
              }
            }
          }
        }
      }

      return 0; // no error
    }

static void blemidi_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    ESP_LOGI(BLEMIDI_TAG, "prepare write, handle = %d, value len = %d", param->write.handle, param->write.len);
    esp_gatt_status_t status = ESP_GATT_OK;
    if (prepare_write_env->prepare_buf == NULL) {
        prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
        prepare_write_env->prepare_len = 0;
        if (prepare_write_env->prepare_buf == NULL) {
            ESP_LOGE(BLEMIDI_TAG, "%s, Gatt_server prep no mem", __func__);
            status = ESP_GATT_NO_RESOURCES;
        }
    } else {
        if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_OFFSET;
        } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
            status = ESP_GATT_INVALID_ATTR_LEN;
        }
    }
    /*send response when param->write.need_rsp is true */
    if (param->write.need_rsp){
        esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
        if (gatt_rsp != NULL){
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(BLEMIDI_TAG, "Send response error");
            }
            free(gatt_rsp);
        }else{
            ESP_LOGE(BLEMIDI_TAG, "%s, malloc failed", __func__);
        }
    }
    if (status != ESP_GATT_OK){
        return;
    }
    memcpy(prepare_write_env->prepare_buf + param->write.offset,
           param->write.value,
           param->write.len);
    prepare_write_env->prepare_len += param->write.len;

}

static void blemidi_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC && prepare_write_env->prepare_buf){
        esp_log_buffer_hex(BLEMIDI_TAG, prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }else{
        ESP_LOGI(BLEMIDI_TAG,"ESP_GATT_PREP_WRITE_CANCEL");
    }
    if (prepare_write_env->prepare_buf) {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }
    prepare_write_env->prepare_len = 0;
}

static void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_REG_EVT:{
            esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(BLEMIDI_DEVICE_NAME);
            if (set_dev_name_ret){
                ESP_LOGE(BLEMIDI_TAG, "set device name failed, error code = %x", set_dev_name_ret);
            }

            //config adv data
            esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
            if (ret){
                ESP_LOGE(BLEMIDI_TAG, "config adv data failed, error code = %x", ret);
            }
            adv_config_done |= ADV_CONFIG_FLAG;
            //config scan response data
            ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
            if (ret){
                ESP_LOGE(BLEMIDI_TAG, "config scan response data failed, error code = %x", ret);
            }
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;

            esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
            if (create_attr_ret){
                ESP_LOGE(BLEMIDI_TAG, "create attr table failed, error code = %x", create_attr_ret);
            }
        }
            break;
        case ESP_GATTS_READ_EVT:
            ESP_LOGI(BLEMIDI_TAG, "ESP_GATTS_READ_EVT");
            break;
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep){
                if (midi_handle_table[IDX_CHAR_VAL_A] == param->write.handle ) {
                  // the data length of gattc write  must be less than blemidi_mtu.
#if 0
                  ESP_LOGI(BLEMIDI_TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", param->write.handle, param->write.len);
                  esp_log_buffer_hex(BLEMIDI_TAG, param->write.value, param->write.len);
#endif
                  blemidi_receive_packet(0, param->write.value, param->write.len, blemidi_callback_midi_message_received);
                }
            } else {
                /* handle prepare write */
                blemidi_prepare_write_event_env(gatts_if, &prepare_write_env, param);
            }
            break;
        case ESP_GATTS_EXEC_WRITE_EVT:
            // the length of gattc prepare write data must be less than blemidi_mtu.
            ESP_LOGI(BLEMIDI_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            blemidi_exec_write_event_env(&prepare_write_env, param);
            break;
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(BLEMIDI_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);

            // change MTU for BLE MIDI transactions
            if( param->mtu.mtu <= 3 ) {
              blemidi_mtu = 3; // very unlikely...
            } else {
              // we decrease -10 to prevent following driver warning:
              //  (30774) BT_GATT: attribute value too long, to be truncated to 97
              blemidi_mtu = param->mtu.mtu - 3;
              // failsave
              if( blemidi_mtu > (GATTS_MIDI_CHAR_VAL_LEN_MAX-3) )
                blemidi_mtu = (GATTS_MIDI_CHAR_VAL_LEN_MAX-3);
            }
            break;
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(BLEMIDI_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", param->conf.status, param->conf.handle);
            break;
        case ESP_GATTS_START_EVT:
            ESP_LOGI(BLEMIDI_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
            break;
        case ESP_GATTS_CONNECT_EVT:
            ESP_LOGI(BLEMIDI_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
            esp_log_buffer_hex(BLEMIDI_TAG, param->connect.remote_bda, 6);
            esp_ble_conn_update_params_t conn_params = {0};
            memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
            /* For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. */
            conn_params.latency = 0;
            conn_params.max_int = 0x10;    // max_int = 0x10*1.25ms = 20ms
            conn_params.min_int = 0x0b;    // min_int = 0x0b*1.25ms = 15ms
            conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
            //start sent the update connection parameters to the peer device.
            esp_ble_gap_update_conn_params(&conn_params);
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            ESP_LOGI(BLEMIDI_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
            esp_ble_gap_start_advertising(&adv_params);
            break;
        case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
            if (param->add_attr_tab.status != ESP_GATT_OK){
                ESP_LOGE(BLEMIDI_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
            }
            else if (param->add_attr_tab.num_handle != HRS_IDX_NB){
                ESP_LOGE(BLEMIDI_TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, HRS_IDX_NB);
            }
            else {
                ESP_LOGI(BLEMIDI_TAG, "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
                memcpy(midi_handle_table, param->add_attr_tab.handles, sizeof(midi_handle_table));
                esp_ble_gatts_start_service(midi_handle_table[IDX_SVC]);
            }
            break;
        }
        case ESP_GATTS_STOP_EVT:
        case ESP_GATTS_OPEN_EVT:
        case ESP_GATTS_CANCEL_OPEN_EVT:
        case ESP_GATTS_CLOSE_EVT:
        case ESP_GATTS_LISTEN_EVT:
        case ESP_GATTS_CONGEST_EVT:
        case ESP_GATTS_UNREG_EVT:
        case ESP_GATTS_DELETE_EVT:
        default:
            break;
    }
}

uint8_t blemidi_timestamp_high(void)
{
  return (0x80 | ((blemidi_timestamp >> 7) & 0x3f));
}

uint8_t blemidi_timestamp_low(void)
{
  return (0x80 | (blemidi_timestamp & 0x7f));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Push a new MIDI message to the output buffer
////////////////////////////////////////////////////////////////////////////////////////////////////
static int32_t blemidi_outbuffer_push(uint8_t blemidi_port, uint8_t *stream, size_t len)
{
  const size_t max_header_size = 2;

  if( blemidi_port >= BLEMIDI_NUM_PORTS )
    return -1; // invalid port

  // if len >= MTU, it makes sense to send out immediately
  if( len >= (blemidi_mtu-max_header_size) ) {
    // this is very unlikely, since applemidi_send_message() maintains the size
    // but just in case of future extensions, we prepare dynamic memory allocation for "big packets"
    blemidi_outbuffer_flush(blemidi_port);
    {
      size_t packet_len = max_header_size + len;
      uint8_t *packet = malloc(packet_len);
      if( packet == NULL ) {
        return -1; // couldn't create temporary packet
      } else {
        // new packet: with timestampHigh and timestampLow, or in case of continued SysEx packet: only timestampHigh
        packet[0] = blemidi_timestamp_high();
        if( stream[0] >= 0x80 ) {
          packet[1] = blemidi_timestamp_low();
          memcpy((uint8_t *)packet + 2, stream, len);
        } else {
          packet_len -= 1;
          memcpy((uint8_t *)packet + 1, stream, len);
        }
        esp_ble_gatts_send_indicate(midi_profile_tab[PROFILE_APP_IDX].gatts_if, midi_profile_tab[PROFILE_APP_IDX].conn_id, midi_handle_table[IDX_CHAR_VAL_A], packet_len, packet, false);
        free(packet);
      }
    }
  } else {
    // flush buffer before adding new message
    if( (blemidi_outbuffer_len[blemidi_port] + len) >= blemidi_mtu )
      blemidi_outbuffer_flush(blemidi_port);

    // adding new message
    if( blemidi_outbuffer_len[blemidi_port] == 0 ) {
      // new packet: with timestampHigh and timestampLow, or in case of continued SysEx packet: only timestampHigh
      blemidi_outbuffer[blemidi_port][blemidi_outbuffer_len[blemidi_port]++] = blemidi_timestamp_high();
      if( stream[0] >= 0x80 ) {
        blemidi_outbuffer[blemidi_port][blemidi_outbuffer_len[blemidi_port]++] = blemidi_timestamp_low();
      }
    } else {
      blemidi_outbuffer[blemidi_port][blemidi_outbuffer_len[blemidi_port]++] = blemidi_timestamp_low();
    }

    memcpy(&blemidi_outbuffer[blemidi_port][blemidi_outbuffer_len[blemidi_port]], stream, len);
    blemidi_outbuffer_len[blemidi_port] += len;
  }

  return 0; // no error
}


int32_t blemidi_send_message(uint8_t blemidi_port, uint8_t *stream, size_t len)
{
  const size_t max_header_size = 2;

  if( blemidi_port >= BLEMIDI_NUM_PORTS )
    return -1; // invalid port

  // we've to consider blemidi_mtu
  // if more bytes need to be sent, split over multiple packets
  // this will cost some extra stack space :-/ therefore handled separatly?

  if( len < (blemidi_mtu-max_header_size) ) {
    // just add to output buffer
    blemidi_outbuffer_push(blemidi_port, stream, len);
  } else {
    // sending packets
    size_t max_size = blemidi_mtu - max_header_size; // -3 since blemidi_outbuffer_push() will add the timestamps
    int pos;
    for(pos=0; pos<len; pos += max_size) {
      size_t packet_len = len-pos;
      if( packet_len >= max_size ) {
        packet_len = max_size;
      }
      blemidi_outbuffer_push(blemidi_port, &stream[pos], packet_len);
    }
  }


  return 0; // no error
}
