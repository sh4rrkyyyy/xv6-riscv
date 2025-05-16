
#!/usr/bin/env bash
set -euo pipefail

IMG="ext2.img"
MNT="./mnt"
UTIL="./main"
BLOCKSIZE=1024
IMG_SIZE="200M"

if [[ ! -x "$UTIL" ]]; then
  echo "Error: utility '$UTIL' not found or not executable."
  exit 1
fi

truncate -s "$IMG_SIZE" "$IMG"
echo "y" | mkfs.ext2 -b "$BLOCKSIZE" "$IMG"

mkdir -p "$MNT"
if mountpoint -q "$MNT"; then
  echo "Mount point $MNT is already mounted; unmounting."
  sudo umount "$MNT"
fi

sudo mount -o loop -t ext2 "$IMG" "$MNT"
sudo chown "$(id -u):$(id -g)" "$MNT"

mkdir "$MNT/dir1" "$MNT/dir2" "$MNT/dir3"
echo "Hello, world!" > "$MNT/file_small.txt"
truncate -s 1M  "$MNT/file_single_indirect.bin"
truncate -s 20M "$MNT/file_double_indirect.bin"

declare -A INODES CHECKSUMS
for f in file_small.txt file_single_indirect.bin file_double_indirect.bin; do
  inode=$(stat -c '%i' "$MNT/$f")
  checksum=$(sha512sum "$MNT/$f" | awk '{print $1}')
  INODES[$f]=$inode
  CHECKSUMS[$f]=$checksum
done

sudo umount "$MNT"

echo
echo "Testing direct reading from image '$IMG'"
for f in "${!INODES[@]}"; do
  inode=${INODES[$f]}
  expected=${CHECKSUMS[$f]}
  out=$(mktemp)
  "$UTIL" "$IMG" "$inode" > "$out"
  actual=$(sha512sum "$out" | awk '{print $1}')
  rm -f "$out"
  if [[ "$actual" != "$expected" ]]; then
    echo "FAIL: $f (inode $inode): expected $expected, got $actual"
    exit 1
  else
    echo " OK: $f (inode $inode)"
  fi
done

echo
echo "Testing reading via loop device"
LOOP=$(sudo losetup -f --show "$IMG")
trap 'sudo losetup -d "$LOOP"' EXIT

echo "Loop device info:"
lsblk -o NAME,SIZE,FSTYPE "$LOOP"

for f in "${!INODES[@]}"; do
  inode=${INODES[$f]}
  expected=${CHECKSUMS[$f]}
  out=$(mktemp)
  sudo "$UTIL" "$LOOP" "$inode" | cat > "$out"
  actual=$(sha512sum "$out" | awk '{print $1}')
  rm -f "$out"
  if [[ "$actual" != "$expected" ]]; then
    echo "FAIL (loop): $f (inode $inode): expected $expected, got $actual"
    exit 1
  else
    echo " OK (loop): $f (inode $inode)"
  fi
done

echo
echo "All tests passed successfully"