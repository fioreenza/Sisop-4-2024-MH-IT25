#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>

#define MAX_CONTENT_SIZE 1024
#define CHUNK_SIZE 10240

struct file_node {
    char name[256];
    char content[1024];
    size_t size;
    struct file_node *next;
};

static struct file_node *file_list = NULL;
static const char *dir_path = "/home/ludwigd/SisOP/Praktikum4/nomor_3/relics";

static struct file_node* find_file(const char *path) {
    struct file_node *current = file_list;
    while (current) {
        if (strcmp(path + 1, current->name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void split_file(const char *filename, const char *content, size_t size) {
    char file_path[1024];
    int part_count = 0;
    size_t offset = 0;

    while (offset < size) {
        snprintf(file_path, sizeof(file_path), "%s/%s.%03d", dir_path, filename, part_count);
        FILE *file = fopen(file_path, "wb");
        if (!file) {
            fprintf(stderr, "Error creating file %s\n", file_path);
            return;
        }

        size_t bytes_to_write = size - offset;
        if (bytes_to_write > 10240) {
            bytes_to_write = 10240;
        }

        fwrite(content + offset, 1, bytes_to_write, file);
        fclose(file);

        offset += bytes_to_write;
        part_count++;
    }
}

static int do_getattr(const char *path, struct stat *st) {
    int res = 0;
    memset(st, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
    } else {
        struct file_node *file = find_file(path);
        if (file) {
            st->st_mode = S_IFREG | 0644;
            st->st_nlink = 1;
            st->st_size = file->size;
        } else {
            char file_path[1024];
            int file_number;
            if (sscanf(path, "/relic_%d.png", &file_number) == 1) {
                st->st_mode = S_IFREG | 0444;
                st->st_nlink = 1;
                st->st_size = 0;
                for (int i = 0;; i++) {
                    snprintf(file_path, sizeof(file_path), "%s/relic_%d.png.%03d", dir_path, file_number, i);
                    struct stat fragment_stat;
                    if (stat(file_path, &fragment_stat) != 0) break;
                    st->st_size += fragment_stat.st_size;
                }
            } else {
                res = -ENOENT;
            }
        }
    }

    return res;
}

static int do_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    if (strcmp(path, "/") != 0) {
        return -ENOENT;
    }

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    struct file_node *current = file_list;
    while (current) {
        filler(buf, current->name, NULL, 0);
        current = current->next;
    }

    DIR *dp;
    struct dirent *de;

    dp = opendir(dir_path);
    if (dp == NULL)
        return -errno;

    int seen_files[1024] = {0};

    while ((de = readdir(dp)) != NULL) {
        int file_number;
        if (sscanf(de->d_name, "relic_%d.png", &file_number) == 1) {
            if (!seen_files[file_number]) {
                seen_files[file_number] = 1;
                char file_name[256];
                snprintf(file_name, sizeof(file_name), "relic_%d.png", file_number);
                filler(buf, file_name, NULL, 0);
            }
        }
    }

    closedir(dp);
    return 0;
}

static int do_open(const char *path, struct fuse_file_info *fi) {
    struct file_node *file = find_file(path);
    if (!file) {
        int file_number;
        if (sscanf(path, "/relic_%d.png", &file_number) == 1) {
            return 0;
        }
        return -ENOENT;
    }

    return 0;
}

static int do_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    struct file_node *file = find_file(path);
    if (!file) {
        int file_number;
        if (sscanf(path, "/relic_%d.png", &file_number) != 1) {
            return -ENOENT;
        }

        char file_path[1024];
        size_t total_read = 0;
        size_t total_size = 0;

        for (int i = 0;; i++) {
            snprintf(file_path, sizeof(file_path), "%s/relic_%d.png.%03d", dir_path, file_number, i);
            int fd = open(file_path, O_RDONLY);
            if (fd == -1) break;

            struct stat st;
            stat(file_path, &st);

            if (offset < total_size + st.st_size) {
                size_t to_read = size;
                if (offset > total_size) {
                    lseek(fd, offset - total_size, SEEK_SET);
                    to_read = st.st_size - (offset - total_size);
                }
                if (to_read > size - total_read) {
                    to_read = size - total_read;
                }
                ssize_t res = read(fd, buf + total_read, to_read);
                if (res == -1) {
                    close(fd);
                    return -errno;
                }
                total_read += res;
                if (total_read == size) {
                    close(fd);
                    break;
                }
            }
            total_size += st.st_size;
            close(fd);
        }

        return total_read;
    }

    if (offset < file->size) {
        if (offset + size > file->size) {
            size = file->size - offset;
        }
        memcpy(buf, file->content + offset, size);
    } else {
        size = 0;
    }

    return size;
}

static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    struct file_node *new_file = malloc(sizeof(struct file_node));
    strcpy(new_file->name, path + 1);
    new_file->size = 0;
    new_file->content[0] = '\0';
    new_file->next = file_list;
    file_list = new_file;

    char file_path[1024];
    snprintf(file_path, sizeof(file_path), "%s%s", dir_path, path);

    int fd = open(file_path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (fd == -1) {
        return -errno;
    }
    close(fd);

    return 0;
}

static int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    struct file_node *file = find_file(path);
    if (!file) {
        return -ENOENT;
    }

    size_t written = 0;
    while (written < size) {
        char chunk_path[1024];
        size_t chunk_num = (offset + written) / CHUNK_SIZE;
        size_t chunk_offset = (offset + written) % CHUNK_SIZE;
        size_t to_write = CHUNK_SIZE - chunk_offset;
        if (to_write > size - written) {
            to_write = size - written;
        }

        snprintf(chunk_path, sizeof(chunk_path), "%s/%s.%03zu", dir_path, path + 1, chunk_num);

        int fd = open(chunk_path, O_WRONLY | O_CREAT, 0644);
        if (fd == -1) {
            return -errno;
        }

        if (pwrite(fd, buf + written, to_write, chunk_offset) == -1) {
            close(fd);
            return -errno;
        }
        close(fd);
        written += to_write;
    }

    if (offset + size > file->size) {
        file->size = offset + size;
    }

    return size;
}

static int do_truncate(const char *path, off_t size) {
    struct file_node *file = find_file(path);
    if (!file) {
        return -ENOENT;
    }

    char file_path[1024];
    size_t chunks = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;

    for (size_t i = 0; i < chunks; i++) {
        snprintf(file_path, sizeof(file_path), "%s/%s.%03zu", dir_path, path + 1, i);
        int fd = open(file_path, O_WRONLY | O_CREAT, 0644);
        if (fd == -1) {
            return -errno;
        }
        if (i == chunks - 1) {
            if (ftruncate(fd, size % CHUNK_SIZE) == -1) {
                close(fd);
                return -errno;
            }
        }
        close(fd);
    }

    while (1) {
        snprintf(file_path, sizeof(file_path), "%s/%s.%03zu", dir_path, path + 1, chunks);
        if (unlink(file_path) == -1) {
            if (errno == ENOENT) {
                break;
            }
            return -errno;
        }
        chunks++;
    }

    file->size = size;

    return 0;
}

static int do_copy(const char *src_path, const char *dst_path, int mode) {
    char src_file_path[1024];
    snprintf(src_file_path, sizeof(src_file_path), "%s%s", dir_path, src_path);

    char dst_file_path[1024];
    snprintf(dst_file_path, sizeof(dst_file_path), "%s%s", dir_path, dst_path);

    int src_fd = open(src_file_path, O_RDONLY);
    if (src_fd == -1) {
        return -errno;
    }

    int dst_fd = open(dst_file_path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    if (dst_fd == -1) {
        close(src_fd);
        return -errno;
    }

    char buffer[8192];
    ssize_t bytes_read;

    while ((bytes_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        if (write(dst_fd, buffer, bytes_read) != bytes_read) {
            close(src_fd);
            close(dst_fd);
            return -errno;
        }
    }

    close(src_fd);
    close(dst_fd);
    return 0;
}

static int do_copy_from_relic(const char *src_path, const char *dst_path) {
    return do_copy(src_path, dst_path, 0644); // Set the permission to read for everyone
}

static struct fuse_operations operations = {
    .getattr = do_getattr,
    .readdir = do_readdir,
    .open = do_open,
    .read = do_read,
    .create = do_create,
    .write = do_write,
    .truncate = do_truncate,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &operations, NULL);
}
