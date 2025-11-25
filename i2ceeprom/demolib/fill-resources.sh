#!/bin/sh

basedir=`dirname -- "$( readlink -f -- "$0"; )";`
resourcefile="$basedir/clamps.qrc"

rm -f $resourcefile
echo '<RCC>' >> $resourcefile
echo '    <qresource prefix="/">' >> $resourcefile

cd $basedir
for eepromfile in `find 'clamps' -name '*.eeprom'`; do
    echo '        <file>'$eepromfile'</file>' >> $resourcefile
done

echo '    </qresource>' >> $resourcefile
echo '</RCC>' >> $resourcefile

