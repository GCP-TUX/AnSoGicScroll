#!/bin/sh
set -e

RULES_FILE=/etc/udev/rules.d/99-ansogicscroll-uinput.rules

# La regla udev ya viene incluida en el paquete (ver CMakeLists.txt), pero
# la volvemos a escribir aquí por si el usuario la borró manualmente antes
# de una reinstalación.
cat > "$RULES_FILE" <<'EOF'
KERNEL=="uinput", MODE="0660", GROUP="input", TAG+="uaccess"
EOF

if command -v udevadm >/dev/null 2>&1; then
    udevadm control --reload-rules || true
    udevadm trigger || true
fi

# Carga el módulo uinput ahora, y en cada arranque futuro.
if command -v modprobe >/dev/null 2>&1; then
    modprobe uinput 2>/dev/null || true
fi
echo "uinput" > /etc/modules-load.d/ansogicscroll-uinput.conf 2>/dev/null || true

# Agrega automáticamente al usuario real (no root) que ejecutó la
# instalación al grupo 'input', necesario para el autoscroll en X11.
# $SUDO_USER lo define apt/dnf cuando se invoca via sudo.
TARGET_USER="${SUDO_USER:-}"
if [ -n "$TARGET_USER" ] && [ "$TARGET_USER" != "root" ]; then
    if command -v usermod >/dev/null 2>&1; then
        usermod -aG input "$TARGET_USER" || true
        echo ""
        echo "AnSoGicScroll: se agrego el usuario '$TARGET_USER' al grupo 'input'"
        echo "para habilitar el autoscroll en X11. Cierra sesion y vuelve a"
        echo "entrar (o reinicia) para que el cambio tenga efecto."
        echo ""
    fi
fi

exit 0
