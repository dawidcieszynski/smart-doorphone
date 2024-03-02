# Installation

1. Get source into Home Assistant
    - Go to config directory using some SSH shell
    - Clone git repository and make links so ESPHome will use the yamls files directly:
    ```
    git clone https://github.com/dawidcieszynski/smart-doorphone.git

    ln /config/smart-doorphone/src/sdp.yaml /config/esphome/sdp.yaml
    ln /config/smart-doorphone/src/sdp-simulator.yaml /config/esphome/sdp-simulator.yaml
    ```
   
2. Build and install firmware into the devices