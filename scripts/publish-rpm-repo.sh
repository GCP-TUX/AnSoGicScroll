#!/usr/bin/env bash
# =============================================================
# publish-rpm-repo.sh
# Construye/actualiza un repositorio DNF/YUM firmado con GPG
# dentro de ./repo/rpm (para publicar junto al repo APT en gh-pages).
#
# Requiere: createrepo_c, gnupg
# =============================================================
set -euo pipefail

REPO_DIR="${REPO_DIR:-repo}"
RPM_FILE="${1:?Uso: publish-rpm-repo.sh <ruta-al-archivo.rpm>}"
FEDORA_VER="${FEDORA_VER:?Falta la variable FEDORA_VER}"
KEY_ID="${GPG_KEY_ID:?Falta la variable GPG_KEY_ID}"
GPG_PASSPHRASE="${GPG_PASSPHRASE:?Falta la variable GPG_PASSPHRASE}"

RPM_DIR="${REPO_DIR}/rpm/${FEDORA_VER}"

echo "==> Preparando estructura del repo RPM en ${RPM_DIR}"
mkdir -p "${RPM_DIR}"

echo "==> Copiando paquete .rpm"
cp "${RPM_FILE}" "${RPM_DIR}/"
echo "==> Contenido de ${RPM_DIR}:"
ls -la "${RPM_DIR}"

echo "==> Generando metadata del repo (createrepo_c)"
createrepo_c --update "${RPM_DIR}"

echo "==> Firmando repomd.xml con GPG (KEY_ID=${KEY_ID})"
rm -f "${RPM_DIR}/repodata/repomd.xml.asc"
gpg --pinentry-mode loopback --passphrase "${GPG_PASSPHRASE}" \
    --default-key "${KEY_ID}" --batch --yes --detach-sign --armor \
    -o "${RPM_DIR}/repodata/repomd.xml.asc" "${RPM_DIR}/repodata/repomd.xml"

echo "==> Listo. Estructura final del repo RPM:"
find "${RPM_DIR}" -maxdepth 2 -type f
