obj-m :=main.o
all :
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
do:
	sudo insmod main.ko
	sudo mkfs.vfat -F16 /dev/mydisk1 && \
 	sudo mkfs.vfat -F16 /dev/mydisk5 && \
	sudo mkfs.vfat -F16 /dev/mydisk6 && \
	sudo mkfs.vfat -F16 /dev/mydisk7
	sudo mount /dev/mydisk1 /mnt/mydisk1 && \
	sudo mount /dev/mydisk5 /mnt/mydisk5 && \
  	sudo mount /dev/mydisk6 /mnt/mydisk6 && \
  	sudo mount /dev/mydisk7 /mnt/mydisk7
rm:
	sudo umount /mnt/mydisk1 && sudo umount /mnt/mydisk5 && sudo umount /mnt/mydisk6 && sudo umount /mnt/mydisk7
	sudo rmmod main.ko
