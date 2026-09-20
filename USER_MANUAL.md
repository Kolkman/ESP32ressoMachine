# ESP32ressoMachine User Manual

This guide is for daily use: connecting the controller, reaching the web interface, adjusting settings, and updating firmware.

## Safety

- This firmware controls a mains-powered heating appliance. Incorrect wiring, enclosure design, or relay selection can create shock, fire, or burn hazards.
- Do not work on the machine while it is powered.
- Do not leave the machine unattended until heater switching and sensor readings have been validated.

## Before You Start

You need:

- the powered controller
- a phone or computer with Wi-Fi
- a web browser

You may additionally need
- access to your home Wi-Fi details
- an account on an MQTT server, for automation.

## First-Time Setup

### 1. Power the controller

After boot, a new Wi-Fi network named `ESP32Mach` should appear. The password of this network is: "ESP32ressoMachine". Connecting to this network gives you access to the configuration portal.

- The configuration portal uses the controller's AP address `192.168.100.1`.
- If Wi-Fi is enabled and Wi-Fi credentials are stored, the portal stays available until you explicitly leave it.

If your machine has a physical interafce with buttons then pressing the black button during the start-up will start the configuration portal and you can change settings. 


### 2. Connect to the setup access point

Join the `ESP32Mach` network from your phone or computer. Most devices should be redirected automatically to the captive portal. If that does not happen, open `http://192.168.100.1/` manually.

### 3. Choose your Wi-Fi network

On the Network Configuration page:

- Press `Rescan` to refresh visible networks.
- Select the SSID you want to use.
- Enter the Wi-Fi password.
- Optionally fill in MQTT settings.
- Press `Save`.

### 4. Finish setup

After saving, use `Exit` to leave the configuration portal. The device saves the configuration and restarts, then attempts to join one of the stored Wi-Fi networks.

## Finding The Device On Your Network

Once connected to Wi-Fi, open the controller in a browser by using one of these methods:

- The IP address assigned by your router
- The configured hostname on your LAN, which defaults to `silvia`

If hostname resolution is unreliable on your network, use the router-assigned IP address.

### Home page

The home page is the normal operating dashboard. It shows:

- Boiler temperature gauge
- Heater power gauge
- Heater power switch
- Firmware version string
- A `Settings` button

The page updates live through server-sent events from the controller.

### Heater power switch

Holding the `Heater Powerswitch` (black) for a few seconds toggles between: 

- `ON`: PID control is allowed to drive the heater
- `OFF`: heater output is forced off

When held a few seconds longer the system will be forst to reboot this allow you to enter the 
control portal by pressing the black button after the reboot happened.

This switch is available directly from the dashboard and does not require logging in. 


## Logging In

Changing configuration requires authentication.

Default credentials in this repository are:

- Username: `admin`
- Password: `silvia`

To log in:

1. Open `Settings`.
2. Press `Login` if prompted.
3. Enter the username and password.

After login, the settings controls become visible.

## Settings Page

The Settings page lets you:

- change the target temperature and controller values
- save or reload configuration
- restore defaults
- reboot the device
- open the firmware update page

Behavior to remember:

- Changes take effect immediately.
- Changes are not permanent until you press `Save Config`.
- `Target Temperature` is limited to `110` °C.

If you use the tuning controls, supervise the machine while tuning is active.

## Saving Changes

Use `Save Config` after changing settings you want to keep. Without that step, the controller reverts to the previously saved configuration on the next reboot.

## MQTT Integration

If MQTT is enabled in your build and configured on the Network Configuration page, the controller connects to the broker using the configured host, port, username, password, and topic prefix.

The configured MQTT topic acts as a base path. By default this base topic is `Espresso` in the setup page, while the firmware fallback default is `EspressoMach` if no saved value exists.

The controller uses two MQTT topics:

- Read-only status topic: `<mqttTopic>/status`
- Read/write control topic: `<mqttTopic>/config`

### Read-only status topic

The controller publishes JSON status messages to `<mqttTopic>/status` while it is connected to the broker.

Available fields in the published JSON are:

