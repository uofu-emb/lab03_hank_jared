# This is the main binary. List your C files here.
add_executable(thread
    thread.c
    )

pico_set_program_name(thread "test")
pico_set_program_version(thread "0.1")

# Modify the below lines to enable/disable output over UART/USB.
# These are mutually exclusive, only set one.
pico_enable_stdio_uart(thread 0)
pico_enable_stdio_usb(thread 1)

# Add the libraries to the build
target_link_libraries(thread PRIVATE
  pico_stdlib
  pico_multicore
  pico_async_context_freertos
  FreeRTOS-Kernel-Heap4
)

# The CYW43 is the Wifi/Bluetooth module. If the board is set to pico_w, this
# variable will be set and the wireless libraries added.
if(PICO_CYW43_SUPPORTED)
    # For led support on pico_w
    target_link_libraries(thread PRIVATE
        pico_cyw43_arch_none
    )
endif()

# Another pico macro which generates the uf2 file in addition to the elf
pico_add_extra_outputs(thread)

add_custom_target(flash picotool load -f $<TARGET_FILE:thread> DEPENDS thread)
