#!/bin/bash

USAGE="./init-new-report.sh <WS-number>"

if [[ $# -ne 1 ]]; then
	echo "You need to pass the worksheet number as only argument!"
	echo -e ${USAGE}
	exit 1
fi
if [[ ! $1 =~ ^[0-9]+$ ]]; then
	echo "Argument \"$1\" is not a number!"
	echo -e ${USAGE}
	exit 1
fi

wsNum=$1
RDIR="Report_WS${wsNum}"
reportName="report_WS${wsNum}"

echo "Copying Template to ${RDIR}..."
cp -r Template ${RDIR}

pushd ${RDIR}

echo "Renaming LaTeX source file to ${reportName}.tex"
mv report_template.tex ${reportName}.tex

echo "Updating target name in Makefile..."
sed -i s/"report_template"/"${reportName}"/ Makefile

echo "Updating worksheet number in report title..."
sed -i s/"<WSnum>"/"${wsNum}"/ ${reportName}.tex

popd

echo "Done! :)"
exit 0

#eof
