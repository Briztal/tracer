
PROJECT_DIR = src/projects/test

project:

#Compile the main source file;
	@$(_CC) -o build/apps/main.o -c $(PROJECT_DIR)/main.c


project_f = main.o

APPS_RULES += project


pwm_args := -DMODULE_NAME=pwm -DNB_CHANNELS=4

pwm_n_args = $(pwm_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DPERIOD=$(3) -DTOLERANCE=$(4)

pwm_0_args := $(call pwm_n_args,0,pwm_0,20000,10)
pwm_1_args := $(call pwm_n_args,1,pwm_1,20000,10)
pwm_2_args := $(call pwm_n_args,2,pwm_2,20000,10)
pwm_3_args := $(call pwm_n_args,3,pwm_3,20000,10)

pwm:

	@$(KCC) $(pwm_0_args) -o build/mods/pwm_0.o -c src/modules/pwm/pwm_ch.c
	@$(KCC) $(pwm_1_args) -o build/mods/pwm_1.o -c src/modules/pwm/pwm_ch.c
	@$(KCC) $(pwm_2_args) -o build/mods/pwm_2.o -c src/modules/pwm/pwm_ch.c
	@$(KCC) $(pwm_3_args) -o build/mods/pwm_3.o -c src/modules/pwm/pwm_ch.c

	@$(KCC) $(pwm_n_args) -o build/mods/pwm.o -c src/modules/pwm/pwm.c


pwm_f = pwm_0.o pwm_1.o pwm_2.o pwm_3.o

#Add the servo rule to the module dependencies list;
MODS_RULES += pwm
