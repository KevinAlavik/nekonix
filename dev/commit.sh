#!/bin/bash
if [ -z "$1" ]; then
  echo "Error: Commit message is required as the first argument."
  exit 1
fi

git commit -m "$1"

commit_hash=$(git rev-parse --short HEAD)

make

scp Nekonix.iso root@uwubox:/srv/ftp/public/nekonix/dist/Nekonix-${commit_hash}.iso

echo "Uploaded Nekonix.iso as Nekonix-${commit_hash}.iso"
