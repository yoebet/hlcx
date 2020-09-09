ftp -n `../bmip` <<FF
user hlcx 5534
cd /usr/hlcx/adm2/backup
lcd /usr/hlcx/adm2/backup
put hlcxdb-$1.gz
bye
FF>>
