# 🌍 Environmental Monitoring and Data Logging System (STM32 + ESP8266 + Sensors)

This project is a real-time environmental monitoring system developed using an STM32 microcontroller. It monitors **temperature**, **humidity**, and **gas levels**, displays the data on an **OLED screen**, logs it to an **SD card**, and transmits alerts via **Wi-Fi** using an ESP8266 module.

---

## 📦 Features

- **Sensors Used**:
  - 📈 **DHT22** – for temperature and humidity
  - 🔥 **MQ6** – for detecting flammable gases

- **Microcontroller**:
  - 🧠 **STM32F103C8T6 (Blue Pill)**

- **Display**:
  - 🖥️ SSD1306 OLED (I2C)

- **Data Logging**:
  - 💾 SD card (FATFS) with timestamped entries

- **Connectivity**:
  - 🌐 ESP8266 (AT Commands) for Wi-Fi transmission
  - 🖥️ Server-side control for resetting alarms

- **Alarms**:
  - 🚨 Buzzer + LED alerts for dangerous gas levels

---

## 📁 Repository Contents

