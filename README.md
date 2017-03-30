All project based on [mbed-os](https://github.com/ARMmbed/mbed-os)

Pre-Request
===========
  * [mbed-cli](https://docs.mbed.com/docs/mbed-os-handbook/en/latest/dev_tools/cli/) tool used to compile this projects

How To Use
=========
```shell
user@toad:~$ git clone https://github.com/vinothgcdm/mbed.git
user@toad:~$ cd mbed
user@toad:~/mbed$ ls mbed-os  # This will empty
user@toad:~/mbed$ git submodule init
user@toad:~/mbed$ git submodule update  # Download all submpdules
user@toad:~/mbed$ mbed new .  # To turn the existing current directory into an mbed project
user@toad:~/mbed$ cd k64f-diag/  # Goto any project directory
user@toad:~/mbed/k64f-diag$ mbed compile -t GCC_ARM -m K64F --source . --source ../mbed-os/
...
...
Image: ../.build/K64F/GCC_ARM/k64f-diag.bin  # Final output
```