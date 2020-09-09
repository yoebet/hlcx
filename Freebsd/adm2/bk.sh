mysqldump -uhlcx -p$1 hlcxdb --add-drop-table | gzip -c - > ./backup/hlcxdb-$2.gz
