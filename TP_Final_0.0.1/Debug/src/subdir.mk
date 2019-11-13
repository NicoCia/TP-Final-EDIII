################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Multiplex.c \
../src/Pesaje.c \
../src/TP_Final_0.0.1.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/logica.c \
../src/teclado.c 

OBJS += \
./src/Multiplex.o \
./src/Pesaje.o \
./src/TP_Final_0.0.1.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/logica.o \
./src/teclado.o 

C_DEPS += \
./src/Multiplex.d \
./src/Pesaje.d \
./src/TP_Final_0.0.1.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/logica.d \
./src/teclado.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"C:\Users\nicoc\Documents\Electronica Digital III\LPCXpresso\LPCXpresso_8.2.2_650\workspace2\CMSISv2p00_LPC17xx\Drivers\inc" -I"C:\Users\nicoc\Documents\Electronica Digital III\LPCXpresso\LPCXpresso_8.2.2_650\workspace2\CMSISv2p00_LPC17xx\inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -mcpu=cortex-m3 -mthumb -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


