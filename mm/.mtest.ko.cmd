cmd_/root/kernel-hacking/mm/mtest.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o /root/kernel-hacking/mm/mtest.ko /root/kernel-hacking/mm/mtest.o /root/kernel-hacking/mm/mtest.mod.o ;  true
