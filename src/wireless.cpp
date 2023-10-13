//
// Created by umbreon on 10/1/23.
//

#include "wireless.h"

#include "comms.h"
#include "screens/scanner.h"
#include <optional>

namespace bluetooth {

btstack_packet_callback_registration_t hci_event_callback_registration;

const uint8_t adv_data[] = {
    // some flags about discoverability or something
    0x02,
    BLUETOOTH_DATA_TYPE_FLAGS,
    0x06,
    // thingamajig name
    0x06,
    BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME,
    'Z',
    'C',
    'a',
    'l',
    'c',
    // UUIDs (oh god)
    1,
    BLUETOOTH_DATA_TYPE_INCOMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS,
};

uint16_t read_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size) {
    printf("BLE read stub\n");
    return 0;
}

int write_callback(hci_con_handle_t con_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer,
                   uint16_t buffer_size) {
    //
    switch (att_handle) {
    case ATT_CHARACTERISTIC_2a3d590c_85b8_4c4f_a690_a94ae44883bd_01_VALUE_HANDLE:
        screens::scanner::set_message(reinterpret_cast<char *>(buffer), buffer_size);
        break;
    case ATT_CHARACTERISTIC_31ac4628_4a47_4bb3_b775_73c4970305f4_01_VALUE_HANDLE:
        screens::scanner::set_image(buffer, buffer_size);
        break;
    }
    printf("BLE write %04X %d \n", att_handle, buffer_size);
    return 0;
}

std::optional<hci_con_handle_t> con_handle;

void notify_if_conn() {
    if (con_handle) {
        printf("BLE notify\n");
        att_server_request_can_send_now_event(*con_handle);
    }
}

void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size) {
    uint8_t event_type = hci_event_packet_get_type(packet);
    switch (event_type) {
    case HCI_EVENT_LE_META:
        if (HCI_SUBEVENT_LE_CONNECTION_COMPLETE == hci_event_le_meta_get_subevent_code(packet)) {
            con_handle = hci_subevent_le_connection_complete_get_connection_handle(packet);
            printf("BLE connection %04X\n", *con_handle);
            screens::scanner::reset_image();
        }
        break;
    case HCI_EVENT_DISCONNECTION_COMPLETE:
        con_handle = {};
        printf("BLE disconnect\n");
        screens::scanner::reset_image();
        break;
    case ATT_EVENT_CAN_SEND_NOW:
        if (con_handle) {
            att_server_indicate(*con_handle, ATT_CHARACTERISTIC_1468ccba_044d_4e26_9d68_b19edae251d4_01_VALUE_HANDLE, &event_type,
                                sizeof(event_type));
        }
    }
}

void init() {
    l2cap_init();
    sm_init();
    att_server_init(profile_data, read_callback, write_callback);

    hci_event_callback_registration.callback = packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    att_server_register_packet_handler(packet_handler);
    hci_power_control(HCI_POWER_ON);

    bd_addr_t null_addr;
    memset(null_addr, 0, sizeof(null_addr));
    gap_advertisements_set_params(800, 800, 0, 0, null_addr, 0x07, 0);
    gap_advertisements_set_data(sizeof(adv_data), const_cast<uint8_t *>(adv_data));
    gap_advertisements_enable(1);
}

} // namespace bluetooth