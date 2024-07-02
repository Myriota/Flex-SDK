#!/usr/bin/env bash

FLEX_SDK_VERSION="1.0.0"
LIBFLEX_VERSION="1.1.0"
SYSTEM_IMAGE_VERSION="1.5.8-FlexSense"

FLEX_SDK_PREFIX="${DESTDIR}/${MESON_INSTALL_PREFIX}"
LIBFLEX_PREFIX="${FLEX_SDK_PREFIX}/subprojects/libflex"
DOC_PREFIX="${DESTDIR}/doc"

LIBFLEX_SOURCE="${MESON_BUILD_ROOT}/libflex.a"
LIBFLEX_DEST="${LIBFLEX_PREFIX}/lib/libflex-v${LIBFLEX_VERSION}.a"

SYSTEM_IMAGE_SOURCE="${MESON_BUILD_ROOT}/subprojects/Production/system_image.bin"
SYSTEM_IMAGE_DEST="${LIBFLEX_PREFIX}/system_image/system_image-${SYSTEM_IMAGE_VERSION}.bin"

DOC_SOURCE="${MESON_BUILD_ROOT}/doc/html/"
DOC_DEST="${DOC_PREFIX}/html"

mkdir -p "${LIBFLEX_DEST%/*}"
cp "${LIBFLEX_SOURCE}" "${LIBFLEX_DEST}"
sha256sum "${LIBFLEX_DEST}" |  sed "s,${LIBFLEX_DEST%/*}/,,g" > "${LIBFLEX_DEST}.sha256"
cp "${LIBFLEX_DEST}" "${DESTDIR}"
cp "${LIBFLEX_DEST}.sha256" "${DESTDIR}"


mkdir -p "${SYSTEM_IMAGE_DEST%/*}"
cp "${SYSTEM_IMAGE_SOURCE}" "${SYSTEM_IMAGE_DEST}"
sha256sum "${SYSTEM_IMAGE_DEST}" |  sed "s,${SYSTEM_IMAGE_DEST%/*}/,,g" > "${SYSTEM_IMAGE_DEST}.sha256"
cp "${SYSTEM_IMAGE_DEST}" "${DESTDIR}"
cp "${SYSTEM_IMAGE_DEST}.sha256" "${DESTDIR}"


tar --transform="s,^libflex,libflex-v${LIBFLEX_VERSION}," -cJvf "${DESTDIR}/libflex-v${LIBFLEX_VERSION}.tar.xz" -C "${LIBFLEX_PREFIX%/*}" libflex/
tar --transform="s,^flex-sdk,flex-sdk-v${LIBFLEX_VERSION}," -cJvf "${DESTDIR}/flex-sdk-v${FLEX_SDK_VERSION}.tar.xz" -C "${FLEX_SDK_PREFIX%/*}" flex-sdk/

if [[ -d "${DOC_SOURCE}" ]]; then
  mkdir -p "${DOC_DEST%/*}"
  cp -r "${DOC_SOURCE}" "${DOC_DEST}"
  tar --transform="s,^doc,flex-sdk-doc-v${LIBFLEX_VERSION}," -cJvf "${DESTDIR}/flex-sdk-doc-v${FLEX_SDK_VERSION}.tar.xz" -C "${DOC_PREFIX%/*}" doc/
fi

for archive in ${DESTDIR}/*.tar.xz; do
  sha256sum "${archive}" |  sed "s,${archive%/*}/,,g" > "${archive}.sha256"
done
