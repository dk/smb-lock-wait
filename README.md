Wait for samba file to become available
=======================================

This little utility covers for a single use case, when your users upload files
to your samba share, and the unix daemon does things with the uploaded file.

The problem is that there's no easy way to tell that Samba finished copying the
file, so programmers doing the task boldly burned their fingers by snatching an
incomplete file.

The reason that it is an unitity, not an API, is that only root can issue
kernel calls required to interoperate with Samba. Therefore this utility must
be installed as root with setuid bit (yes, I know, random C code from internet
with suid bit :)

Also, add this

```
   [global]
      kernel oplocks = yes
```

to your smb.conf and you're all set. A simple daemon using the utility would look like
this:

```
  while (1) {
	sleep(1);
	opendir D, "a";
	for (readdir D) {
		next unless -f "a/$_";
		if ( system "/usr/bin/smb_lock_wait -v a/$_") {
			warn "unable to lock $_\n";
			next;
		}
		... do stuff ...
		unlink "a/$_";
		print "$_ ok\n";
	}
	closedir D;
  }
```

Enjoy!
dmitry@karasik.eu.org 
