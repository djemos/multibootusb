#!/bin/sh
#
echo `locale | grep LANG= | sed 's/LANG=//'  | sed 's/UTF-8/utf8/' ` > system_locale
cat system_locale
