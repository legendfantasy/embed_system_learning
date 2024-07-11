CGI_OBJS =  env1.cgi a9_beep.cgi a9_led.cgi a9_seg.cgi
OBJS=main.o pthread_web_client_handle.o pthread_led_control.o pthread_fan_control.o \
	linuxuart.o

CROSS_COMPILE=arm-none-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
#CC=gcc
CFLAGS= -c -g
AFLAGS= -Wall -g
#LDFLAGS= -lpthread -L ./lib -lsqlite3
LDFLAGS= -lpthread -L ./lib

proj_monitor : $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	mv *.o ./prog_monitor_obj
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

cgi_prog : $(CGI_OBJS)
	mv *.cgi ./prog_monitor_cgi
$(CGI_OBJS) : %.cgi : %.c
	$(CC) $(AFLAGS) $< cgic.c -o $@ $(LDFLAGS)

install_proj:
	sudo cp proj_monitor /opt/4412/rootfs
install_cgi: 
	sudo cp ./prog_monitor_cgi/*.cgi /opt/4412/rootfs/boa/cgi-bin


.PHONY:clean
clean:
	rm ./prog_monitor_obj/*.o proj_monitor ./prog_monitor_cgi/*.cgi -rf 
	rm proj_monitor
