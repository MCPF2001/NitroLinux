#!/bin/bash

# Colori
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${RED}=== RIMOZIONE TOTALE NITROLINUX ===${NC}"

if [ "$EUID" -ne 0 ]; then
  echo "Errore: Esegui come root (sudo ./uninstall.sh)"
  exit 1
fi

# Rileva utente reale (per pulire le sue impostazioni GNOME)
REAL_USER=$SUDO_USER
if [ -z "$REAL_USER" ]; then REAL_USER=$(whoami); fi
USER_UID=$(id -u $REAL_USER)

echo "1. Rimozione file di sistema..."
rm -f /usr/local/bin/nitrolinux
rm -f /usr/share/icons/nitro.png
rm -f /usr/share/applications/nitrolinux.desktop
rm -f /etc/sudoers.d/nitrolinux

# Pulizia vecchi nomi (per sicurezza)
rm -f /usr/local/bin/NitroLinuxSense
rm -f /usr/share/applications/nitrosense.desktop
rm -f /etc/sudoers.d/nitrosense-app

echo "2. Rimozione servizio hardware (Mappatura Tasti)..."
if systemctl list-units --full -all | grep -q "nitro-keymap.service"; then
    systemctl stop nitro-keymap.service
    systemctl disable nitro-keymap.service &> /dev/null
    rm -f /etc/systemd/system/nitro-keymap.service
    systemctl daemon-reload
    echo "   -> Servizio rimosso."
fi

# --- PARTE NUOVA: RIMOZIONE SCORCIATOIA GNOME ---
echo "3. Rimozione Scorciatoia GNOME..."

cat > /tmp/remove_shortcut.py <<EOF
import sys
try:
    from gi.repository import Gio, GLib
except ImportError:
    sys.exit(0)

SCHEMA = "org.gnome.settings-daemon.plugins.media-keys"
CUSTOM_PATH = "/org/gnome/settings-daemon/plugins/media-keys/custom-keybindings/custom-nitro/"

settings = Gio.Settings(schema=SCHEMA)
current_list = settings.get_strv("custom-keybindings")

if CUSTOM_PATH in current_list:
    # Rimuovi il percorso dalla lista
    current_list.remove(CUSTOM_PATH)
    settings.set_strv("custom-keybindings", current_list)
    
    # Resetta le impostazioni dentro quel percorso (pulizia profonda)
    path_settings = Gio.Settings(schema_id="org.gnome.settings-daemon.plugins.media-keys.custom-keybinding", path=CUSTOM_PATH)
    path_settings.reset("name")
    path_settings.reset("command")
    path_settings.reset("binding")
    print("   -> Scorciatoia rimossa dalle impostazioni utente.")
else:
    print("   -> Scorciatoia non trovata (già rimossa?).")
EOF

if command -v python3 &> /dev/null; then
    # Usiamo lo stesso trucco dell'installer per parlare con DBus dell'utente
    DBUS_ADDR="unix:path=/run/user/$USER_UID/bus"
    sudo -u $REAL_USER env DBUS_SESSION_BUS_ADDRESS="$DBUS_ADDR" python3 /tmp/remove_shortcut.py
fi
rm -f /tmp/remove_shortcut.py

echo "4. Aggiornamento cache..."
update-desktop-database /usr/share/applications/ &> /dev/null
gtk-update-icon-cache /usr/share/icons/ &> /dev/null

echo -e "${GREEN}Disinstallazione completata.${NC}"
echo "Il sistema è pulito come se NitroLinux non fosse mai esistito."