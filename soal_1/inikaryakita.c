#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

static const char *dir_path = "/home/fio/portofolio";

// Function to reverse the content of a string
void reverse_content(char *content) {
    int len = strlen(content);
    for (int i = 0; i < len / 2; i++) {
        char temp = content[i];
        content[i] = content[len - i - 1];
        content[len - i - 1] = temp;
    }
}

// Get file attributes
static int porto_getattr(const char *path, struct stat *stbuf) {
    int res;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
    res = lstat(full_path, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

// Read the content of a file
static int porto_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);

    (void) fi;
    fd = open(full_path, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    // Reverse the content if the file has "test" prefix
    if (strncmp(path, "/bahaya/test", 12) == 0) {
        reverse_content(buf);
    }

    return res;
}

// Change the permission of script.sh to make it executable
static int porto_open(const char *path, struct fuse_file_info *fi) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);

    // Change permission of script.sh
    if (strcmp(path, "/bahaya/script.sh") == 0) {
        chmod(full_path, 0755);
    }

    int res = open(full_path, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

// Read directory contents
static int porto_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char full_path[1024];

    snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
    dp = opendir(full_path);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

void add_watermark(const char *image_path) {
    char command[1024];
    snprintf(command, sizeof(command), "convert %s -pointsize 36 -fill white -gravity South -annotate +0+10 'inikaryakita.id' %s", image_path, image_path);
    system(command);
}



static int porto_mkdir(const char *path, mode_t mode) {
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);

    int res = mkdir(full_path, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int porto_rename(const char *from, const char *to) {
    char full_path_from[1024];
    char full_path_to[1024];
    snprintf(full_path_from, sizeof(full_path_from), "%s%s", dir_path, from);
    snprintf(full_path_to, sizeof(full_path_to), "%s%s", dir_path, to);

    int res = rename(full_path_from, full_path_to);
    if (res == -1)
        return -errno;

    if (strstr(to, "/wm/") != NULL) {
        add_watermark(full_path_to);
    }

    return 0;
}


static struct fuse_operations reversefs_oper = {
    .getattr = porto_getattr,
    .read = porto_read,
    .open = porto_open,
    .readdir = porto_readdir,
    .mkdir = porto_mkdir,
    .rename = porto_rename, 
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &reversefs_oper, NULL);
}

