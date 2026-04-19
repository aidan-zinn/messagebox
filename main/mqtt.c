#include "mqtt.h"
#include "mqtt_client.h"
#include "esp_log.h"

static const char *TAG = "mqtt";
static mqtt_message_cb_t s_message_cb;

extern const uint8_t hivemq_root_ca_pem_start[] asm("_binary_hivemq_root_ca_pem_start");
extern const uint8_t hivemq_root_ca_pem_end[]   asm("_binary_hivemq_root_ca_pem_end");

static void mqtt_event_handler(void *arg, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch (event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to broker");
        esp_mqtt_client_subscribe(event->client, CONFIG_MQTT_TOPIC, 1);
        ESP_LOGI(TAG, "Subscribed to %s", CONFIG_MQTT_TOPIC);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from broker");
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Message received (%d bytes)", event->data_len);
        if (s_message_cb) {
            s_message_cb(event->data, event->data_len);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error");
        break;
    default:
        break;
    }
}

esp_err_t mqtt_start(mqtt_message_cb_t cb)
{
    s_message_cb = cb;

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = CONFIG_MQTT_BROKER_URI,
            .verification.certificate = (const char *)hivemq_root_ca_pem_start,
        },
        .credentials = {
            .username = CONFIG_MQTT_USERNAME,
            .authentication.password = CONFIG_MQTT_PASSWORD,
        },
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    if (!client) return ESP_FAIL;

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    return esp_mqtt_client_start(client);
}
