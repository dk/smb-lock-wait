smb_lock_wait: smb_lock_wait.c
	cc -D_GNU_SOURCE -Wall -Werror -o smb_lock_wait smb_lock_wait.c

local: smb_lock_wait
	sudo chown root ./smb_lock_wait
	sudo chmod u+s ./smb_lock_wait

install: smb_lock_wait
	cp smb_lock_wait /usr/bin
	chown root:root /usr/bin/smb_lock_wait
	chmod u+s /usr/bin/smb_lock_wait
