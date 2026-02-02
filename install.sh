#!/bin/bash

# Colori
GREEN='\033[0;32m'
RED='\033[0;31m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${GREEN}=== INSTALLER NITROLINUX AUTOMATICO v4.0 ===${NC}"

# 1. Controllo Root
if [ "$EUID" -ne 0 ]; then
  echo -e "${RED}Errore: Esegui come root (sudo ./install.sh)${NC}"
  exit 1
fi

# Rileva utente reale
REAL_USER=$SUDO_USER
if [ -z "$REAL_USER" ]; then REAL_USER=$(whoami); fi
USER_UID=$(id -u $REAL_USER)

# 2. Controllo Dipendenze
if ! command -v qmake &> /dev/null; then
    echo -e "${RED}Errore: qmake non trovato. Installa le librerie Qt5.${NC}"
    exit 1
fi

echo "1. Compilazione..."
make clean &> /dev/null
rm -f Makefile
if [ -f "nitrolinux.pro" ]; then
    if command -v qmake-qt5 &> /dev/null; then qmake-qt5 nitrolinux.pro; else qmake nitrolinux.pro; fi
else
    echo -e "${RED}Errore: File nitrolinux.pro non trovato!${NC}"
    exit 1
fi
make -j$(nproc)

if [ ! -f "nitrolinux" ]; then
    echo -e "${RED}Compilazione fallita.${NC}"
    exit 1
fi

echo "2. Installazione File..."
cp nitrolinux /usr/local/bin/
chmod +x /usr/local/bin/nitrolinux
cp nitro.png /usr/share/icons/nitro.png
chmod 644 /usr/share/icons/nitro.png

echo "3. Configurazione Permessi (No Password)..."
echo "$REAL_USER ALL=(root) NOPASSWD: /usr/local/bin/nitrolinux" > /etc/sudoers.d/nitrolinux
chmod 440 /etc/sudoers.d/nitrolinux

echo "4. Creazione Menu Applicazioni..."
cat > /usr/share/applications/nitrolinux.desktop <<EOF
[Desktop Entry]
Name=NitroLinux
Comment=Fan Control for Acer Nitro
Exec=sudo /usr/local/bin/nitrolinux
Icon=/usr/share/icons/nitro.png
Terminal=false
Type=Application
Categories=System;Settings;
StartupNotify=true
StartupWMClass=nitrolinux
EOF
chmod 644 /usr/share/applications/nitrolinux.desktop

echo "5. Configurazione Hardware Tasto 'N'..."
cat > /etc/systemd/system/nitro-keymap.service <<EOF
[Unit]
Description=Map Acer Nitro Key to Launch1
After=multi-user.target

[Service]
Type=oneshot
ExecStart=/usr/bin/setkeycodes f5 148

[Install]
WantedBy=multi-user.target
EOF
systemctl daemon-reload
systemctl enable nitro-keymap.service &> /dev/null
systemctl start nitro-keymap.service

# --- FIX: SCORCIATOIA SENZA DBUS-LAUNCH ---
echo "6. Configurazione Scorciatoia GNOME..."

cat > /tmp/setup_shortcut.py <<EOF
import sys
try:
    from gi.repository import Gio, GLib
except ImportError:
    print("Librerie Python GI mancanti. Salto.")
    sys.exit(0)

SCHEMA = "org.gnome.settings-daemon.plugins.media-keys"
NAME = "NitroLinux"
CMD = "sudo /usr/local/bin/nitrolinux"
BINDING = "Launch1" 
CUSTOM_PATH = "/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom-nitro/"

settings = Gio.Settings(schema=SCHEMA)
current_list = settings.get_strv("custom-keybindings")

if CUSTOM_PATH not in current_list:
    current_list.append(CUSTOM_PATH)
    settings.set_strv("custom-keybindings", current_list)

new_key = Gio.Settings(schema_id="org.gnome.settings-daemon.plugins.media-keys.custom-keybinding", path=CUSTOM_PATH)
new_key.set_string("name", NAME)
new_key.set_string("command", CMD)
new_key.set_string("binding", BINDING)

print("Scorciatoia configurata!")
EOF

if command -v python3 &> /dev/null; then
    # USIAMO IL PERCORSO SOCKET DIRETTO (Evita l'errore dbus-launch)
    DBUS_ADDR="unix:path=/run/user/$USER_UID/bus"
    sudo -u $REAL_USER env DBUS_SESSION_BUS_ADDRESS="$DBUS_ADDR" python3 /tmp/setup_shortcut.py
else
    echo "Python3 non trovato."
fi
rm -f /tmp/setup_shortcut.py

update-desktop-database /usr/share/applications/ &> /dev/null
gtk-update-icon-cache /usr/share/icons/ &> /dev/null

echo "7. Pulizia..."
make clean &> /dev/null
rm -f Makefile .qmake.stash

echo -e "${GREEN}=== TUTTO PRONTO! ===${NC}"
echo -e "Il tasto 'N' è attivo. Premilo per lanciare NitroLinux."