- `time`: controller uptime counter used internally by the firmware
- `measuredTemperature`: current measured boiler temperature
- `intTemperature`: internal MCU temperature reading
- `targetTemperature`: active target temperature setpoint
- `heaterPower`: current heater output value
- `externalControlMode`: whether external control mode is active
- `externalButtonState`: current external button state reported by the firmware
- `powerOffMode`: whether the machine is in power-off mode
- `tuning`: whether PID autotuning is currently active
- `heap`: free heap in bytes
- `heapMaxAl`: largest allocatable heap block in bytes
- `FloatingAvg`: floating average temperature used by the control logic

Example status payload:

```json
{
	"time": 123456,
	"measuredTemperature": 96.8,
	"intTemperature": 32.1,
	"targetTemperature": 98.5,
	"heaterPower": 41.2,
	"externalControlMode": false,
	"externalButtonState": 0,
	"powerOffMode": false,
	"tuning": false,
	"heap": 201344,
	"heapMaxAl": 110580,
	"FloatingAvg": 96.5
}
```

### Read/write control topic

The controller subscribes to `<mqttTopic>/config` and accepts JSON payloads.

Supported writable fields are:

- `targetTemp`: updates the target temperature immediately
- `powerOffMode`: intended to switch power-off mode on or off

Example payload to change the target temperature:

```json
{
	"targetTemp": 99.0
}
```

Example payload intended to change power-off mode:

```json
{
	"powerOffMode": "true"
}
```

### MQTT write caveats

- MQTT changes affect the running controller immediately.
- MQTT changes are runtime control inputs. They do not save configuration to flash.
- The current firmware implementation clearly supports `targetTemp` updates.
- The current `powerOffMode` MQTT handling in this revision is inconsistent with the rest of the API, so treat it as experimental and verify behavior on your device before relying on it for automation.

## Firmware Update

The controller supports browser-based OTA updates.

To update firmware:

1. Open the Settings page.
2. Press `Update Firmware`.
3. Select the firmware file.
4. Start the upload.
5. Wait for the reboot countdown to complete.

- The update page requires authentication.
- The device redirects back to the home page after reboot.
- Do not interrupt power during the upload.

## Troubleshooting

### The setup portal does not appear

- Look for the `ESP32Mach` Wi-Fi network.
- If your device does not auto-open the portal, browse to `http://192.168.100.1/`.
- If the controller already has valid Wi-Fi credentials, it may go straight to normal operation.

### The device does not join Wi-Fi after setup

- Re-enter the configuration portal and verify the SSID and password.
- Check that your Wi-Fi password is correct.

### I changed settings but they reverted after reboot

- You likely changed runtime values only.
- Open the Settings page and press `Save Config`.

### The Settings page says I need to log in

- Use the `Login` button on the page.
- The repository defaults are `admin` / `silvia` unless your build changed them.

### The heater never turns on

- Make sure `Heater Powerswitch` is `ON`.
- Verify that the measured temperature is below the target.
- Check sensor wiring and heater relay wiring.
- Confirm the machine is not in a power-off state.

### The machine powers off after sitting idle

The firmware includes a power-safe auto-off function intended to switch heating off after a long stable period.

### OTA update fails

- Confirm you are logged in.
- Make sure you selected the correct firmware image.
- Retry from a stable network connection.
- Avoid closing the browser until the reboot sequence finishes.

## Daily Use Tips

- Let the machine warm up fully before judging temperature stability.
- Change one setting group at a time.
- Save known-good settings once the machine behaves the way you want.

For implementation details, API behavior, compile-time defaults, and maintenance tasks, see [MAINTAINER_GUIDE.md](MAINTAINER_GUIDE.md).
- Change one parameter group at a time.
- Save known-good settings after successful tests.
- Keep a copy of your preferred settings so you can restore them quickly.

## Related Files

If you are maintaining or customizing the device, the most relevant repository entry points are:

- `README.md`
- `WEBsources/configuration.html`
- `WEBsources/networkSetup.html`
- `WEBsources/index.html`
- `src/webInterfaceAPI.cpp`
- `src/wifiManager.cpp`