# -------------------------------------------------------------------------
# Script HSI para generar Device Tree desde XSA (Metodo del Usuario)
#
# Uso en XSCT:
# source generate_dts.tcl
# generate_dts <path_to_xsa> <path_to_device_tree_repo> <output_dir>
# -------------------------------------------------------------------------

proc generate_dts {xsa_path repo_path out_dir} {
    puts "--> 1. Abriendo diseno de hardware: $xsa_path"
    hsi open_hw_design $xsa_path

    puts "--> 2. Configurando repositorio Device Tree: $repo_path"
    hsi set_repo_path $repo_path

    puts "--> 3. Creando diseno de software para PSU Cortex-A53_0..."
    hsi create_sw_design device-tree -os device_tree -proc psu_cortexa53_0

    # Opcional: Activar overlay si se requiere explicitamente (comentado por defecto si no lo usaste)
    # common::set_property CONFIG.dt_overlay true [hsi::get_os]

    puts "--> 4. Generando archivos en: $out_dir"
    hsi generate_target -dir $out_dir

    puts "--> 5. Cerrando diseno."
    hsi close_hw_design [hsi current_hw_design]
    
    puts "-----------------------------------------------------------"
    puts "Exito! Archivos .dtsi generados en $out_dir"
    puts "   Recuerda compilar el pl.dtsi a .dtbo con dtc."
    puts "-----------------------------------------------------------"
}

puts "Script cargado. Ejecuta el comando: generate_dts <xsa> <repo> <out>"