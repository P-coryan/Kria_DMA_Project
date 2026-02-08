#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

// --- CONFIGURACIÓN HARDWARE ---
#define DMA_BASE_ADDR 0xA0000000
#define MAP_SIZE 65536 

// Offsets AXI DMA (Soporte 64-bits)
#define MM2S_DMACR     0x00  // TX Control
#define MM2S_DMASR     0x04  // TX Status
#define MM2S_SA_LSB    0x18  // TX Source Addr (Baja)
#define MM2S_SA_MSB    0x1C  // TX Source Addr (Alta - Bits 32+)
#define MM2S_LENGTH    0x28  // TX Length

#define S2MM_DMACR     0x30  // RX Control
#define S2MM_DMASR     0x34  // RX Status
#define S2MM_DA_LSB    0x48  // RX Dest Addr (Baja)
#define S2MM_DA_MSB    0x4C  // RX Dest Addr (Alta - Bits 32+)
#define S2MM_LENGTH    0x58  // RX Length

#define TEST_SIZE 4096 // 4KB

// Función para traducir Virtual -> Físico usando /proc/self/pagemap
uintptr_t virt_to_phys(void *virt) {
    long page_size = sysconf(_SC_PAGESIZE);
    uintptr_t vaddr = (uintptr_t)virt;
    long page_idx = vaddr / page_size;
    long offset = vaddr % page_size;
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) return 0;
    
    uint64_t entry;
    if (lseek(fd, page_idx * 8, SEEK_SET) == -1 || read(fd, &entry, 8) != 8) {
        close(fd); return 0;
    }
    close(fd);
    
    // Bit 63 indica si la página está presente en RAM
    if (!(entry & (1ULL << 63))) return 0;
    
    // PFN (Page Frame Number) son los bits 0-54
    uint64_t pfn = entry & ((1ULL << 55) - 1);
    return (pfn * page_size) + offset;
}

int main() {
    // Abrir /dev/mem con O_SYNC para saltar la caché (Cache Bypass)
    int dh = open("/dev/mem", O_RDWR | O_SYNC);
    if (dh == -1) { perror("Error crítico abriendo /dev/mem"); return -1; }

    // 1. Mapear Registros de Control del DMA
    uint32_t *dma = mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dh, DMA_BASE_ADDR);
    if (dma == MAP_FAILED) { perror("Mmap DMA failed"); return -1; }

    printf("=== AXI DMA TEST V2 (64-bit High DDR Support) ===\n");

    // 2. Preparar memoria RAM normal (4KB alineados)
    void *src_ram, *dst_ram;
    if (posix_memalign(&src_ram, 4096, TEST_SIZE) != 0) { perror("Alloc src failed"); return -1; }
    if (posix_memalign(&dst_ram, 4096, TEST_SIZE) != 0) { perror("Alloc dst failed"); return -1; }
    
    // Bloquear en RAM física para evitar Swap
    mlock(src_ram, TEST_SIZE);
    mlock(dst_ram, TEST_SIZE);

    // 3. Obtener direcciones físicas reales
    uintptr_t src_phys = virt_to_phys(src_ram);
    uintptr_t dst_phys = virt_to_phys(dst_ram);
    
    printf("Phys Src: 0x%lx\n", src_phys);
    printf("Phys Dst: 0x%lx\n", dst_phys);

    if (src_phys == 0 || dst_phys == 0) {
        fprintf(stderr, "Error: No se pudo resolver la dirección física.\n");
        return -1;
    }

    // 4. TRUCO DE MAGIA: Remapear como Uncached
    // Creamos una ventana directa a la RAM saltando la caché del procesador
    uint32_t *src_uncached = mmap(NULL, TEST_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dh, src_phys);
    uint32_t *dst_uncached = mmap(NULL, TEST_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, dh, dst_phys);

    if (src_uncached == MAP_FAILED || dst_uncached == MAP_FAILED) {
        perror("Error en remapeo uncached"); return -1;
    }

    // 5. Llenar datos (Escribiendo directo a DDR)
    for(int i=0; i<TEST_SIZE/4; i++) {
        src_uncached[i] = 0xAA000000 + i; 
        dst_uncached[i] = 0xDEADBEEF;     // Basura inicial conocida
    }

    // 6. Configurar y Disparar DMA (Soporte 64 bits)
    // Resetear DMA
    dma[MM2S_DMACR/4] = 4; dma[S2MM_DMACR/4] = 4;
    usleep(1000);

    // --- CONFIGURACIÓN CANAL RX (Destino) ---
    dma[S2MM_DMACR/4] = 1; // Run
    // Escribir dirección en dos partes (LSB y MSB)
    dma[S2MM_DA_LSB/4] = (uint32_t)(dst_phys & 0xFFFFFFFF);
    dma[S2MM_DA_MSB/4] = (uint32_t)((dst_phys >> 32) & 0xFFFFFFFF);
    dma[S2MM_LENGTH/4] = TEST_SIZE; // ¡Disparo RX!

    // --- CONFIGURACIÓN CANAL TX (Origen) ---
    dma[MM2S_DMACR/4] = 1; // Run
    // Escribir dirección en dos partes (LSB y MSB)
    dma[MM2S_SA_LSB/4] = (uint32_t)(src_phys & 0xFFFFFFFF);
    dma[MM2S_SA_MSB/4] = (uint32_t)((src_phys >> 32) & 0xFFFFFFFF);
    dma[MM2S_LENGTH/4] = TEST_SIZE; // ¡Disparo TX!

    printf("--> DMA Running (64-bit address mode)...\n");

    // 7. Esperar a que termine (Polling)
    int timeout = 100000;
    while(timeout > 0) {
        uint32_t sr_tx = dma[MM2S_DMASR/4];
        uint32_t sr_rx = dma[S2MM_DMASR/4];
        
        // Verificar bit Idle (bit 1) en ambos canales
        if ((sr_tx & 0x02) && (sr_rx & 0x02)) break;
        
        // Verificar errores (bits 4, 5, 6)
        if ((sr_tx & 0x70) || (sr_rx & 0x70)) {
            printf("\nERROR DE HARDWARE DETECTADO!\n");
            printf("Status TX: 0x%08X\n", sr_tx);
            printf("Status RX: 0x%08X\n", sr_rx);
            break;
        }

        timeout--;
    }

    if(timeout == 0) printf("TIMEOUT! El DMA no responde.\n");
    else printf("--> Transferencia completada.\n");

    // 8. Verificar Resultados (Leyendo directo de DDR)
    int errors = 0;
    for(int i=0; i<TEST_SIZE/4; i++) {
        if(dst_uncached[i] != src_uncached[i]) {
            printf("Error [%d]: Esperado 0x%08X, Leído 0x%08X\n", i, src_uncached[i], dst_uncached[i]);
            errors++;
            if(errors > 5) {
                printf("...demasiados errores, abortando print.\n");
                break;
            }
        }
    }

    if(errors == 0) printf("\nSUCCESS! 🎉 Datos verificados correctamente.\n");
    else printf("\nFAILURE! ❌ Fallo de verificación (%d errores).\n", errors);

    // Limpieza
    munmap(src_uncached, TEST_SIZE);
    munmap(dst_uncached, TEST_SIZE);
    munmap(dma, MAP_SIZE);
    close(dh);
    return 0;
}
