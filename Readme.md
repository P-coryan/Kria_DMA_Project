# 🚀 Kria KV260: Custom Yocto Linux & FPGA SoC Integration

**Unlock the full potential of Heterogeneous Computing.**

> **Status:** ✅ Stable (Yocto 2024.1 + 64-bit DMA Loopback)

This repository serves as the **foundational framework** for high-performance Edge Computing on the **Xilinx Kria KV260**.  
It is not just a “Hello World”; it is a **production-ready baseline** that integrates a **Custom Yocto Linux OS** with **High-Speed FPGA Hardware Acceleration**.

---

## ⚡ The Philosophy: From Silicon to System

Before diving into complex **Edge AI** , it is crucial to validate the integrity of the **hardware-to-software bridge**.  
This project represents the successful **“Day 1”** of my **Kria KV260** development journey—moving from a fresh out-of-the-box board to a fully functional, custom-tailored system.

---

## 🧪 Phase 0: The Hardware Handshake

This initial implementation focuses on a **64-bit DMA loopback**.  
Why? Because before the Kria can *“see”* or *“think”*, it must be able to *“move”* data reliably and at high speed.

### What This Validates
- **Hardware Validation** – Confirms the Kria KV260 hardware is defect-free.
- **System Integrity** – Proves the custom Yocto OS can communicate with the PL (Programmable Logic) at high throughput.
- **Infrastructure Baseline** – Establishes the reliable data pipeline required for future developments.

---

## 🎯 The Goal: Mastering the Offloading Paradigm

In the era of **Edge AI**, the **ARM Cortex-A53** CPU alone is not enough.  
This repository establishes the **foundational baseline** for efficient hardware acceleration:

- 🧠 **CPU (Linux)** – High-level logic, networking, control, and orchestration.
- 🦾 **FPGA (Hardware)** – Massive parallel acceleration for math and signal processing.
- 🌉 **DMA Bridge** – High-throughput data movement to system DDR (>4 GB) using a **64-bit AXI DMA**, without saturating the processor.


---

## 🚀 Latest Release & Quick Start

The latest release includes **pre-compiled binaries** to get the DMA Loopback running on your **Kria KV260** immediately.

### Included Assets
- **`.wic`** – Complete SD card image containing the OS and runtime environment.
- **`.bin` & `.dtbo`** – FPGA bitstream and upper-layer Device Tree Overlay for hardware loading.
- **`shell.json`** – Configuration file for the Kria `xmutil` tool.
- **`.xsa`** – Hardware handoff file for Vitis or custom software development.

---

## How to Use

### 1️⃣ Flash the SD Card
1. Download the **`.wic`** file from the **Releases** section.
2. Flash the image onto a microSD card (16GB+ recommended) using:
   - **BalenaEtcher**, or  
   - the `dd` command.
3. Insert the SD card into the KV260 and boot the board. the Login is `root`

    If you experience issues with the automatic boot process, refer to  
