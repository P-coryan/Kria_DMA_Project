#!/usr/bin/env python3
import sys
import re

# -----------------------------------------------------------------------------
# PLANTILLA EXACTA (Sin amba_pl, DMA directo en overlay)
# -----------------------------------------------------------------------------
DTS_TEMPLATE = """/dts-v1/;
/plugin/;

/ {{
    /* FRAGMENTO 0: Carga del Bitstream */
    fragment@0 {{
        target-path = "/fpga-region";
        __overlay__ {{
            firmware-name = "{0}";
        }};
    }};

    /* FRAGMENTO 1: Nodos del Hardware (AXI DMA) */
    fragment@1 {{
        target-path = "/axi";
        __overlay__ {{
            #address-cells = <2>;
            #size-cells = <2>;

{1}
        }};
    }};
}};
"""

def extract_dma_node(content):
    """
    Busca y extrae quirúrgicamente el nodo del DMA (axi_dma_0: dma@...)
    ignorando la jerarquía 'amba_pl' o '/'.
    """
    # 1. Eliminar comentarios para evitar falsos positivos
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)

    # 2. Buscar el inicio del nodo DMA. 
    # Patrón: una palabra (etiqueta), dos puntos, espacio, "dma@", dirección hex, llave.
    # Ej: axi_dma_0: dma@a0000000 {
    match = re.search(r'(\w+:\s*dma@[0-9a-fA-F]+)\s*\{', content)

    if not match:
        raise ValueError("No se encontró ningún nodo 'dma@' en el archivo .dtsi")

    start_index = match.start()
    
    # 3. Algoritmo de balanceo de llaves para encontrar el cierre exacto
    # Empezamos a contar desde la primera llave encontrada
    open_braces = 0
    found_first_brace = False
    end_index = -1

    for i in range(start_index, len(content)):
        char = content[i]
        
        if char == '{':
            open_braces += 1
            found_first_brace = True
        elif char == '}':
            open_braces -= 1
        
        # Si ya encontramos la primera llave y el contador vuelve a cero, 
        # hemos encontrado el cierre del bloque.
        if found_first_brace and open_braces == 0:
            end_index = i + 1 # +1 para incluir la llave de cierre
            break
    
    if end_index != -1:
        # Extraemos el bloque y le añadimos punto y coma si le falta (Device tree standard)
        block = content[start_index:end_index]
        if content[end_index] == ';': 
             block += ';'
        return block
    
    return ""

def main():
    if len(sys.argv) != 4:
        print("❌ Uso: python3 gen_dt_overlay.py <input.dtsi> <firmware.bin> <output.dts>")
        sys.exit(1)

    input_dtsi = sys.argv[1]
    firmware_name = sys.argv[2]
    output_dts = sys.argv[3]

    try:
        # Leer archivo
        with open(input_dtsi, 'r') as f:
            raw_content = f.read()

        # Extraer SOLO el nodo DMA
        dma_node = extract_dma_node(raw_content)

        if not dma_node:
            print("❌ Error: No se pudo extraer el bloque del DMA.")
            sys.exit(1)

        # Indentar para que quede idéntico a tu ejemplo
        indented_content = "\n".join(["            " + line for line in dma_node.splitlines()])

        # Generar salida
        final_dts = DTS_TEMPLATE.format(firmware_name, indented_content)

        with open(output_dts, 'w') as f:
            f.write(final_dts)

        print(f"✅ Overlay generado: {output_dts}")
        print(f"   -> Jerarquía aplanada: 'amba_pl' eliminado, DMA insertado directamente.")

    except Exception as e:
        print(f"❌ Error crítico: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()