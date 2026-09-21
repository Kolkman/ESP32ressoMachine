# ESP32ressoMachine Maintainer Guide

## Purpose

This guide is for people building, modifying, flashing, or supporting the firmware. It collects the implementation-facing details that do not belong in the shorter end-user manual.

## Main User Flows

The firmware exposes two primary operating modes:

1. Configuration portal mode
2. Normal operating mode

Configuration portal mode is entered when no Wi-Fi credentials are stored, or when local hardware interaction triggers setup. In that mode, the device brings up its own access point and serves the captive portal. After configuration, the device restarts and attempts to join one of the stored Wi-Fi networks.

## Default Runtime And UI Endpoints

Main pages:

- `/`
- `/config`
- `/configuration.html`
- `/update`
- `/WebLogin.html`

Primary API endpoints:

- `/api/v1/status`
- `/api/v1/firmware`
- `/api/v1/get`
- `/api/v1/set`
- `/api/v1/pwr`
- `/api/v1/statistics`
- `/api/v1/config`

Relevant route registration lives in `src/webinterface.cpp`, `src/webInterfaceAPI.cpp`, and `src/webInterfaceOTAUpdate.cpp`.

## Configuration Portal Behavior

The AP and portal behavior are currently defined by the Wi-Fi manager implementation.

Current defaults:

- AP SSID: `ESP32Mach`
- AP IP: `192.168.100.1`
- stored Wi-Fi slots: `2`
- config portal timeout constant: `60 * 1000`

Important implementation note:

- `setupWiFiAp()` derives an AP password internally, but the current `WiFi.softAP()` call starts the AP with SSID only. Do not document a required setup password unless that code path changes.

The portal stays available indefinitely when no Wi-Fi credentials exist because `_waitingForClientAction` remains true until the user exits configuration.

## Authentication

Normal operation requires authentication for configuration-changing endpoints and for the OTA page.

Repository defaults:

- username: `admin`
- password: `silvia`
- hostname: `silvia`

These defaults are defined in `include/ESPressoMachineDefaults.h` and may also be overridden by local secrets headers depending on the build setup.

Authentication uses a cookie derived from username, password, and client IP. The login and cookie flow is implemented in `src/EspressoWebServer.cpp`.

## Core Configurable Parameters

User-facing settings map to runtime config state roughly as follows:

- `tset`: target temperature
- `tband`: near-target band
- `eqPwr`: feed-forward equilibrium power
- `pgain`, `igain`, `dgain`: near-target PID
- `apgain`, `aigain`, `adgain`: outside-band PID
- `PidInterval`: PID calculation interval
- `HeaterInterval`: heater duty update interval
- `sensorSampleInterval`: sensor refresh interval
- `maxCool`: downward smoothing limit
- `powersafeTimeout`: intended auto-off timeout

The HTTP handlers for reading and writing these values are in `src/webInterfaceAPI.cpp`.

## Persistence Model

There are two classes of configuration changes:

- runtime-only changes applied immediately in memory
- saved changes persisted in LittleFS as `/config.json`

Load, save, and reset behavior:

- `/api/v1/config?load`: load from file and reconfigure runtime
- `/api/v1/config?save`: save current runtime config
- `/api/v1/config?default`: reset runtime config to defaults

Wi-Fi portal submissions use `safeandrestart`, save the config, serve a completion page, and reboot.

## Default Firmware Values

Repository defaults include:

- target temperature: `98.5`
- temperature band: `1.5`
- near-target PID: `50 / 1.5 / 500`
- outside-band PID: `8 / 0.1 / 400`
- equilibrium power: `32.5`
- heater interval: `100` ms
- PID interval: `500` ms
- max cool: `0.025`
- fail-safe temperature: `112.5` °C

See `include/ESPressoMachineDefaults.h` for the current source of truth.

## Known User-Visible Limitations

- `Target Temperature` writes are clamped to `MAXTEMP`, currently `110` °C.

If you are updating the documentation again later, keep these caveats aligned with the code.

## OTA Update Path

OTA is implemented through `/update.html` and `/doUpdate`.

Behavior:

- GET `/update` redirects to `/update.html`
- the update page requires authentication
- POST `/doUpdate` accepts the upload and reboots on success
- files whose names contain `spiffs` are written to the SPIFFS/LittleFS update target; others use the flash target

Implementation is in `src/webInterfaceOTAUpdate.cpp`.

## UI Sources And Generated Assets

Editable web assets live under `WEBsources/`.

Important files:

- `WEBsources/index.html`
- `WEBsources/index_helper.js`
- `WEBsources/configuration.html`
- `WEBsources/configuration_helper.js`
- `WEBsources/networkSetup.html`
- `WEBsources/networkConfigPage.js`
- `WEBsources/WebLogin.html`

The firmware serves generated page headers from `include/pages/` through macros such as `DEF_HANDLE_*`. If you change UI source files, confirm the generated page artifacts are refreshed by the repository's existing asset pipeline.

This repository relies on the MinifyAll extension to generate `WEBsources/*-min.js`, `*-min.html`, and similar minified derivatives from the editable originals.

Before changing a web asset, check the `Makefile` rule for the generated header you are touching. Some served assets are still packaged from the minified derivative rather than the editable source. For example, `include/pages/networkConfigPage.js.h` is generated from `WEBsources/networkConfigPage-min.js`, so edits made only in `WEBsources/networkConfigPage.js` will not reach the device unless the minified file is refreshed or the generation rule is changed.

If browser behavior does not match your source edits, compare all three of these before debugging runtime code:

- the editable asset in `WEBsources/`
- the generated `*-min.*` derivative
- the `Makefile` rule that produces the served header in `include/pages/`

## Hardware-Dependent Features

The default build configuration in this repository enables:

- MQTT
- serial logging
- OLED interface
- button support through the OLED or LCD path
- NTP clock support on OLED builds

These are compile-time options in `include/ESPressoMachineDefaults.h`.

## Suggested Support Checklist

When diagnosing user issues, check these in order:

1. Whether the device is in config portal mode or normal mode.
2. Whether Wi-Fi credentials were actually saved.
3. Whether the user is hitting the correct IP or hostname.
4. Whether the heater is disabled by `powerOffMode`.
5. Whether sensor readings look sane.
6. Whether changes were applied only at runtime and never saved.
7. Whether the issue is an auth problem, especially around settings and OTA.

## Relevant Source Files

- `src/interface.cpp`
- `src/wifiManager.cpp`
- `src/webinterface.cpp`
- `src/webInterfaceAPI.cpp`
- `src/webInterfaceOTAUpdate.cpp`
- `src/ESPressoMachine.cpp`
- `src/config.cpp`
- `include/ESPressoMachineDefaults.h`