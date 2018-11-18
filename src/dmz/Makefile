
DMZ_F = src/dmz

DMZ_ST = build/stdl

DMZ_KR = build/krnl


#------------------------------------------- stdlib dmz -------------------------------------------

stdlib_dmz :

	@$(_CC) -o $(DMZ_ST)/cmd.o 		-c $(DMZ_F)/if/cmd.c
	@$(_CC) -o $(DMZ_ST)/gpio.o 	-c $(DMZ_F)/if/gpio.c
	@$(_CC) -o $(DMZ_ST)/iface.o 	-c $(DMZ_F)/if/iface.c
	@$(_CC) -o $(DMZ_ST)/timer.o 	-c $(DMZ_F)/if/timer.c

	@$(_CC) -o $(DMZ_ST)/fstring.o 	-c $(DMZ_F)/fstring.c
	@$(_CC) -o $(DMZ_ST)/syscall.o 	-c $(DMZ_F)/syscall.c
	@$(_CC) -o $(DMZ_ST)/stdmem.o 	-c $(DMZ_F)/stdmem.c
	@$(_CC) -o $(DMZ_ST)/list.o 	-c $(DMZ_F)/list.c
	@$(_CC) -o $(DMZ_ST)/string.o 	-c $(DMZ_F)/string.c

STDL_RULES += stdlib_dmz


#------------------------------------------- kernel dmz -------------------------------------------

krnl_dmz :

	@$(_CC) -o $(DMZ_KR)/cmd.o 		-c $(DMZ_F)/if/cmd.c
	@$(_CC) -o $(DMZ_KR)/gpio.o 	-c $(DMZ_F)/if/gpio.c
	@$(_CC) -o $(DMZ_KR)/iface.o 	-c $(DMZ_F)/if/iface.c
	@$(_CC) -o $(DMZ_KR)/timer.o 	-c $(DMZ_F)/if/timer.c

	@$(_CC) -o $(DMZ_KR)/fstring.o 	-c $(DMZ_F)/fstring.c
	@$(_CC) -o $(DMZ_KR)/syscall.o 	-c $(DMZ_F)/syscall.c
	@$(_CC) -o $(DMZ_KR)/stdmem.o 	-c $(DMZ_F)/stdmem.c
	@$(_CC) -o $(DMZ_KR)/list.o 	-c $(DMZ_F)/list.c
	@$(_CC) -o $(DMZ_KR)/string.o 	-c $(DMZ_F)/string.c


KRNL_RULES += krnl_dmz