[Section 6: Flashing the SD Card from WSL2](./yocto-build/README.md#-booting-the-kria-from-sd-card-manual-recovery--permanent-fix)
for manual recovery and permanent fix instructions.

---

### 2️⃣ Deploy the Hardware Overlay
If you are already running an OS and only want to load the hardware design:

1. Transfer the following files to the board (via SCP or USB):
   - `.bin`
   - `.dtbo`
   - `shell.json`
    
   See [How to connect Windows 11 to the Kria via SSH](./software/README.md#-transferring-files-to-kria-using-scp) for detailed instructions on SSH and SCP access.



2. Create a new firmware directory:
   ```bash
   sudo mkdir /lib/firmware/xilinx/kria-dma
   sudo mv your_files.* /lib/firmware/xilinx/kria-dma/
    ```
3. Load the design using `xmutil`:
    ```bash
    sudo xmutil unloadapp
    sudo xmutil loadapp kria-dma
    ```
### 3️⃣ Run the Test

Once the hardware is loaded, you can use the provided  C applications (software folder) to verify the DMA loopback functionality.


## 🛠️ Environment & Prerequisites

This project uses a **hybrid workflow**:
- **Windows** for Hardware Design
- **Linux (WSL2)** for OS compilation

| Component | Tool | Version | OS |
|---------|------|---------|----|
| **FPGA Design** | Vivado Design Suite | **2024.2** | Windows 11 |
| **Firmware / DT** | Vitis / XSCT | **2024.2** | Windows 11 |
| **OS Build** | Yocto Project | Scarthgap / Langdale | **WSL2 (Ubuntu 22.04)** |

---

## ⚠️ Critical WSL2 Configuration (Read Before Building!)

Yocto is **resource-intensive**.  
If you use WSL2, you **must** allocate enough resources or builds will fail.

Create or edit the file:

```text
%UserProfile%\.wslconfig

[wsl2]
memory=12GB      # Minimum (16GB recommended)
processors=10    # Use available CPU cores
swap=90GB        # Critical for Yocto linking stages

```

After saving, restart WSL:
```powershell
wsl --shutdown
```

## 📂 Repository Structure

The project is organized into **clear functional domains**:

| Directory | Description |
|---------|-------------|
| `yocto-build/` | 🐧 **Start here.** Build a custom Linux OS from source using Yocto. Includes the custom layer `meta-kria-custom`. |
| `hardware/` | ⚙️ TCL scripts to regenerate the Vivado project with **AXI DMA (64-bit)**. |
| `os-integration/` | 🌲 Scripts to generate the **Device Tree Overlay (`.dtbo`)** and `shell.json` for `xmutil`. |
| `software/` | 💻 Userspace C application to benchmark and validate DMA transfers. |


---

## 🚀 Getting Started: End-to-End Workflow

Follow this **exact order** to replicate the system.

---

### Phase 1: Build the Foundation (OS) 🐧

Go to `yocto-build/README.md`.

1. Set up the Yocto environment in WSL2.
2. Build `core-image-full-cmdline`.
3. Flash the SD card and fix U-Boot variables.

**Result:** Kria boots with a custom Linux and a static IP.

---

### Phase 2: Design the Hardware (FPGA) ⚙️

Go to `hardware/README.md`.

1. Run the TCL script in **Vivado 2024.2**.
2. Generate the bitstream (`.bin`) and export hardware (`.xsa`).

**Result:** FPGA binaries ready for runtime loading.

---

### Phase 3: Bridge the Worlds (Integration) 🌉

Go to `os-integration/README.md`.

1. Extract Device Tree data from the `.xsa` using **XSCT**.
2. Generate `.dts` and compile the overlay (`.dtbo`).
3. Prepare `shell.json` for the FPGA Manager.

**Result:** Runtime-loadable hardware description.

---

### Phase 4: Verify Performance (Software) 🏎️

Go to `software/README.md`.

1. Copy overlays to the board via `scp`.
2. Load hardware using `xmutil`.
3. Compile and run the DMA benchmark.

**Result:** **SUCCESS!** 🎉 High-speed data movement between FPGA and DDR.

---

## 🔮 What’s Next?

With the **Data Highway (DMA)** and **Custom OS (Yocto)** fully stable, future work includes:

- **Custom FPGA IP:** Replace the DMA loopback with real accelerators.
- **Video Processing:** Leverage the Kria VCUMA / VCU.
- **Hardware-in-the-Loop (HIL):** Control systems and real-time validation.
- **Edge AI Pipelines:** Camera → FPGA → DDR → CPU → Network.

---

## License

This project is licensed under the MIT License – see the LICENSE file for details.

## 👤 Author

**Patricio Carrasco O'Ryan**  
*Embedded Systems & FPGA Engineer*  
*Focus: Edge AI · Heterogeneous Computing · Linux Kernel & Drivers*

---

*Powered by Xilinx Kria KV260 & Yocto Project.*
