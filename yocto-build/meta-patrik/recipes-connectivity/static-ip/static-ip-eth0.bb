SUMMARY = "Configuracion de IP Estatica para eth0"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://10-eth0.network"

S = "${WORKDIR}"

do_install() {
    # Crear la carpeta de destino en el sistema de archivos de la imagen
    install -d ${D}${sysconfdir}/systemd/network
    
    # Copiar nuestro archivo de configuracion
    install -m 0644 ${WORKDIR}/10-eth0.network ${D}${sysconfdir}/systemd/network/
}

FILES:${PN} += "${sysconfdir}/systemd/network/10-eth0.network"
