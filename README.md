# 🚀 NitroLinux - Ultimate Control Center for Acer Nitro

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Platform](https://img.shields.io/badge/platform-Linux-green.svg)
![Tech](https://img.shields.io/badge/backend-C%2B%2B%20%7C%20Qt5-red.svg)

**NitroLinux** is a powerful, open-source alternative to NitroSense for Linux users. It provides full control over your Acer Nitro's cooling system, power profiles, and real-time monitoring, bypassing BIOS limitations directly via the Embedded Controller (EC).

---

## 📸 Screenshots

| **Dashboard & Monitoring** | **Max Performance Mode** |
|:---:|:---:|
| ![Main Dashboard](Screenshot%201%20NitroLinux.png) | ![Max Speed](Screenshot%202%20NitroLinux.png) |
| *Real-time Graphs & Auto Mode* | *Full Speed (6000+ RPM)* |

### Custom Control
![Custom Mode](Screenshot%203%20NitroLinux.png)
*Precise manual control with slider (0-100%)*

## NitroLinux

---

## ✨ Features

* **⚡ Fan Control Modes:**
    * **Auto:** Lets the BIOS handle thermals (Default).
    * **Max:** Forces fans to absolute maximum (CoolBoost override).
    * **Custom:** Set a fixed speed percentage (0-100%) with a slider.
* **❤️ Heartbeat System:** Continuously overrides the BIOS watchdog to prevent fans from resetting to "Auto" during gaming.
* **📊 Real-Time Monitoring:**
    * Dual Graphs for CPU & GPU (Temperature + Usage).
    * Uses smart sensor detection (`x86_pkg_temp`) and Nvidia driver APIs.
* **🔋 Power Plan Integration:** Switch between *Power Saver*, *Balanced*, and *Performance* directly from the app (wraps `power-profiles-daemon`).
* **⌨️ "N" Key Support:** Automatic hardware mapping of the physical "Nitro" key to launch the app instantly.
* **🎨 Modern UI:** Dark-themed, gaming-inspired interface built with Qt5.

---

## 🛠️ Installation

NitroLinux comes with a smart universal installer that compiles the source code specifically for your system and sets up all necessary permissions.

### Prerequisites
You need a Linux distribution (Ubuntu, Fedora, Arch, etc.) and `git`.

### 1-Click Install
Open your terminal and run:

```bash
# 1. Clone the repository
git clone [https://github.com/mcpf2001/NitroLinux.git](https://github.com/mcpf2001/NitroLinux.git)
cd NitroLinux


```
What the installer does:

    Detects your distro and checks for dependencies (qmake, g++).

    Compiles the backend (C) and frontend (C++ Qt).

    Installs the binary to /usr/local/bin/.

    Configures sudoers so you don't need a password to open it.

    Maps the physical "N" key using a systemd service and auto-configures GNOME shortcuts.

    Sets up the desktop shortcut and icons.

🎮 Usage
Launching the App

    Physical Key: Just press the "N" key on your numpad.

    Menu: Search for "NitroLinux" in your application menu.

    Terminal: Run sudo nitrolinux.

Modes Explained

    Auto: The safest mode for daily use.

    Max: Use this for heavy gaming or rendering. Fans will spin at ~6122 RPM.

    Custom: Useful if you want a constant airflow without the noise of Max mode.

    Power Plans:

        Power Saver: Limits CPU boost to save battery.

        Balanced: Standard behavior.

        Performance: Aggressive CPU scaling (Requires AC Adapter).

🔧 Under the Hood

This project combines high-level UI with low-level kernel operations:

    Frontend: C++ using the Qt5 Widgets framework for a responsive UI.

    Backend: Pure C using ioctl and lseek to write directly to /sys/kernel/debug/ec/ec0/io.

    Registers: Writes to registers 0x37 (CPU) and 0x3A (GPU) for speed control, and 0x22/0x21 for mode switching.

    Hardware Mapping: Uses setkeycodes f5 148 to remap the proprietary Acer key scan code to Launch1.

## ⚠️ Disclaimer

This software writes directly to your laptop's Embedded Controller (EC) registers.

* **Supported Models:** Strictly tested on **Acer Nitro 5 (AN515-54)**.
* **Compatibility:** It will **likely NOT work** on other models (even other Acer Nitro versions). Using this on unsupported hardware may lead to unexpected behavior, system instability, or fans not responding.

**Use it at your own risk.** The developer is not responsible for any hardware damage or data loss caused by improper usage or running this software on incompatible devices.
👨‍💻 Credits

Developed by Christian (mcpf2001.github.io).

# 2. Run the installer
sudo ./install.sh
