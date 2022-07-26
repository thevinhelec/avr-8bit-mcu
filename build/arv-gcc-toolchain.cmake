#
# AVR GCC Toolchain file
#
# @author Natesh Narain. see more at https://nnarain.github.io/2016/03/29/AVR-CMake-Toolchain.html
# @since Feb 06 2016

# @editor thevinh

set(TRIPLE "avr")

# find the toolchain root directory

if(UNIX)

    set(OS_SUFFIX "")
    find_path(TOOLCHAIN_ROOT
        NAMES
            ${TRIPLE}-gcc${OS_SUFFIX}

        PATHS
            #${CMAKE_CURRENT_LIST_DIR}/../
    )

elseif(WIN32)

    set(OS_SUFFIX ".exe")
    find_path(TOOLCHAIN_ROOT
        NAMES
            ${TRIPLE}-gcc${OS_SUFFIX}

        PATHS
            #${CMAKE_CURRENT_LIST_DIR}/../AVRToolchain/bin
    )

else(UNIX)
    message(FATAL_ERROR "toolchain not supported on this OS")
endif(UNIX)

if(NOT TOOLCHAIN_ROOT)
    message(FATAL_ERROR "Toolchain root could not be found!!!")
endif(NOT TOOLCHAIN_ROOT)

# setup the AVR compiler variables

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)
set(CMAKE_CROSS_COMPILING 1)

set(CMAKE_C_COMPILER   "${TOOLCHAIN_ROOT}/${TRIPLE}-gcc${OS_SUFFIX}"     CACHE PATH "gcc"     FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}/${TRIPLE}-g++${OS_SUFFIX}"     CACHE PATH "g++"     FORCE)
set(CMAKE_AR           "${TOOLCHAIN_ROOT}/${TRIPLE}-ar${OS_SUFFIX}"      CACHE PATH "ar"      FORCE)
set(CMAKE_LINKER       "${TOOLCHAIN_ROOT}/${TRIPLE}-ld${OS_SUFFIX}"      CACHE PATH "linker"  FORCE)
set(CMAKE_NM           "${TOOLCHAIN_ROOT}/${TRIPLE}-nm${OS_SUFFIX}"      CACHE PATH "nm"      FORCE)
set(CMAKE_OBJCOPY      "${TOOLCHAIN_ROOT}/${TRIPLE}-objcopy${OS_SUFFIX}" CACHE PATH "objcopy" FORCE)
set(CMAKE_OBJDUMP      "${TOOLCHAIN_ROOT}/${TRIPLE}-objdump${OS_SUFFIX}" CACHE PATH "objdump" FORCE)
set(CMAKE_STRIP        "${TOOLCHAIN_ROOT}/${TRIPLE}-strip${OS_SUFFIX}"   CACHE PATH "strip"   FORCE)
set(CMAKE_RANLIB       "${TOOLCHAIN_ROOT}/${TRIPLE}-ranlib${OS_SUFFIX}"  CACHE PATH "ranlib"  FORCE)
set(AVR_SIZE           "${TOOLCHAIN_ROOT}/${TRIPLE}-size${OS_SUFFIX}"    CACHE PATH "size"    FORCE)

#set(CMAKE_EXE_LINKER_FLAGS "-L /usr/lib/gcc/avr/4.8.2")


# setup the avr exectable macro

set(AVR_LINKER_LIBS "-lm")

macro(add_avr_executable target_name avr_mcu)

    set(elf_file ${target_name}-${avr_mcu}.elf)
    set(eep_file ${target_name}-${avr_mcu}.eep)
    set(map_file ${target_name}-${avr_mcu}.map)
    set(hex_file ${target_name}-${avr_mcu}.hex)
    set(lss_file ${target_name}-${avr_mcu}.lss)

    # create elf file
    add_executable(${elf_file}
        ${ARGN}
    )

    set_target_properties(
        ${elf_file}

        PROPERTIES
            COMPILE_FLAGS "-mmcu=${avr_mcu} -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -c -std=gnu99" # add -g2 to debug
            LINK_FLAGS    "-mmcu=${avr_mcu} -Wl,-Map=${map_file} ${AVR_LINKER_LIBS}"
    )

    # create hex file
    add_custom_command(
        OUTPUT ${hex_file}

        COMMAND
            ${CMAKE_OBJCOPY} -O ihex -R .eeprom -R .fuse -R .lock -R .signature ${elf_file} ${hex_file}

        DEPENDS ${elf_file}
    )

    # create epp file
    add_custom_command(
        OUTPUT ${eep_file}

        COMMAND
            ${CMAKE_OBJCOPY} -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex ${elf_file} ${eep_file} || exit 0

            DEPENDS ${elf_file}
    )

    # generate the lss file
    add_custom_command(
        OUTPUT ${lss_file}

        COMMAND
            ${CMAKE_OBJDUMP} -h -S ${elf_file} > ${lss_file}

        DEPENDS ${elf_file}
    )

    add_custom_command(
        OUTPUT "print-size-${elf_file}"

        COMMAND
            ${AVR_SIZE} -C --mcu=${avr_mcu}  ${elf_file}

        DEPENDS ${elf_file}
    )

    # build the intel hex file for the device
    add_custom_target(
        ${target_name}
        ALL
        DEPENDS ${hex_file} ${lss_file} ${eep_file} "print-size-${elf_file}"
    )

    set_target_properties(
        ${target_name}

        PROPERTIES
            OUTPUT_NAME ${elf_file}
    )
endmacro(add_avr_executable)