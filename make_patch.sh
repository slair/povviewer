#!/bin/sh

. ./config.sh

if ! [ -d ${org_dir} ]; then
	msg "Extracting '${src_bundle}'"
	tar xf ${src_bundle}
fi

msg "Comparing '${org_dir}/' with '${povdump_dir}'"
diff -rupNb ${org_dir}/ ${povdump_dir}/ > povdump.patch && echo ok

cp -vf povdump.patch build
