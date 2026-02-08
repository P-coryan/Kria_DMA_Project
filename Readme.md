# 🚀 Kria KV260: Edge Acceleration & Custom OS Platform

**Unlock the full potential of Heterogeneous Computing.**

> **Status:** ✅ Stable (Yocto 2024.1 + 64-bit DMA Loopback)

This repository serves as the **foundational framework** for high-performance Edge Computing on the **Xilinx Kria KV260**.  
It is not just a “Hello World”; it is a **production-ready baseline** that integrates a **Custom Yocto Linux OS** with **High-Speed FPGA Hardware Acceleration**.

---

## ⚡ The Philosophy: Why This Matters

In the era of Edge AI, relying on cloud servers introduces **latency, bandwidth costs, and privacy risks**.  
The Kria KV260 enables local processing, but the **ARM Cortex-A53 CPU alone is not enough**.

This project demonstrates the **Offloading Paradigm**:

- **🧠 CPU (Linux):** High-level logic, networking, orchestration.
- **🦾 FPGA (Hardware):** Massive parallel acceleration for math, signal processing, and vision.

### 🎯 The Goal

Prove that **large data volumes** can be transferred efficiently from FPGA to CPU memory  
(**High DDR > 4 GB**) using **AXI DMA (64-bit)** — without saturating the processor.

This repository establishes a **reliable HW + OS + Driver pipeline** that future projects
(Vision, Radar, RF processing, Edge AI) can build upon.

---

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

## 👤 Author

**Patricio Carrasco O'Ryan**  
*Embedded Systems & FPGA Engineer*  
*Focus: Edge AI · Heterogeneous Computing · Linux Kernel & Drivers*

---

*Powered by Xilinx Kria KV260 & Yocto Project.*
