################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/cash_register.c \
../Src/main.c \
../Src/mfrc522.c \
../Src/queue.c \
../Src/spi.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/timing.c \
../Src/usart.c 

OBJS += \
./Src/cash_register.o \
./Src/main.o \
./Src/mfrc522.o \
./Src/queue.o \
./Src/spi.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/timing.o \
./Src/usart.o 

C_DEPS += \
./Src/cash_register.d \
./Src/main.d \
./Src/mfrc522.d \
./Src/queue.d \
./Src/spi.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/timing.d \
./Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32F0 -DNUCLEO_F091RC -DSTM32F091RCTx -DSTM32F091xC -c -I../Inc -I../CMSIS -O0 -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/cash_register.cyclo ./Src/cash_register.d ./Src/cash_register.o ./Src/cash_register.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/mfrc522.cyclo ./Src/mfrc522.d ./Src/mfrc522.o ./Src/mfrc522.su ./Src/queue.cyclo ./Src/queue.d ./Src/queue.o ./Src/queue.su ./Src/spi.cyclo ./Src/spi.d ./Src/spi.o ./Src/spi.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su ./Src/timing.cyclo ./Src/timing.d ./Src/timing.o ./Src/timing.su ./Src/usart.cyclo ./Src/usart.d ./Src/usart.o ./Src/usart.su

.PHONY: clean-Src

