/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"
#include "app_entry.h"

#include "exports/iot_export_shadow.h"

#define PRODUCT_KEY      "a1A83PCMVng"
#define PRODUCT_SECRET   "DXxguDoumhnH5tYO"
#define DEVICE_NAME      "ali_test"
#define DEVICE_SECRET    "nAHRe1LAY1rLE51fLbccbYNHQBfKtpJF"


// #define PRODUCT_KEY             "a1DW8sdFeno"
// #define PRODUCT_SECRET          "CKzl2t2jQ3HccnwO"
// #define DEVICE_NAME             "shadowtest"
// #define DEVICE_SECRET           "8qq4WssQESXe58xYEPP0znghDdciIJ95"


/* These are pre-defined topics */
#define TOPIC_UPDATE            "/"PRODUCT_KEY"/"DEVICE_NAME"/update"
#define TOPIC_ERROR             "/"PRODUCT_KEY"/"DEVICE_NAME"/update/error"
#define TOPIC_GET               "/"PRODUCT_KEY"/"DEVICE_NAME"/get"
#define TOPIC_DATA               "/"PRODUCT_KEY"/"DEVICE_NAME"/data"

#define MQTT_MSGLEN             (1024)

#define EXAMPLE_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

static int      user_argc;
static char   **user_argv;

