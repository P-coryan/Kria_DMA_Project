# --- CONFIGURACION KRIA KV260 ---
set project_name "Kria_DMA_64bit"
set part_name "xck26-sfvc784-2LV-c"
set board_part "xilinx.com:kv260_som:part0:1.3"
set bd_script "build_bd.tcl"

# --- 1. CREAR PROYECTO LIMPIO ---
# Crea el proyecto en una subcarpeta llamada 'vivado_project' para no ensuciar
create_project $project_name ./vivado_project -part $part_name -force
set_property board_part $board_part [current_project]

# --- 2. IMPORTAR EL BLOCK DESIGN ---
# Ejecuta el script que generaste desde Vivado
source $bd_script

# --- 3. GENERAR EL WRAPPER HDL ---
# Esto convierte el diagrama de bloques en codigo Verilog sintetizable
make_wrapper -files [get_files ./vivado_project/${project_name}.srcs/sources_1/bd/design_1/design_1.bd] -top
add_files -norecurse ./vivado_project/${project_name}.gen/sources_1/bd/design_1/hdl/design_1_wrapper.v

# --- 4. LISTO PARA COMPILAR ---
update_compile_order -fileset sources_1
puts "---------------------------------------------------------"
puts "¡EXITO! Proyecto reconstruido. Ahora dale a 'Generate Bitstream'"
puts "---------------------------------------------------------"