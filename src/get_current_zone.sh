#/bin/bash
ls -l /etc/localtime > zone
sed -i 's/^.*zoneinfo//' zone
sed -i 's/\///' zone
cat zone
