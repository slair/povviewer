#!/bin/sh

set -e

povdump_dir="povdump-3.7.0.0"
org_dir="povray-3.7.0.0"
src_bundle="v3.7.0.0.tar.gz"
POVRAY_PREFIX="/usr"
POVRAY_EXTRA_FLAGS="--sysconfdir=/etc"
POVRAY_COMPILED_BY="slair <s9lair@mail.ru>"
POVRAY_URL="https://github.com/POV-Ray/povray/archive/$src_bundle"

set +e
msg() {
	hrmsg=$(which hr-msg 2>/dev/null)
	if [ -z ${hrmsg} ]; then
		echo -e "${@}"
	else
		hr-msg " ${@} "
	fi
}
set -e
