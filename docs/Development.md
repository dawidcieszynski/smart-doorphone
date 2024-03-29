# Development

## Environment

1.  ESPHome - used separated ESPHome LXC instance here
     - [Proxmox-Helper-Scripts](https://tteck.github.io/Proxmox/) - helper scripts for ESPHome LXC (and mayn other) installation as a separate environment
        - ```bash -c "$(wget -qLO - https://github.com/tteck/Proxmox/raw/main/ct/esphome.sh)"```
          - custom settings: disk size 32GB, enable root access
        - to update ESPHome just type ```update``` in the LXC console.
2. VSCode + Remote - SSH extension
   - ```code --folder-uri "vscode-remote://ssh-remote+root@192.168.10.128/root/config"```
3. Source code - depending on the ESPHome configuration clone repo into the ESPHome config folder
      - for ESPHome LXC config folder is /root/config
      ```
      cd /root/config
      git clone https://github.com/dawidcieszynski/smart-doorphone.git
      cp ./smart-doorphone/src/sdp.template.yaml ./sdp.yaml
      cp ./smart-doorphone/src/sdp-simulator.template.yaml ./sdp-simulator.yaml
      ```
      now you should have the configurations automatically detected
      ![Image](../docs/img/esphome-sdp-projects.png)
4. Build firmware using ESPHome
   1. Click three-dots menu -> Install -> Manual download - you will get the firmware
   2. Upload the firmware using [web.esphome.io](https://web.esphome.io/) using proper usb interface
5. Now you should be able to update firmware wirelessly
    ![Image](../docs/img/esphome-sdp-projects-online.png)
    (or use commandline: ```esphome compile *.yaml; esphome upload *.yaml```)
    and use the devices by webbrowser
    ![Image](../docs/img/sdp-web.png)
    ![Image](../docs/img/sdp-simulator-web.png)
    ![Image](../docs/img/sdp-both.png)
    additionally these devices should be detected by Home Assistant
    ![Image](../docs/img/ha-sdp-detected.png)
## Tools

- [Home Assistant Community Add-on: Studio Code Server](https://github.com/hassio-addons/addon-vscode) - useful for editing and commiting code

## Commands

### Refresh code in Home Assistant

```cd /config/smart-doorphone/; git pull --rebase```

