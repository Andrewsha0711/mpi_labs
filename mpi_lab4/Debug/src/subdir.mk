################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/mpi_lab4.cpp 

CPP_DEPS += \
./src/mpi_lab4.d 

OBJS += \
./src/mpi_lab4.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	/usr/lib64/openmpi/bin/mpic++ -I/usr/include/openmpi-x86_64 -I/usr/include -I/usr/include/c++/12 -I/usr/include/c++/12/backward -I/usr/include/c++/12/x86_64-redhat-linux -I/usr/local/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/mpi_lab4.d ./src/mpi_lab4.o

.PHONY: clean-src

