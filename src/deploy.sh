#!bin/bash
scp src/cat/s21_cat root@10.10.2.2:/usr/local/bin/
scp src/grep/s21_grep root@10.10.2.2:/usr/local/bin/

ssh root@10.10.2.2 ls -lah /usr/local/bin/