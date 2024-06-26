# SmartDoorPhone

<div align="center">

  <img src="docs/img/_e67b7304-4aa4-4d6b-8bc5-cdb299155211.jpg" alt="drawing" width="400" class="logo"/>

</div>

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

| Device / board | Reference          | SDP      | SDP-SIMULATOR |
| -------------- | ------------------ | -------- | ------------- |
| ESP32-CAM      | esp32dev           | yes      | ?             |
| ESP_01         | esp01_1m           | ?        | yes           |
| ESP32-C3       | esp32-c3-devkitm-1 | unstable | ?             |

## Docs

-  [Development](./docs/Development.md)

## Maybe

- Receive calls using something like SIM800L

## Technical details

### Protocol
For this line low is the active state.

#### Flat call
Flat call starts with init pulse of about 200ms

![line init pulse](docs/img/line_init.png)

Then subsequent pulses gives the flat number that is requested

![line pulses](docs/img/line_pulse.png)

after about 50 ms there will be an analog ring tone signal transfered through the line

![timeout after flat number and ring tone](docs/img/line_timeout_ring.png)

## Related links

- [ESPHome Pulse Meter Component](https://esphome.io/components/sensor/pulse_meter.html)
  - [source code](https://github.com/esphome/esphome/tree/dev/esphome/components/pulse_meter)
- [Scheme-it](https://www.digikey.pl/en/schemeit/project) - tool for schematics creation
- [Proxmox-Helper-Scripts](https://tteck.github.io/Proxmox/) - helper scripts for ESPHome LXC (and mayn other) installation as a separate environment
- Boards
  - ESP32-CAM
    - https://lastminuteengineers.com/esp32-cam-pinout-reference/
  - ESP32-C3 MINI
    - https://sklep.msalamon.pl/wp-content/uploads/2024/01/ESP32_C3_MINI-9.png
    - https://sklep.msalamon.pl/produkt/plytka-esp32-c3-super-mini-wifi-bluetooth/
    - https://pl.aliexpress.com/item/1005005834887898.html
  - ESP_01
  