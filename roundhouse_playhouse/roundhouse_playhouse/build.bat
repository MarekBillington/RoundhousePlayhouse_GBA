set path=C:\Users\Marek\Desktop\gbadevkit\devkitadv\bin;%path%
gcc -o main.elf main.c -lm
objcopy -O binary main.elf roundhouseplayhouse.gba
pause