void event_handle(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    uintptr_t packet_id = (uintptr_t)msg->msg;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    switch (msg->event_type) {
        case IOTX_MQTT_EVENT_UNDEF:
            EXAMPLE_TRACE("undefined event occur.");
            break;

        case IOTX_MQTT_EVENT_DISCONNECT:
            EXAMPLE_TRACE("MQTT disconnect.");
            break;

        case IOTX_MQTT_EVENT_RECONNECT:
            EXAMPLE_TRACE("MQTT reconnect.");
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("subscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("subscribe wait ack timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_SUBCRIBE_NACK:
            EXAMPLE_TRACE("subscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_SUCCESS:
            EXAMPLE_TRACE("unsubscribe success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_TIMEOUT:
            EXAMPLE_TRACE("unsubscribe timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_UNSUBCRIBE_NACK:
            EXAMPLE_TRACE("unsubscribe nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_SUCCESS:
            EXAMPLE_TRACE("publish success, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_TIMEOUT:
            EXAMPLE_TRACE("publish timeout, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_NACK:
            EXAMPLE_TRACE("publish nack, packet-id=%u", (unsigned int)packet_id);
            break;

        case IOTX_MQTT_EVENT_PUBLISH_RECEIVED:
            EXAMPLE_TRACE("topic message arrived but without any related handle: topic=%.*s, topic_msg=%.*s",
                          topic_info->topic_len,
                          topic_info->ptopic,
                          topic_info->payload_len,
                          topic_info->payload);
            break;

        case IOTX_MQTT_EVENT_BUFFER_OVERFLOW:
            EXAMPLE_TRACE("buffer overflow, %s", msg->msg);
            break;

        default:
            EXAMPLE_TRACE("Should NOT arrive here.");
            break;
    }
}

static void _device_shadow_cb_light1(iotx_shadow_attr_pt pattr)
{

    /*
     * ****** Your Code ******
     */

    EXAMPLE_TRACE("----");
    EXAMPLE_TRACE("Attrbute Name: '%s'", pattr->pattr_name);
    EXAMPLE_TRACE("Attrbute Value: %d", *(int32_t *)pattr->pattr_data);
    EXAMPLE_TRACE("----");
}

static void _device_shadow_cb_light2(iotx_shadow_attr_pt pattr)
{

    /*
     * ****** Your Code ******
     */

    EXAMPLE_TRACE("----");
    EXAMPLE_TRACE("Attrbute Name: '%s'", pattr->pattr_name);
    EXAMPLE_TRACE("Attrbute Value: %d", *(int32_t *)pattr->pattr_data);
    EXAMPLE_TRACE("----");
}


int mqtt_client(void)
{
    void *h_shadow;
    iotx_conn_info_pt puser_info;
//    iotx_mqtt_param_t mqtt_params;
    iotx_shadow_para_t shadow_para;
    char buf[1024];
    int32_t LightSwitch1 = 0, LightSwitch2 = 0, temperature = 0;
    iotx_shadow_attr_t attr_lightswitch1, attr_lightswitch2, attr_temperature;

    if (0 != IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&puser_info)) {
        EXAMPLE_TRACE("AUTH request failed!");
        return -1;
    }

    /* Construct a device shadow */
    memset(&shadow_para, 0, sizeof(iotx_shadow_para_t));

    shadow_para.mqtt.port = puser_info->port;
    shadow_para.mqtt.host = puser_info->host_name;
    shadow_para.mqtt.client_id = puser_info->client_id;
    shadow_para.mqtt.username = puser_info->username;
    shadow_para.mqtt.password = puser_info->password;
    shadow_para.mqtt.pub_key = puser_info->pub_key;

    shadow_para.mqtt.request_timeout_ms = 2000;
    shadow_para.mqtt.clean_session = 0;
    shadow_para.mqtt.keepalive_interval_ms = 60000;
    shadow_para.mqtt.read_buf_size = MQTT_MSGLEN;
    shadow_para.mqtt.write_buf_size = MQTT_MSGLEN;

    shadow_para.mqtt.handle_event.h_fp = NULL;
    shadow_para.mqtt.handle_event.pcontext = NULL;

    h_shadow = IOT_Shadow_Construct(&shadow_para);

 	memset(&attr_lightswitch1, 0, sizeof(iotx_shadow_attr_t));
    memset(&attr_lightswitch2, 0, sizeof(iotx_shadow_attr_t));
    memset(&attr_temperature, 0, sizeof(iotx_shadow_attr_t));

    /* Initialize the @lightswitch1 attribute */
    attr_lightswitch1.attr_type = IOTX_SHADOW_INT32;
    attr_lightswitch1.mode = IOTX_SHADOW_RW;
    attr_lightswitch1.pattr_name = "switch1";
    attr_lightswitch1.pattr_data = &LightSwitch1;
    attr_lightswitch1.attr_type = IOTX_SHADOW_INT32;
    attr_lightswitch1.callback = _device_shadow_cb_light1;

    attr_lightswitch2.attr_type = IOTX_SHADOW_INT32;
    attr_lightswitch2.mode = IOTX_SHADOW_RW;
    attr_lightswitch2.pattr_name = "switch2";
    attr_lightswitch2.pattr_data = &LightSwitch2;
    attr_lightswitch1.attr_type = IOTX_SHADOW_INT32;
    attr_lightswitch2.callback = _device_shadow_cb_light2;

    /* Initialize the @temperature attribute */
    attr_temperature.attr_type = IOTX_SHADOW_INT32;
    attr_temperature.mode = IOTX_SHADOW_READONLY;
    attr_temperature.pattr_name = "temperature";
    attr_temperature.pattr_data = &temperature;
    attr_lightswitch1.attr_type = IOTX_SHADOW_INT32;
    attr_temperature.callback = NULL;

    /* Register the attribute */
    /* Note that you must register the attribute you want to synchronize with cloud
     * before calling IOT_Shadow_Pull() */
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_lightswitch1);
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_lightswitch2);
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_temperature);

    /* synchronize the device shadow with device shadow cloud */
    IOT_Shadow_Pull(h_shadow);

    format_data_t format;

    /* Format the attribute data */
    IOT_Shadow_PushFormat_Init(h_shadow, &format, buf, 1024);
    IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_temperature);
    IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_lightswitch1);
    IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_lightswitch2);
    IOT_Shadow_PushFormat_Finalize(h_shadow, &format);

    /* Update attribute data */
    IOT_Shadow_Push(h_shadow, format.buf, format.offset, 10);

 //   HAL_SleepMs(1000);

    do {

    //    IOT_MQTT_Yield(h_shadow, 200);
        IOT_Shadow_Yield(h_shadow, 200);
   		HAL_SleepMs(1000);

    } while (1);

    /* Delete the two attributes */
    IOT_Shadow_DeleteAttribute(h_shadow, &attr_temperature);
    IOT_Shadow_DeleteAttribute(h_shadow, &attr_lightswitch1);
    IOT_Shadow_DeleteAttribute(h_shadow, &attr_lightswitch2);

    IOT_Shadow_Destroy(h_shadow);

    return 0;
}

int linkkit_main(void *paras)
{
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    user_argc = 0;
    user_argv = NULL;

    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        user_argc = p->argc;
        user_argv = p->argv;
    }

    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    HAL_SetProductSecret(PRODUCT_SECRET);
    
    /* Choose Login Server */
    int domain_type = IOTX_CLOUD_REGION_SHANGHAI;
    IOT_Ioctl(IOTX_IOCTL_SET_DOMAIN, (void *)&domain_type);

    /* Choose Login  Method */
    int dynamic_register = 0;
    IOT_Ioctl(IOTX_IOCTL_SET_DYNAMIC_REGISTER, (void *)&dynamic_register);

    mqtt_client();
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_SetLogLevel(IOT_LOG_NONE);

    EXAMPLE_TRACE("out of sample!");

    return 0;
}
