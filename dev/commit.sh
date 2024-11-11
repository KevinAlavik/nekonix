#!/bin/bash

if [ -z "$1" ]; then
  echo "Error: Commit message is required as the first argument."
  exit 1
fi

git add .
git commit -m "$1"

commit_hash=$(git rev-parse --short HEAD)

commit_dir="dist/${commit_hash}"
mkdir -p "$commit_dir"

echo "$1" > "${commit_dir}/DESCRIPTION"
git format-patch -1 HEAD --stdout > "${commit_dir}/commit.patch"

make

mv Nekonix.iso "${commit_dir}/Nekonix-${commit_hash}.iso"

scp -r "$commit_dir" root@192.168.100.119:/srv/ftp/public/nekonix/dist/

rm -rf "$commit_dir"

echo "Uploaded ${commit_dir} as Nekonix-${commit_hash}.iso"
