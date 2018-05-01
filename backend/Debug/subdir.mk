################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Bullet.cpp \
../Entity.cpp \
../GameMaster.cpp \
../Matrix.cpp \
../NeuralNet.cpp \
../Player.cpp \
../Zombie.cpp \
../run.cpp 

OBJS += \
./Bullet.o \
./Entity.o \
./GameMaster.o \
./Matrix.o \
./NeuralNet.o \
./Player.o \
./Zombie.o \
./run.o 

CPP_DEPS += \
./Bullet.d \
./Entity.d \
./GameMaster.d \
./Matrix.d \
./NeuralNet.d \
./Player.d \
./Zombie.d \
./run.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++1y -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


