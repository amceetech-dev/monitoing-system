# 🌍 Environmental Monitoring and Data Logging System (STM32 + ESP8266 + Sensors)

This project is a real-time environmental monitoring system developed using an STM32 microcontroller. It monitors **temperature**, **humidity**, and **gas levels**, displays the data on an **OLED screen**, logs it to an **SD card**, and transmits alerts via **Wi-Fi** using an ESP8266 module.

---

## 📦 Features

- **Sensors Used**:
  - 📈 **DHT22** – for temperature and humidity
  - 🔥 **MQ6** – for detecting flammable gases

- **Microcontroller**:
  - 🧠 **STM32F446RE (Nucleo)**

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

## 📁  Prototype

![Pict1](https://github.com/user-attachments/assets/fde6c720-c6be-457f-a3e2-71af47ab67c2)
<img width="452" alt="Pict2" src="https://github.com/user-attachments/assets/1e3cdb37-96cc-4496-8257-b32360321d15" />

