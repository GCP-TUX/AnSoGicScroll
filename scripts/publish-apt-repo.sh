#!/usr/bin/env bash
# =============================================================
# publish-apt-repo.sh
# Construye/actualiza un repositorio APT "flat dists" firmado con GPG
# y lo deja listo en la carpeta ./repo para publicar (ej. GitHub Pages).
#
# Requiere: dpkg-dev (dpkg-scanpackages), apt-utils (apt-ftparchive), gnupg
# =============================================================
set -euo pipefail

REPO_DIR="${REPO_DIR:-repo}"
DEB_FILE="${1:?Uso: publish-apt-repo.sh <ruta-al-archivo.deb>}"
CODENAME="${CODENAME:-stable}"
ARCH="${ARCH:-amd64}"
PKG_NAME="ansogicscroll"
KEY_ID="${GPG_KEY_ID:?Falta la variable GPG_KEY_ID}"

echo "==> Preparando estructura del repo en ${REPO_DIR}"
POOL_DIR="${REPO_DIR}/pool/main/a/${PKG_NAME}"
DIST_DIR="${REPO_DIR}/dists/${CODENAME}/main/binary-${ARCH}"
mkdir -p "${POOL_DIR}" "${DIST_DIR}"

echo "==> Copiando paquete .deb al pool"
cp "${DEB_FILE}" "${POOL_DIR}/"

echo "==> Generando Packages / Packages.gz"
cd "${REPO_DIR}"
dpkg-scanpackages --arch "${ARCH}" pool/ > "dists/${CODENAME}/main/binary-${ARCH}/Packages"
gzip -9fk "dists/${CODENAME}/main/binary-${ARCH}/Packages"

echo "==> Generando Release"
cat > "dists/${CODENAME}/Release" <<EOF
Origin: AnSoGicScroll
Label: AnSoGicScroll
Suite: ${CODENAME}
Codename: ${CODENAME}
Architectures: ${ARCH}
Components: main
Description: Repositorio APT de AnSoGicScroll
Date: $(date -Ru)
EOF

# Apéndice de hashes (requerido por apt para validar integridad)
apt-ftparchive release "dists/${CODENAME}" >> "dists/${CODENAME}/Release"

echo "==> Firmando Release con GPG (KEY_ID=${KEY_ID})"
rm -f "dists/${CODENAME}/Release.gpg" "dists/${CODENAME}/InRelease"
gpg --default-key "${KEY_ID}" --batch --yes -abs \
    -o "dists/${CODENAME}/Release.gpg" "dists/${CODENAME}/Release"
gpg --default-key "${KEY_ID}" --batch --yes --clearsign \
    -o "dists/${CODENAME}/InRelease" "dists/${CODENAME}/Release"

echo "==> Copiando clave pública al root del repo"
cp ../pubkey.gpg . 2>/dev/null || echo "AVISO: no se encontró pubkey.gpg en el nivel superior, cópialo manualmente."

echo "==> Listo. Estructura final:"
find . -maxdepth 4 -type f
