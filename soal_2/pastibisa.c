#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

static char *decode_base64(const char *input) {
    BIO *bio, *b64;
    char *buffer = malloc(strlen(input) + 1);

    bio = BIO_new_mem_buf((void *)input, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_read(bio, buffer, strlen(input));
    BIO_free_all(bio);

    return buffer;
}

static char *decode_rot13(const char *input) {
    char *buffer = strdup(input);

    for (size_t i = 0; i < strlen(buffer); ++i) {
        if (isalpha(buffer[i])) {
            if (islower(buffer[i])) {
                buffer[i] = (buffer[i] - 'a' + 13) % 26 + 'a';
            } else {
                buffer[i] = (buffer[i] - 'A' + 13) % 26 + 'A';
            }
        }
    }

    return buffer;
}

static char *decode_hex(const char *input) {
    size_t len = strlen(input);
    char *buffer = malloc(len / 2 + 1);

    for (size_t i = 0, j = 0; i < len; i += 2, ++j) {
        sscanf(input + i, "%2hhx", &buffer[j]);
    }

    return buffer;
}

static char *reverse_text(const char *input) {
    size_t len = strlen(input);
    char *buffer = malloc(len + 1);

    for (size_t i = 0; i < len; ++i) {
        buffer[i] = input[len - i - 1];
    }
    buffer[len] = '\0';

    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <file_content>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    const char *file_content = argv[2];

    char *decoded_content = NULL;

    if (strstr(filename, "base64") != NULL) {
        decoded_content = decode_base64(file_content);
    } else if (strstr(filename, "rot13") != NULL) {
        decoded_content = decode_rot13(file_content);
    } else if (strstr(filename, "hex") != NULL) {
        decoded_content = decode_hex(file_content);
    } else if (strstr(filename, "rev") != NULL) {
        decoded_content = reverse_text(file_content);
    } else {
        fprintf(stderr, "Unsupported file type\n");
        return 1;
    }

    printf("Decoded content:\n%s\n", decoded_content);

    free(decoded_content);

    return 0;
}