################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../ADC.c \
../analog_servo_drv.c \
../buttons.c \
../CAN.c \
../cmd_sts_helpers.c \
../eeprom_storage.c \
../events.c \
../framework.c \
../IOC.c \
../light_drv.c \
../light_setting_alg.c \
../lin_drv.c \
../LIN_XCVR_WD_Kicker.c \
../main.c \
../master_service.c \
../MS_LIN_top_layer.c \
../PWM.c \
../slave_number_setting_SM.c \
../slave_parameters.c \
../slave_service.c \
../SPI.c \
../SPI_Service.c \
../timer.c \
../__template.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
ADC.o \
analog_servo_drv.o \
buttons.o \
CAN.o \
cmd_sts_helpers.o \
eeprom_storage.o \
events.o \
framework.o \
IOC.o \
light_drv.o \
light_setting_alg.o \
lin_drv.o \
LIN_XCVR_WD_Kicker.o \
main.o \
master_service.o \
MS_LIN_top_layer.o \
PWM.o \
slave_number_setting_SM.o \
slave_parameters.o \
slave_service.o \
SPI.o \
SPI_Service.o \
timer.o \
__template.o

OBJS_AS_ARGS +=  \
ADC.o \
analog_servo_drv.o \
buttons.o \
CAN.o \
cmd_sts_helpers.o \
eeprom_storage.o \
events.o \
framework.o \
IOC.o \
light_drv.o \
light_setting_alg.o \
lin_drv.o \
LIN_XCVR_WD_Kicker.o \
main.o \
master_service.o \
MS_LIN_top_layer.o \
PWM.o \
slave_number_setting_SM.o \
slave_parameters.o \
slave_service.o \
SPI.o \
SPI_Service.o \
timer.o \
__template.o

C_DEPS +=  \
ADC.d \
analog_servo_drv.d \
buttons.d \
CAN.d \
cmd_sts_helpers.d \
eeprom_storage.d \
events.d \
framework.d \
IOC.d \
light_drv.d \
light_setting_alg.d \
lin_drv.d \
LIN_XCVR_WD_Kicker.d \
main.d \
master_service.d \
MS_LIN_top_layer.d \
PWM.d \
slave_number_setting_SM.d \
slave_parameters.d \
slave_service.d \
SPI.d \
SPI_Service.d \
timer.d \
__template.d

C_DEPS_AS_ARGS +=  \
ADC.d \
analog_servo_drv.d \
buttons.d \
CAN.d \
cmd_sts_helpers.d \
eeprom_storage.d \
events.d \
framework.d \
IOC.d \
light_drv.d \
light_setting_alg.d \
lin_drv.d \
LIN_XCVR_WD_Kicker.d \
main.d \
master_service.d \
MS_LIN_top_layer.d \
PWM.d \
slave_number_setting_SM.d \
slave_parameters.d \
slave_service.d \
SPI.d \
SPI_Service.d \
timer.d \
__template.d

OUTPUT_FILE_PATH +=Ford_360_Lighting_Project.elf

OUTPUT_FILE_PATH_AS_ARGS +=Ford_360_Lighting_Project.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler

















































./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATautomotive_DFP\1.1.96\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=ata6617c -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATautomotive_DFP\1.1.96\gcc\dev\ata6617c" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Ford_360_Lighting_Project.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=ata6617c -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATautomotive_DFP\1.1.96\gcc\dev\ata6617c"  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Ford_360_Lighting_Project.elf" "Ford_360_Lighting_Project.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Ford_360_Lighting_Project.elf" "Ford_360_Lighting_Project.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Ford_360_Lighting_Project.elf" > "Ford_360_Lighting_Project.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Ford_360_Lighting_Project.elf" "Ford_360_Lighting_Project.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "Ford_360_Lighting_Project.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Ford_360_Lighting_Project.elf" "Ford_360_Lighting_Project.a" "Ford_360_Lighting_Project.hex" "Ford_360_Lighting_Project.lss" "Ford_360_Lighting_Project.eep" "Ford_360_Lighting_Project.map" "Ford_360_Lighting_Project.srec" "Ford_360_Lighting_Project.usersignatures"
	