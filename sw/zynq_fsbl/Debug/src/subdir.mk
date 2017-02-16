################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/_exit.c \
../src/ddr_init.c \
../src/ff.c \
../src/image_mover.c \
../src/main.c \
../src/mio.c \
../src/mmc.c \
../src/nand.c \
../src/nor.c \
../src/outbyte.c \
../src/pcap.c \
../src/ps7_init.c \
../src/qspi.c \
../src/sd.c \
../src/uart.c \
../src/xbasic_types.c \
../src/xil_io.c \
../src/xil_printf.c \
../src/xnandpss.c \
../src/xnandpss_bbm.c \
../src/xnandpss_onfi.c \
../src/xuartpss_hw.c 

LD_SRCS += \
../src/lscript.ld 

S_UPPER_SRCS += \
../src/boot.S \
../src/cpu_init.S \
../src/fsbl_handoff.S \
../src/xil-crt0.S 

S_SRCS += \
../src/asm_vectors.s \
../src/translation_table.s 

OBJS += \
./src/_exit.o \
./src/asm_vectors.o \
./src/boot.o \
./src/cpu_init.o \
./src/ddr_init.o \
./src/ff.o \
./src/fsbl_handoff.o \
./src/image_mover.o \
./src/main.o \
./src/mio.o \
./src/mmc.o \
./src/nand.o \
./src/nor.o \
./src/outbyte.o \
./src/pcap.o \
./src/ps7_init.o \
./src/qspi.o \
./src/sd.o \
./src/translation_table.o \
./src/uart.o \
./src/xbasic_types.o \
./src/xil-crt0.o \
./src/xil_io.o \
./src/xil_printf.o \
./src/xnandpss.o \
./src/xnandpss_bbm.o \
./src/xnandpss_onfi.o \
./src/xuartpss_hw.o 

C_DEPS += \
./src/_exit.d \
./src/ddr_init.d \
./src/ff.d \
./src/image_mover.d \
./src/main.d \
./src/mio.d \
./src/mmc.d \
./src/nand.d \
./src/nor.d \
./src/outbyte.d \
./src/pcap.d \
./src/ps7_init.d \
./src/qspi.d \
./src/sd.d \
./src/uart.d \
./src/xbasic_types.d \
./src/xil_io.d \
./src/xil_printf.d \
./src/xnandpss.d \
./src/xnandpss_bbm.d \
./src/xnandpss_onfi.d \
./src/xuartpss_hw.d 

S_UPPER_DEPS += \
./src/boot.d \
./src/cpu_init.d \
./src/fsbl_handoff.d \
./src/xil-crt0.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: ARM gcc compiler
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -mfloat-abi=soft -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '

src/%.o: ../src/%.s
	@echo Building file: $<
	@echo Invoking: ARM gcc assembler
	arm-xilinx-eabi-gcc -c  -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '

src/%.o: ../src/%.S
	@echo Building file: $<
	@echo Invoking: ARM gcc compiler
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -c -fmessage-length=0 -mfloat-abi=soft -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


