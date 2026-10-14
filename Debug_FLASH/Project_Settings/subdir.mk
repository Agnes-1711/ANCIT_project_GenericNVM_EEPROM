################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Project_Settings/eeprom_i2c.c \
../Project_Settings/nvm_block.c 

OBJS += \
./Project_Settings/eeprom_i2c.o \
./Project_Settings/nvm_block.o 

C_DEPS += \
./Project_Settings/eeprom_i2c.d \
./Project_Settings/nvm_block.d 


# Each subdirectory must supply rules for building sources it contributes
Project_Settings/%.o: ../Project_Settings/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Project_Settings/eeprom_i2c.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


