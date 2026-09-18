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
- access to your home Wi-Fi details

## First-Time Setup

### 1. Power the controller

After boot, a new Wi-Fi network named `ESP32Mach` should appear.

- The configuration portal uses the controller's AP address `192.168.100.1`.
- If no Wi-Fi credentials are stored, the portal stays available until you explicitly leave it.

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

The `Heater Powerswitch` toggles between:

- `ON`: PID control is allowed to drive the heater
- `OFF`: heater output is forced off

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