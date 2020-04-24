################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/CRC.c \
../src/Client.c \
../src/Hello_world.c \
../src/Server.c \
../src/UART_EIVE_Protocol_Flags.c \
../src/UART_EIVE_Protocol_Recv.c \
../src/UART_EIVE_Protocol_Send.c 

OBJS += \
./src/CRC.o \
./src/Client.o \
./src/Hello_world.o \
./src/Server.o \
./src/UART_EIVE_Protocol_Flags.o \
./src/UART_EIVE_Protocol_Recv.o \
./src/UART_EIVE_Protocol_Send.o 

C_DEPS += \
./src/CRC.d \
./src/Client.d \
./src/Hello_world.d \
./src/Server.d \
./src/UART_EIVE_Protocol_Flags.d \
./src/UART_EIVE_Protocol_Recv.d \
./src/UART_EIVE_Protocol_Send.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


