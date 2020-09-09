ftp `../bmip` <<FF
user hlcx
cd /usr/hlcx/adm2/backup
lcd /usr/hlcx/adm2/backup
put hlcxdb-$1.gz
bye
FF>>
