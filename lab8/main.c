#include <endian.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define SB_OFFSET 1024

typedef struct {
  uint32_t s_inodes_count;
  uint32_t s_blocks_count;
  uint32_t s_r_blocks_count;
  uint32_t s_free_blocks_count;
  uint32_t s_free_inodes_count;
  uint32_t s_first_data_block;
  uint32_t s_log_block_size;
  uint32_t s_log_frag_size;
  uint32_t s_blocks_per_group;
  uint32_t s_frags_per_group;
  uint32_t s_inodes_per_group;
  uint32_t s_mtime;
  uint32_t s_wtime;
  uint16_t s_mnt_count;
  uint16_t s_max_mnt_count;
  uint16_t s_magic;
  uint16_t s_state;
  uint16_t s_errors;
  uint16_t s_minor_rev_level;
  uint32_t s_lastcheck;
  uint32_t s_checkinterval;
  uint32_t s_creator_os;
  uint32_t s_rev_level;
  uint16_t s_def_resuid;
  uint16_t s_def_resgid;
  uint32_t s_first_ino;
  uint16_t s_inode_size;
  uint16_t s_block_group_nr;
  uint32_t s_feature_compat;
  uint32_t s_feature_incompat;
  uint32_t s_feature_ro_compat;
  uint8_t s_uuid[16];
  char s_volume_name[16];
  char s_last_mounted[64];
  uint32_t s_algorithm_usage_bitmap;
} ext2_super_block_t;

typedef struct {
  uint32_t bg_block_bitmap;
  uint32_t bg_inode_bitmap;
  uint32_t bg_inode_table;
  uint16_t bg_free_blocks_count;
  uint16_t bg_free_inodes_count;
  uint16_t bg_used_dirs_count;
  uint16_t bg_pad;
  uint8_t bg_reserved[12];
} ext2_group_desc_t;

typedef struct {
  uint16_t i_mode;
  uint16_t i_uid;
  uint32_t i_size;
  uint32_t i_atime;
  uint32_t i_ctime;
  uint32_t i_mtime;
  uint32_t i_dtime;
  uint16_t i_gid;
  uint16_t i_links_count;
  uint32_t i_blocks;
  uint32_t i_flags;
  uint32_t i_osd1;
  uint32_t i_block[15];
  uint32_t i_generation;
  uint32_t i_file_acl;
  uint32_t i_dir_acl;
  uint32_t i_faddr;
  uint8_t i_osd2[12];
} ext2_inode_t;

int read_block(FILE *f, char *buf, uint32_t block_size, uint32_t block,
               uint64_t *rmd) {
  size_t w = *rmd > block_size ? block_size : *rmd;
  if (!block) {
    memset(buf, 0, w);
  } else {
    if (fseek(f, block * block_size, SEEK_SET) != 0) {
      return -1;
    }
    if (fread(buf, 1, w, f) != w) {
      return -1;
    }
  }
  if (fwrite(buf, 1, w, stdout) != w) {
    return -1;
  }
  *rmd -= w;
  return 0;
}

static int dump_blocks(FILE *f, char *buf, uint32_t block_size, uint32_t blk,
                       int level, uint64_t *rmd) {
  if (*rmd == 0) {
    return 0;
  }
  if (level == 0) {
    return read_block(f, buf, block_size, blk, rmd);
  }
  if (blk == 0) {
    return 0;
  }
  uint32_t cnt = block_size / sizeof(uint32_t);
  uint32_t *inds = malloc(block_size);
  if (!inds) {
    return -1;
  }
  if (fseek(f, (off_t)blk * block_size, SEEK_SET) != 0 ||
      fread(inds, sizeof(uint32_t), cnt, f) != cnt) {
    free(inds);
    return -1;
  }
  for (uint32_t i = 0; i<cnt && * rmd> 0; i++) {
    uint32_t ptr = le32toh(inds[i]);
    if (ptr) {
      if (dump_blocks(f, buf, block_size, ptr, level - 1, rmd) < 0) {
        free(inds);
        return -1;
      }
    }
  }
  free(inds);
  return 0;
}
int main(int argc, char **argv) {
  if (argc != 3) {
    fprintf(stderr, "Not 3 args\n");
    return EXIT_FAILURE;
  }
  FILE *f = fopen(argv[1], "rb");
  if (!f) {
    perror("fopen");
    return EXIT_FAILURE;
  }
  uint32_t inode_num = atoi(argv[2]);
  if (fseek(f, SB_OFFSET, SEEK_SET) != 0) {
    perror("fseek sb");
    fclose(f);
    return EXIT_FAILURE;
  }
  ext2_super_block_t sb;
  if (fread(&sb, sizeof sb, 1, f) != 1) {
    perror("fread sb");
    fclose(f);
    return EXIT_FAILURE;
  }
  uint32_t block_size = 1024 << le32toh(sb.s_log_block_size);
  uint32_t inodes_per_group = le32toh(sb.s_inodes_per_group);
  uint16_t inode_size = le32toh(sb.s_inode_size);
  if (inode_size == 0) {
    inode_size = 128;
  }
  uint32_t group = (inode_num - 1) / inodes_per_group;
  uint32_t index = (inode_num - 1) % inodes_per_group;

  off_t gd_offset = (le32toh(sb.s_first_data_block) + 1) * block_size;
  if (fseek(f, gd_offset + group * sizeof(ext2_group_desc_t), SEEK_SET) != 0) {
    perror("fseek gd");
    fclose(f);
    return EXIT_FAILURE;
  }
  ext2_group_desc_t gd;
  if (fread(&gd, sizeof gd, 1, f) != 1) {
    perror("fread gd");
    fclose(f);
    return EXIT_FAILURE;
  }
  uint32_t inode_table = le32toh(gd.bg_inode_table);

  off_t inode_offset = inode_table * block_size + index * inode_size;
  if (fseek(f, inode_offset, SEEK_SET) != 0) {
    perror("fseek inode");
    fclose(f);
    return EXIT_FAILURE;
  }
  ext2_inode_t inode;
  if (fread(&inode, sizeof inode, 1, f) != 1) {
    perror("fread inode");
    fclose(f);
    return EXIT_FAILURE;
  }

  uint64_t sz = (uint64_t)le32toh(inode.i_size) |
                ((uint64_t)le32toh(inode.i_dir_acl) << 32);
  char *buf = (char *)malloc(block_size);
  if (!buf) {
    perror("malloc");
    fclose(f);
    return EXIT_FAILURE;
  }
  for (int i = 0; i < 12 && sz; ++i) {
    if (dump_blocks(f, buf, block_size, le32toh(inode.i_block[i]), 0, &sz) <
        0) {
      perror("dump direct");
      free(buf);
      fclose(f);
      return EXIT_FAILURE;
    }
  }
  for (int i = 1; i <= 3; ++i) {
    if (dump_blocks(f, buf, block_size, le32toh(inode.i_block[11 + i]),
                    i, &sz) < 0) {
      perror("dump indirect");
      free(buf);
      fclose(f);
      return EXIT_FAILURE;
    }
  }
  while (sz > 0) {
    size_t w = sz < block_size ? sz : block_size;
    memset(buf, 0, w);
    if (fwrite(buf, 1, w, stdout) != w) {
      perror("fwrite");
      free(buf);
      fclose(f);
      return EXIT_FAILURE;
    }
    sz -= w;
  }

  free(buf);
  fclose(f);
}
