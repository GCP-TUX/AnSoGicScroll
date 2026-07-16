# Instalación vía APT (Linux)

## Debian / Ubuntu 24.04 o más reciente

```bash
# 1. Importar la clave GPG del repositorio
curl -fsSL https://gcp-tux.github.io/AnSoGicScroll/pubkey.gpg | \
    sudo gpg --dearmor -o /usr/share/keyrings/ansogicscroll.gpg

# 2. Agregar el repositorio
echo "deb [signed-by=/usr/share/keyrings/ansogicscroll.gpg] \
https://gcp-tux.github.io/AnSoGicScroll stable main" | \
    sudo tee /etc/apt/sources.list.d/ansogicscroll.list

# 3. Instalar
sudo apt update
sudo apt install ansogicscroll
```

## Ubuntu 22.04

Igual que arriba, pero usando `jammy` en vez de `stable`:

```bash
curl -fsSL https://gcp-tux.github.io/AnSoGicScroll/pubkey.gpg | \
    sudo gpg --dearmor -o /usr/share/keyrings/ansogicscroll.gpg

echo "deb [signed-by=/usr/share/keyrings/ansogicscroll.gpg] \
https://gcp-tux.github.io/AnSoGicScroll jammy main" | \
    sudo tee /etc/apt/sources.list.d/ansogicscroll.list

sudo apt update
sudo apt install ansogicscroll
```
