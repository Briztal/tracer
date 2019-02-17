

#servo_args := -DMODULE_NAME=servo -DNB_CHANNELS=1 -DPERIOD=20000 -DTOLERANCE=10 -DTIMER_NAME=pit_0
#
#servo_cx_args = $(servo_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DGPIO_NAME=$(3) -DMAX_DURATION=$(4)
#
#servo_c0_args := $(call servo_cx_args,0,servo_0,led,20000)
#
#servo:
#
#	@mkdir -p build/mod/servo
#
#	@$(KCC)$(servo_c0_args) -o build/mod/servo_channel0.o -c src/modules/servo_channel.c
#
#	@$(KCC)$(servo_args) -o build/mod/servo.o -c src/modules/servo.c
#

#Add the servo rule to the module dependencies list;
#MODULES += servo

#Add the servo object to the modules list;
##MODULES_OBJS += build/mod/servo.o  build/mod/servo_channel0.o




#multi_pwm_args := -DMODULE_NAME=pwm -DNB_CHANNELS=1 -DTOLERANCE=10 -DTIMER_NAME=pit_1
#
#multi_pwm_n_args = $(pwm_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DGPIO_NAME=$(3) -DPERIOD=$(4)
#
#multi_pwm_0_args := $(call multi_pwm_n_args,0,pwm_0,led,20000)
#
#multi_pwm:
#
#	@mkdir -p build/mod
#
#	@$(KCC)$(multi_pwm_0_args) -o build/mod/multi_pwm_0.o -c src/modules/multi_pwm_n.c
#
#	@$(KCC)$(multi_pwm_args) -o build/mod/multi_pwm.o -c src/modules/multi_pwm.c
#
#
#Add the servo rule to the module dependencies list;
#MODULES += multi_pwm

#Add the servo object to the modules list;
#MODULES_OBJS += build/mod/multi_pwm.o  build/mod/multi_pwm_0.o




#pwm_args := -DMODULE_NAME=pwm -DNB_CHANNELS=4
#
#pwm_n_args = $(pwm_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DTIMER_NAME=$(3) -DGPIO_NAME=$(4) -DGPIO_MUX=$(5) -DPERIOD=$(6) -DTOLERANCE=$(7)
#
#pwm_0_args := $(call pwm_n_args,0,pwm_0,pit_0,0,1,20000,10)
#pwm_1_args := $(call pwm_n_args,1,pwm_1,pit_1,1,1,20000,10)
#pwm_2_args := $(call pwm_n_args,2,pwm_2,pit_2,2,1,20000,10)
#pwm_3_args := $(call pwm_n_args,3,pwm_3,pit_3,3,1,20000,10)
#
#pwm:
#
#	@mkdir -p build/mod/pwm
#
#	@$(KCC) $(pwm_0_args) -o build/mods/pwm_0.o -c src/modules/pwm_n.c
#	@$(KCC) $(pwm_1_args) -o build/mods/pwm_1.o -c src/modules/pwm_n.c
#	@$(KCC) $(pwm_2_args) -o build/mods/pwm_2.o -c src/modules/pwm_n.c
#	@$(KCC) $(pwm_3_args) -o build/mods/pwm_3.o -c src/modules/pwm_n.c
#
#	@$(KCC) $(pwm_n_args) -o build/mods/pwm.o -c src/modules/pwm.c
#
#
##Add the servo rule to the module dependencies list;
#MODS_RULES += pwm
#
#