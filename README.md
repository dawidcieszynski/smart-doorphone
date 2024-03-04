# SmartDoorPhone

![Image](./docs/img/_e67b7304-4aa4-4d6b-8bc5-cdb299155211.jpg)

## Purpose

Purpose of this project is to create a device integrated with Home Assistant that will send notifications when someone is calling the intercom.

## Features

- Notification about the flat number someone is calling
- Configurable flat numbers to receive notifications

## Technologies used:

- ESP32 / ESP8266 
- ESPHome (uses Platform.io)
- Home Assistant

## Supported / tested devices

| Device / board | Reference          | SDP | SDP-SIMULATOR |
| -------------- | ------------------ | --- | ------------- |
| ESP32-CAM      | esp32dev           | ?   | ?             |
| ESP_01         | esp01_1m           | ?   | ?             |
| ESP32-C3       | esp32-c3-devkitm-1 | ?   | ?             |

## Docs

-  [Installation](./docs/Installation.md)

## Maybe

- Receive calls using something like SIM800L

## Related links

- [ESPHome Pulse Meter Component](https://esphome.io/components/sensor/pulse_meter.html)
  - [source code](https://github.com/esphome/esphome/tree/dev/esphome/components/pulse_meter)
- [Scheme-it](https://www.digikey.pl/en/schemeit/project) - tool for schematics creation
- Boards
  - ESP32-CAM
    - https://lastminuteengineers.com/esp32-cam-pinout-reference/
  - ESP32-C3 MINI
    - https://sklep.msalamon.pl/wp-content/uploads/2024/01/ESP32_C3_MINI-9.png
    - https://sklep.msalamon.pl/produkt/plytka-esp32-c3-super-mini-wifi-bluetooth/
    - https://pl.aliexpress.com/item/1005005834887898.html
  - ESP_01