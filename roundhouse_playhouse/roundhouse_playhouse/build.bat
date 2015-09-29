set path=C:\Users\Marek\Desktop\gbadevkit\devkitadv\bin;%path%

gcc -c -O3 -mthumb -mthumb-interwork gfx.c
gcc -c -O3 -mthumb -mthumb-interwork main.c
gcc -mthumb -mthumb-interwork -o main.elf main.o gfx.o

objcopy -O binary main.elf roundhouseplayhouse.gba
pause