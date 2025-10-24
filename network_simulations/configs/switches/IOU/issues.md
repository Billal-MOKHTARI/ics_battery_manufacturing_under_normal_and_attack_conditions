Some L2 IOU Switches recquire an old computer architecture `i386` instead of `x86_64`. When it comes to start the appliance, an error will be shown indicating that the file `libcrypto.so.4` is missing. To resolve the problem, you should follow these steps:

1. Connect to the terminal as a root: `sudo su`.
2. Enable 32-bit architecture support (if not already enabled).
```bash
dpkg --add-architecture i386
apt update
```
3. Install `libssl` for `i386` hardware architecture: `apt-get install libssl-dev:i386`.
4. Locate the Installed 32-bit `libcrypto` Library.

```bash
file /usr/lib/i386-linux-gnu/libcrypto.so.*
file /lib/i386-linux-gnu/libcrypto.so.*
```

The output:

```plaintext
/usr/lib/i386-linux-gnu/libcrypto.so.3: ELF 32-bit LSB shared object, Intel 80386, version 1 (SYSV), dynamically linked, BuildID[sha1]=26085faa8053e75d70984c8bfb52f14dd6dc9ee2, stripped
/lib/i386-linux-gnu/libcrypto.so.3: ELF 32-bit LSB shared object, Intel 80386, version 1 (SYSV), dynamically linked, BuildID[sha1]=26085faa8053e75d70984c8bfb52f14dd6dc9ee2, stripped
```

- **ELF 32-bit LSB shared object** in the output means that it works well. The file path returned in the output `/usr/lib/i386-linux-gnu/libcrypto.so.3` will be needed in the next step.
5. Create a symbolic link and restart the configurations:
```bash
ln -s /usr/lib/i386-linux-gnu/libcrypto.so.3 /usr/lib/i386-linux-gnu/libcrypto.so.4
ln -s /usr/lib/i386-linux-gnu/libcrypto.so.3 /usr/lib/libcrypto.so.4
ldconfig
```

6. Verify that IOU can find `libcrypto.so.4`:
```bash
ldd /path/to/your/IOU_image
```

Example:
```bash
ldd /home/billal-mokhtari/GNS3/images/IOU/i86bi-linux-l2-adventerprisek9-15.1a.bin
```

7. Generate a licence key:
```bash
apt-get update
http://www.ipvanquish.com/download/CiscoIOUKeygen3f.py
chmod 777 CiscoIOUKeygen3f.py
python3 CiscoIOUKeygen3f.py
```

8. The `iourc.txt` will be generated on the same