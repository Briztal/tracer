
MODULES_SRCS += modules/test.c

#MODULES_SRCS += modules/pwm.c




servo_args := -DMODULE_NAME=servo -DNB_CHANNELS=1 -DPERIOD=20000 -DTOLERANCE=10 -DTIMER_NAME=pit_0

servo_cx_args = $(servo_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DGPIO_NAME=$(3) -DMAX_DURATION=$(4)

servo_c0_args := $(call servo_cx_args,0,servo_0,led,20000)

servo:

	@mkdir -p build/mod/servo

	@$(CC) $(CFLAGS) $(INC) $(servo_c0_args) -o build/mod/servo/servo_channel0.o -c src/modules/servo_channel.c

	@$(CC) $(CFLAGS) $(INC) $(servo_args) -o build/mod/servo/servo.o -c src/modules/servo.c


#Add the servo rule to the module dependencies list;
#MODULES += servo

#Add the servo object to the modules list;
##MODULES_OBJS += build/mod/servo/servo.o  build/mod/servo/servo_channel0.o




pwm_args := -DMODULE_NAME=pwm -DNB_CHANNELS=1 -DTOLERANCE=10 -DTIMER_NAME=pit_1

pwm_cx_args = $(pwm_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DGPIO_NAME=$(3) -DPERIOD=$(4)

pwm_c0_args := $(call pwm_cx_args,0,pwm_0,led,20000)

pwm:

	@mkdir -p build/mod/pwm

	@$(CC) $(CFLAGS) $(INC) $(pwm_c0_args) -o build/mod/pwm/pwm_channel0.o -c src/modules/pwm_channel.c

	@$(CC) $(CFLAGS) $(INC) $(pwm_args) -o build/mod/pwm/pwm.o -c src/modules/pwm.c


#Add the servo rule to the module dependencies list;
MODULES += pwm

#Add the servo object to the modules list;
MODULES_OBJS += build/mod/pwm/pwm.o  build/mod/pwm/pwm_channel0.o


