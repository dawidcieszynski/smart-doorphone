# Installation

1. Get source into Home Assistant
    - Go to config directory using some SSH shell
    - Clone git repository and make links so ESPHome will use the yamls files directly:
    ```
    git clone https://github.com/dawidcieszynski/smart-doorphone.git

    mv /config/smart-doorphone/src/sdp.yaml /config/esphome/sdp.yaml
    mv /config/smart-doorphone/src/sdp-simulator.yaml /config/esphome/sdp-simulator.yaml
    ln /config/esphome/sdp.yaml /config/smart-doorphone/src/sdp.yaml
    ln /config/esphome/sdp-simulator.yaml /config/smart-doorphone/src/sdp-simulator.yaml
    
    ```
   
2. Build and install firmware into the devices