# --- Modificaciones de Patrik para Kria ---

# 1. Herramientas básicas de desarrollo y Python
IMAGE_INSTALL:append = " \
    python3 \
    python3-pip \
    python3-core \
    python3-numpy \
    git \
    nano \
    htop \
    util-linux \
    static-ip-eth0 \
"

# 2. Herramientas Exclusivas de Xilinx Kria (Hardware Manager)
IMAGE_INSTALL:append = " \
    xmutil \
    dfx-mgr \
    libdfx \
    xrt \
"

# 3. Asegurar espacio extra en disco (opcional pero recomendado)
IMAGE_ROOTFS_EXTRA_SPACE = "524288"
