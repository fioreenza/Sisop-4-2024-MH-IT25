# Laporan Resmi SisOp Modul 4 Kelompok IT25

**Anggota kelompok IT25**:
* Fiorenza Adelia Nalle (5027231053)
* Tio Axellino Irin (5027231065)
* Wira Samudra Siregar (5027231041)

# Deskripsi Praktikum Modul 4

Pada Praktikum Modul 4 ini, kami diberikan tugas untuk mengerjakan Soal Shift Modul 4. Berikut adalah pembagian pengerjaan Soal Shift Modul 4:
* Soal 1 yang dikerjakan oleh Fiorenza Adelia Nalle (5027231053) 
* Soal 2 yang dikerjakan oleh Wira Samudra Siregar (5027231041)
* Soal 3 yang dikerjakan oleh Tio Axellino Irin (5027231065) 

# Proses dan Hasil Praktikum Modul 4

## Soal 1

**oleh Fiorenza Adelia Nalle (5027231053)**

### Deskripsi Soal 1
Adfi merupakan seorang CEO agency creative bernama Ini Karya Kita. Ia sedang melakukan inovasi pada manajemen project photography Ini Karya Kita. Salah satu ide yang dia kembangkan adalah tentang pengelolaan foto project dalam sistem arsip Ini Karya Kita. Dalam membangun sistem ini, Adfi tidak bisa melakukannya sendirian, dia perlu bantuan mahasiswa Departemen Teknologi Informasi angkatan 2023 untuk membahas konsep baru yang akan mengubah project fotografinya lebih menarik untuk dilihat. Adfi telah menyiapkan portofolio hasil project fotonya yang bisa didownload dan diakses di www.inikaryakita.id . Silahkan eksplorasi web Ini Karya Kita dan temukan halaman untuk bisa mendownload projectnya. Setelah kalian download terdapat folder gallery dan bahaya.
* Pada folder "gallery":
  Membuat folder dengan prefix "wm." Dalam folder ini, setiap gambar yang dipindahkan ke dalamnya akan diberikan watermark bertuliskan inikaryakita.id.
  Ex: "mv ikk.jpeg wm-foto/"
  Output:
  Before: (tidak ada watermark bertuliskan inikaryakita.id)
  After: (terdapat watermark tulisan inikaryakita.id)
* Pada folder "bahaya":
  Terdapat file bernama "script.sh." Adfi menyadari pentingnya menjaga keamanan dan integritas data dalam folder ini. Mereka harus mengubah permission pada file "script.sh" agar bisa
  dijalankan, karena jika dijalankan maka dapat menghapus semua dan isi dari  "gallery". Adfi dan timnya juga ingin menambahkan fitur baru dengan membuat file dengan prefix "test"
  yang ketika disimpan akan mengalami pembalikan (reverse) isi dari file tersebut.  


### Penyelesaian Soal 1
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
    
    
    void reverse_content(char *content) {
        int len = strlen(content);
        for (int i = 0; i < len / 2; i++) {
            char temp = content[i];
            content[i] = content[len - i - 1];
            content[len - i - 1] = temp;
        }
    }
    
    
    static int porto_getattr(const char *path, struct stat *stbuf) {
        int res;
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
        res = lstat(full_path, stbuf);
        if (res == -1)
            return -errno;
        return 0;
    }
    
    
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
    
        if (strncmp(path, "/bahaya/test", 12) == 0) {
            reverse_content(buf);
        }
    
        return res;
    }
    
    
    static int porto_open(const char *path, struct fuse_file_info *fi) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
    
        if (strcmp(path, "/bahaya/script.sh") == 0) {
            chmod(full_path, 0755);
        }
    
        int res = open(full_path, fi->flags);
        if (res == -1)
            return -errno;
    
        close(res);
        return 0;
    }
    
    
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
    
        if (strstr(to, "/wm") != NULL) {
            add_watermark(full_path_to);
        }
    
        return 0;
    }
    
    
    static struct fuse_operations porto_oper = {
        .getattr = porto_getattr,
        .read = porto_read,
        .open = porto_open,
        .readdir = porto_readdir,
        .mkdir = porto_mkdir,
        .rename = porto_rename, 
    };
    
    
    int main(int argc, char *argv[]) {
        return fuse_main(argc, argv, &porto_oper, NULL);
    }

### Penjelasan Soal 1

    static const char *dir_path = "/home/fio/portofolio";
Mendefinisikan path dasar untuk file sistem yang akan di-mount. 

    void reverse_content(char *content) {
        int len = strlen(content);
        for (int i = 0; i < len / 2; i++) {
            char temp = content[i];
            content[i] = content[len - i - 1];
            content[len - i - 1] = temp;
        }
    }
Fungsi reverse_content membalikkan isi string yang diberikan. Fungsi ini menerima parameter char *content, menghitung panjang string dengan strlen(content), dan menggunakan loop untuk menukar karakter dari depan dan belakang hingga mencapai tengah string. Penukaran karakter dilakukan dengan menggunakan variabel sementara temp.

    static int porto_getattr(const char *path, struct stat *stbuf) {
        int res;
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
        res = lstat(full_path, stbuf);
        if (res == -1)
            return -errno;
        return 0;
    }
Fungsi porto_getattr digunakan untuk mendapatkan atribut file atau direktori. Fungsi ini menerima path dari file atau direktori, menggabungkannya dengan dir_path untuk membentuk path lengkap, dan kemudian memanggil fungsi lstat untuk mendapatkan atribut file tersebut dan menyimpannya dalam struktur stat. Jika lstat gagal, fungsi mengembalikan nilai kesalahan yang sesuai menggunakan -errno.

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
    
        if (strncmp(path, "/bahaya/test", 12) == 0) {
            reverse_content(buf);
        }
    
        return res;
    }
Fungsi porto_read bertanggung jawab untuk membaca isi file. Path file digabungkan dengan dir_path untuk mendapatkan path lengkap, dan file dibuka dalam mode read-only menggunakan open. Setelah file berhasil dibuka, fungsi pread digunakan untuk membaca sejumlah byte tertentu dari file mulai dari offset tertentu dan menyimpannya dalam buffer buf. Jika file yang dibaca adalah /bahaya/test, isi buffer kemudian dibalik menggunakan fungsi reverse_content. Akhirnya, file descriptor ditutup dan jumlah byte yang dibaca atau kesalahan dikembalikan.

    static int porto_open(const char *path, struct fuse_file_info *fi) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
    
        if (strcmp(path, "/bahaya/script.sh") == 0) {
            chmod(full_path, 0755);
        }
    
        int res = open(full_path, fi->flags);
        if (res == -1)
            return -errno;
    
        close(res);
        return 0;
    }
Fungsi porto_open digunakan untuk membuka file. Path file lengkap dibentuk dengan menggabungkan dir_path dan path. Jika file yang dibuka adalah /bahaya/script.sh, izin file diubah menjadi executable (0755) menggunakan chmod. File kemudian dibuka dengan flag yang ditentukan dalam struktur fuse_file_info, dan jika berhasil, file descriptor segera ditutup karena FUSE tidak memerlukan file tetap terbuka. Fungsi mengembalikan 0 jika berhasil atau nilai kesalahan jika gagal membuka file.

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
Fungsi porto_readdir digunakan untuk membaca isi direktori. Jalur direktori lengkap dibentuk dan direktori dibuka menggunakan opendir. Setiap entri dalam direktori dibaca menggunakan readdir, dan informasi tentang setiap entri diisi dalam struktur stat. Nama file dan atributnya kemudian ditambahkan ke buffer menggunakan fungsi filler. Setelah semua entri dibaca, direktori ditutup dan fungsi mengembalikan 0 jika berhasil atau nilai kesalahan jika gagal.

    void add_watermark(const char *image_path) {
        char command[1024];
        snprintf(command, sizeof(command), "convert %s -pointsize 36 -fill white -gravity South -annotate +0+10 'inikaryakita.id' %s", image_path, image_path);
        system(command);
    }
Fungsi add_watermark menambahkan watermark pada gambar menggunakan ImageMagick. Fungsi ini menerima path gambar sebagai parameter, lalu menjalankan ImageMagick menggunakan convert untuk menambahkan watermark 'inikaryakita.id' pada gambar, dan menjalankan perintah tersebut menggunakan system. Perintah convert menambahkan teks watermark pada bagian bawah gambar.

    static int porto_mkdir(const char *path, mode_t mode) {
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s%s", dir_path, path);
    
        int res = mkdir(full_path, mode);
        if (res == -1)
            return -errno;
    
        return 0;
    }
Fungsi porto_mkdir digunakan untuk membuat direktori baru. Path direktori lengkap dibentuk dengan menggabungkan dir_path dan path, dan direktori dibuat menggunakan fungsi mkdir dengan mode yang ditentukan. Jika berhasil, fungsi mengembalikan 0, dan jika gagal, fungsi mengembalikan nilai kesalahan yang sesuai.

    static int porto_rename(const char *from, const char *to) {
        char full_path_from[1024];
        char full_path_to[1024];
        snprintf(full_path_from, sizeof(full_path_from), "%s%s", dir_path, from);
        snprintf(full_path_to, sizeof(full_path_to), "%s%s", dir_path, to);
    
        int res = rename(full_path_from, full_path_to);
        if (res == -1)
            return -errno;
    
        if (strstr(to, "/wm") != NULL) {
            add_watermark(full_path_to);
        }
    
        return 0;
    }
Fungsi porto_rename bertanggung jawab untuk mengganti nama file atau direktori. Path lengkap untuk file/direktori sumber dan file/direktori tujuan dibentuk, dan fungsi rename digunakan untuk mengganti nama. Jika file atau direktori tujuan memiliki '/wm' dalam namanya, fungsi add_watermark dipanggil untuk menambahkan watermark pada file tersebut. Fungsi ini mengembalikan 0 jika berhasil atau nilai kesalahan jika gagal mengganti nama file atau direktori.

    static struct fuse_operations porto_oper = {
        .getattr = porto_getattr,
        .read = porto_read,
        .open = porto_open,
        .readdir = porto_readdir,
        .mkdir = porto_mkdir,
        .rename = porto_rename, 
    };
Struktur fuse_operations mendefinisikan operasi yang didukung oleh file system ini. Operasi yang didefinisikan meliputi getattr, read, open, readdir, mkdir, dan rename. Setiap operasi dihubungkan dengan fungsi yang sesuai yang telah diimplementasikan sebelumnya.

    int main(int argc, char *argv[]) {
        return fuse_main(argc, argv, &porto_oper, NULL);
    }
Fungsi main adalah titik masuk program yang menjalankan FUSE dengan operasi yang telah didefinisikan dalam porto_oper. Fungsi ini memanggil fuse_main dengan argumen yang diteruskan dari command line, dan mengembalikan hasil dari fuse_main. Fungsi ini menginisialisasi dan menjalankan file system FUSE dengan operasi yang telah didefinisikan, memungkinkan pengguna untuk berinteraksi dengan file system yang dibuat di user space.

### Screenshot Hasil Pengerjaan Soal 1

Memberikan watermark ketika gambar di-move/di-rename ke folder dengan prefix "wm"

<img width="713" alt="Screenshot 2024-05-25 at 22 38 20" src="https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/147926732/a751e95c-dabf-4a40-a13d-21a3d2133e14">
<img width="632" alt="Screenshot 2024-05-25 at 22 39 04" src="https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/147926732/26a6f6c8-b79b-47b3-b760-61b96500b123">

File script.sh bisa dijalankan 

<img width="717" alt="Screenshot 2024-05-25 at 22 40 03" src="https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/147926732/11e1b8d6-c5a3-4d41-8ec9-681ca71af355">

Reverse isi file yang memiliki nama dengan prefix "test"

<img width="727" alt="Screenshot 2024-05-25 at 22 39 43" src="https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/147926732/f436849d-6133-46c7-86db-d0353ecfa48f">


## Soal 2

**oleh Wira Samudra Siregar (5027231041)**

### Deskripsi Soal 2

Masih dengan Ini Karya Kita, sang CEO ingin melakukan tes keamanan pada folder sensitif Ini Karya Kita. Karena Teknologi Informasi merupakan departemen dengan salah satu fokus di Cyber Security, maka dia kembali meminta bantuan mahasiswa Teknologi Informasi angkatan 2023 untuk menguji dan mengatur keamanan pada folder sensitif tersebut. Untuk mendapatkan folder sensitif itu, mahasiswa IT 23 harus kembali mengunjungi website Ini Karya Kita pada www.inikaryakita.id/schedule . Silahkan isi semua formnya, tapi pada form subject isi dengan nama kelompok_SISOP24 , ex: IT01_SISOP24 . Lalu untuk form Masukkan Pesanmu, ketik “Mau Foldernya” . Tunggu hingga 1x24 jam, maka folder sensitif tersebut akan dikirimkan melalui email kalian. Apabila folder tidak dikirimkan ke email kalian, maka hubungi sang CEO untuk meminta bantuan.   
Pada folder "pesan" Adfi ingin meningkatkan kemampuan sistemnya dalam mengelola berkas-berkas teks dengan menggunakan fuse.
* Jika sebuah file memiliki prefix "base64," maka sistem akan langsung mendekode isi file tersebut dengan algoritma Base64.
* Jika sebuah file memiliki prefix "rot13," maka isi file tersebut akan langsung di-decode dengan algoritma ROT13.
* Jika sebuah file memiliki prefix "hex," maka isi file tersebut akan langsung di-decode dari representasi heksadesimalnya.
* Jika sebuah file memiliki prefix "rev," maka isi file tersebut akan langsung di-decode dengan cara membalikkan teksnya.
* Contoh:
  * File yang belum didecode/ dienkripsi rot_13:
    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/145766477/ffcd9cd2-8e9a-4e15-b4ee-6d803c2a09cc)

    File yang sudah didecode/ dienkripsi rot_13
    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/145766477/3df9b84b-dd29-438a-8ff9-1679d67cf8e8)

* Pada folder “rahasia-berkas”, Adfi dan timnya memutuskan untuk menerapkan kebijakan khusus. Mereka ingin memastikan bahwa folder dengan prefix "rahasia" tidak dapat diakses tanpa izin khusus. 
  * Jika seseorang ingin mengakses folder dan file pada “rahasia”, mereka harus memasukkan sebuah password terlebih dahulu (password bebas). 
* Setiap proses yang dilakukan akan tercatat pada logs-fuse.log dengan format :
  [SUCCESS/FAILED]::dd/mm/yyyy-hh:mm:ss::[tag]::[information]
  Ex:
  [SUCCESS]::01/11/2023-10:43:43::[moveFile]::[File moved successfully]

### Penyelesaian Soal 2

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

### Penjelasan Soal 2
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>
    #include <openssl/bio.h>
    #include <openssl/evp.h>
* Program ini menggunakan beberapa header file seperti `stdio.h` untuk operasi input/output, `string.h` untuk manipulasi string, `stdlib.h` untuk alokasi memori dan konversi tipe data, `ctype.h` untuk klasifikasi dan manipulasi karakter, serta `openssl/bio.h` dan `openssl/evp.h` dari library OpenSSL untuk operasi kriptografi.

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
* Fungsi `_decode_base64` adalah sebuah fungsi dalam program yang bertanggung jawab untuk melakukan dekode atau decoding terhadap string yang diberikan dalam format Base64. Fungsi ini menggunakan library OpenSSL untuk melakukan operasi dekode. `b64 = BIO_new(BIO_f_base64());` Baris ini membuat objek BIO baru menggunakan fungsi `BIO_new` dari OpenSSL dengan tipe `BIO_f_base64()` yang merupakan tipe BIO untuk melakukan dekode Base64.

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
* Fungsi `_decode_rot13` adalah sebuah fungsi dalam program yang bertanggung jawab untuk melakukan dekode terhadap string yang telah dienkode menggunakan algoritma ROT13. ROT13 adalah teknik enkripsi sederhana yang menggeser setiap huruf dalam alfabet sebanyak 13 posisi. Fungsi `islower` memeriksa apakah karakter saat ini adalah huruf kecil atau huruf besar, Jika karakter adalah huruf kecil, maka karakter tersebut digeser sebanyak 13 posisi dalam rentang 'a' hingga 'z' menggunakan operasi `(buffer[i] - 'a' + 13) % 26 + 'a';`, Jika karakter adalah huruf besar, maka menggunakan operasI `(buffer[i] - 'A' + 13) % 26 + 'A'`.

      static char *decode_hex(const char *input) {
      size_t len = strlen(input);
      char *buffer = malloc(len / 2 + 1);

      for (size_t i = 0, j = 0; i < len; i += 2, ++j) {
        sscanf(input + i, "%2hhx", &buffer[j]);
      }

      return buffer;
      }
* Fungsi `_decode_hex` adalah sebuah fungsi dalam program yang bertanggung jawab untuk melakukan dekode terhadap string dalam format heksadesimal. Fungsi ini melakukan iterasi pada setiap pasangan karakter heksadesimal dalam string input menggunakan loop `for`. Untuk setiap pasangan karakter heksadesimal, fungsi menggunakan fungsi `sscanf` untuk mengonversi pasangan tersebut menjadi byte dan menyimpannya dalam buffer hasil dekode.

      static char *reverse_text(const char *input) {
      size_t len = strlen(input);
      char *buffer = malloc(len + 1);

      for (size_t i = 0; i < len; ++i) {
        buffer[i] = input[len - i - 1];
      }
      buffer[len] = '\0';

      return buffer;
      }
* Fungsi `_reverse_text` adalah sebuah fungsi dalam program yang bertanggung jawab untuk membalikkan urutan karakter dalam sebuah string. Inti dari Fungsi ini ada pada `buffer[i] = input[len - i - 1];` Baris ini melakukan pembalikan urutan karakter dengan menyalin karakter dari string input ke buffer dalam urutan terbalik. Hasil pembalikan disimpan dalam buffer yang dialokasikan secara dinamis, dengan ukuran yang sama dengan panjang string asli ditambah satu byte untuk karakter null terminator.

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
* Fungsi `main` adalah inti dari program yang menerima argumen baris perintah berupa `<nama file>` dan `<isi file>`.  Jika argumen valid, nama file dan konten file disimpan dalam variabel terpisah. Kemudian, fungsi memeriksa ekstensi atau tipe file berdasarkan nama file menggunakan fungsi `strstr`. Berdasarkan tipe file yang cocok antara `base64`, `rot13`, `hex`, `rev`, selain itu `Unsupported file type` fungsi dekode yang sesuai dipanggil dengan konten file sebagai argumen, dan hasilnya disimpan dalam variabel `decoded_content`.

### Kendala Pengerjaan Soal 2

* Tidak berhasil membuat program menjadi berbasi FUSE, hanya bisa men-decode file.txt saja.
* Belum membuat sebuah sistem keamanan khusus untuk folder "rahasia-berkas" menggunakan FUSE.  

### Screenshot Hasil Pengerjaan Soal 2
* hasil decode `enkripsi_rot13.txt`
![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/145766477/0d84d239-daf7-4908-ad83-7a8e1d85455d)
* hasil decode `new-hex.txt`
![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/145766477/5e3d1e59-4c6d-4076-8c45-58c57352374d)
* hasil decode `notes-base64.txt`
![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/145766477/e7c7aa17-2f21-4337-b260-356c3a421e10)
* hasil decode `rev-text.txt`
![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/145766477/d5931d6c-0fdd-424e-a773-1fae51084ea9)

## Soal 3

**oleh Tio Axellino Irin (5027231065)**

### Deskripsi Soal 3

Seorang arkeolog menemukan sebuah gua yang didalamnya tersimpan banyak relik dari zaman praaksara, sayangnya semua barang yang ada pada gua tersebut memiliki bentuk yang terpecah belah akibat bencana yang tidak diketahui. Sang arkeolog ingin menemukan cara cepat agar ia bisa menggabungkan relik-relik yang terpecah itu, namun karena setiap pecahan relik itu masih memiliki nilai tersendiri, ia memutuskan untuk membuat sebuah file system yang mana saat ia mengakses file system tersebut ia dapat melihat semua relik dalam keadaan utuh, sementara relik yang asli tidak berubah sama sekali.
Ketentuan :

a. Buatlah sebuah direktori dengan ketentuan seperti pada tree berikut

    .
    ├── [nama_bebas]
    ├── relics
    │   ├── relic_1.png.000
    │   ├── relic_1.png.001
    │   ├── dst dst…
    │   └── relic_9.png.010
    └── report

b. Direktori **[nama_bebas]** adalah direktori FUSE dengan direktori asalnya adalah direktori relics. Ketentuan Direktori **[nama_bebas]** adalah sebagai berikut :

- Ketika dilakukan listing, isi dari direktori **[nama_bebas]** adalah semua relic dari relics yang telah tergabung.

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/120b0ab4-15aa-41a2-8eb9-3d0d5a48d166)

- Ketika dilakukan copy (dari direktori **[nama_bebas]** ke tujuan manapun), file yang disalin adalah file dari direktori **relics** yang sudah tergabung.

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/8f48fac6-2a67-401d-8a44-7b1a119a8656)

- Ketika ada file dibuat, maka pada direktori asal (direktori **relics**) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb.

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/30cf055b-7a00-46de-8b3f-cb90dfe69ae1)

- File yang dipecah akan memiliki nama **[namafile].000** dan seterusnya sesuai dengan jumlah pecahannya.

- Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

c. Direktori **report** adalah direktori yang akan dibagikan menggunakan Samba File Server. Setelah kalian berhasil membuat direktori **[nama_bebas]**, jalankan FUSE dan salin semua isi direktori **[nama_bebas]** pada direktori **report**.

![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/ec30db9c-b16c-43c7-ace2-fa2d9d8ceb3c)


d. Catatan:

- pada contoh terdapat 20 relic, namun pada zip file hanya akan ada 10 relic

- **[nama_bebas]** berarti namanya dibebaskan

- pada soal 3c, cukup salin secara manual. File Server hanya untuk membuktikan bahwa semua file pada direktori **[nama_bebas]** dapat dibuka dengan baik.

e. [discoveries.zip](https://drive.google.com/file/d/1BJkaBvGaxqiwPWvXRdYNXzxxmIYQ8FKf/view)

### Penyelesaian Soal 3

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

### Penjelasan Soal 3

*  **Header dan Makro**

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

    `#define FUSE_USE_VERSION 28`: Menentukan versi FUSE yang digunakan.
    
    `#include <fuse.h>`: Menyertakan header FUSE yang diperlukan untuk menggunakan fungsi-fungsi FUSE.
    
    Header lainnya digunakan untuk operasi file, input/output, manajemen direktori, dan memori.
    
    `#define MAX_CONTENT_SIZE 1024`: Menentukan ukuran maksimal isi berkas yang disimpan dalam memori.
    
    `#define CHUNK_SIZE 10240`: Menentukan ukuran tiap potongan berkas yang akan disimpan di disk.

* **Struktur Data `file_node`**

        struct file_node {
            char name[256];
            char content[1024];
            size_t size;
            struct file_node *next;
        };

        static struct file_node *file_list = NULL;
        static const char *dir_path = "/home/ludwigd/SisOP/Praktikum4/nomor_3/relics";

    `file_node`: Struktur data untuk menyimpan informasi berkas, termasuk nama (`name`), isi (`content`), ukuran (`size`), dan pointer ke berkas berikutnya dalam linked list (`next`).
    
    `file_list`: Pointer ke linked list berkas yang ada dalam memori.
    
    `dir_path`: Direktori di mana potongan berkas disimpan.

* **Fungsi `find_file`**

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

    Fungsi ini mencari berkas dalam linked list berdasarkan path yang diberikan.
  
    `path + 1` digunakan untuk menghilangkan karakter awal '/' dari path.

* **Fungsi `split_file`**

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
                if (bytes_to_write > CHUNK_SIZE) {
                    bytes_to_write = CHUNK_SIZE;
                }

                fwrite(content + offset, 1, bytes_to_write, file);
                fclose(file);

                offset += bytes_to_write;
                part_count++;
            }
        }

    Fungsi ini memecah berkas besar menjadi beberapa potongan dan menyimpannya di direktori yang ditentukan (`dir_path`).

    Setiap potongan disimpan dalam file terpisah dengan penamaan berkas menggunakan pola `filename.###`.

* **Fungsi `do_getattr`**

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
    
    Mengambil atribut berkas atau direktori.

    Jika path adalah root (`/`), atribut direktori diisi.

    Jika berkas ditemukan dalam linked list, atribut berkas diisi.

    Jika berkas adalah potongan berkas (`relic_###.png`), ukuran berkas dihitung berdasarkan semua potongan yang ada.

* **Fungsi `do_readdir`**

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

    Membaca isi direktori root (`/`).
   
    Mengisi buffer dengan nama berkas dan direktori dalam linked list dan potongan berkas dalam direktori `dir_path`.

* **Fungsi `do_open`**

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

    Membuka berkas. Jika berkas tidak ditemukan dalam linked list, dicek apakah berkas tersebut adalah potongan berkas.

* **Fungsi `do_read`**

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

    Membaca isi berkas. Jika berkas adalah potongan berkas, gabungkan potongan tersebut dan baca isinya.

    Jika berkas ditemukan dalam linked list, baca dari memori. Jika tidak, gabungkan dan baca potongan berkas dari disk.

* **Fungsi `do_create`**

        static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
            struct file_node *new_file = malloc(sizeof(struct file_node));
            if (!new_file) {
                return -ENOMEM;
            }
            memset(new_file, 0, sizeof(struct file_node));
            strncpy(new_file->name, path + 1, sizeof(new_file->name) - 1);
            new_file->next = file_list;
            file_list = new_file;
            return 0;
        }

    Membuat berkas baru dan menambahkannya ke linked list.

    `path + 1` digunakan untuk menghilangkan karakter awal '/' dari path.

* **Fungsi `do_write`**

        static int do_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
            struct file_node *file = find_file(path);
            if (!file) {
                return -ENOENT;
            }

            if (offset + size > MAX_CONTENT_SIZE) {
                return -EFBIG;
            }

            memcpy(file->content + offset, buf, size);
            if (offset + size > file->size) {
                file->size = offset + size;
            }

            split_file(file->name, file->content, file->size);
            return size;
        }

    Menulis isi ke berkas. Potongan berkas akan dibuat sesuai dengan offset dan ukuran.

    Jika ukuran total melebihi `MAX_CONTENT_SIZE`, pengembalian error `-EFBIG`.

* **Fungsi `do_truncate`**

        static int do_truncate(const char *path, off_t size) {
            struct file_node *file = find_file(path);
            if (!file) {
                return -ENOENT;
            }

            if (size < file->size) {
                file->size = size;
            }
            memset(file->content + file->size, 0, MAX_CONTENT_SIZE - file->size);

            split_file(file->name, file->content, file->size);
            return 0;
        }

    Mengubah ukuran berkas. Jika ukuran diperkecil, potongan berkas yang berlebih akan dihapus.

* **Fungsi `do_copy` dan `do_copy_from_relic`**

        static int do_copy(const char *src_path, const char *dst_path, int mode) {
            char buf[1024];
            int fd_src = open(src_path, O_RDONLY);
            if (fd_src == -1) {
                return -errno;
            }

            int fd_dst = open(dst_path, O_WRONLY | O_CREAT, mode);
            if (fd_dst == -1) {
                close(fd_src);
                return -errno;
            }

            ssize_t bytes_read;
            while ((bytes_read = read(fd_src, buf, sizeof(buf))) > 0) {
                if (write(fd_dst, buf, bytes_read) != bytes_read) {
                    close(fd_src);
                    close(fd_dst);
                    return -errno;
                }
            }

            close(fd_src);
            close(fd_dst);
            return 0;
        }

        static int do_copy_from_relic(const char *src_path, const char *dst_path) {
            return do_copy(src_path, dst_path, 0644); // Menyalin dengan mode baca untuk semua orang.
        }

    Menyalin isi berkas dari satu path ke path lain.

    `do_copy_from_relic` digunakan untuk menyalin berkas dari potongan berkas.

* **Main Function**

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

    `operations`: Struktur yang mendefinisikan fungsi-fungsi FUSE yang diimplementasikan.

    `main`: Fungsi utama yang memanggil `fuse_main` untuk menjalankan sistem berkas FUSE.

### Kendala Pengerjaan Soal 3

* Pada point ke 3 soal b:

  ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/0a9b33b4-3613-4850-807d-57e8a02fc233)

  Seharusnya file `sample_img.jpg` tersebut terpecah di directory relics, seperti ini:

  ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/907d14be-402b-45b9-a4ae-f237b8ac49b4)

  - Penyelesaian

        Memodifikasi kode bagian fungsi `do_create`, berikut kode setelah dimodifikasi:

            static int do_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
                struct file_node *new_file = malloc(sizeof(struct file_node));
                strcpy(new_file->name, path + 1);
                new_file->size = 0;
                new_file->content[0] = '\0';
                new_file->next = file_list;
                file_list = new_file;

                char src_file_path[1024];
                char dst_file_path[1024];
                snprintf(src_file_path, sizeof(src_file_path), "%s%s", dir_path, path);
                snprintf(dst_file_path, sizeof(dst_file_path), "%s/%s", dir_path, path + 1);

                int res = do_copy(path, path + 1, mode);
                if (res == 0) {
                    split_file(path + 1, NULL, 0);
                }

                return res;
            }

        Dalam modifikasi ini, setelah membuat file kosong di direktori FUSE mount, kode akan memanggil fungsi `do_copy` untuk menyalin file dari direktori sumber (dalam kasus Anda, `sample_img.jpg`) ke direktori `relics`. Kemudian, fungsi `split_file` dipanggil dengan `content` diatur ke `NULL` dan `size` diatur ke `0`, yang akan memaksa fungsi `split_file` untuk membaca isi file dari fragmen-fragmennya di direktori `relics`.

    - Hasil Modifikasi

        ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/992e7005-d02c-43b8-b26a-78f5bdfae7ff)

* Pada point ke 5 soal b:

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/b72d7cf5-85aa-4c6c-a938-ff0c04d3bccf)

    Seharusnya file sample_img.jpg tersebut dapat dihapus di directory FUSE mount (kecap)

    - Penyelesaian

        Menambahkan fungsi pada kode yaitu fungsi `do_unlink`, seperti ini:

            static int do_unlink(const char *path) {
                struct file_node *file = find_file(path);
                if (!file) {
                    return -ENOENT;
                }

                // Remove the file node from the linked list
                struct file_node *current = file_list;
                struct file_node *prev = NULL;
                while (current) {
                    if (current == file) {
                        if (prev) {
                            prev->next = current->next;
                        } else {
                            file_list = current->next;
                        }
                        break;
                    }
                    prev = current;
                    current = current->next;
                }

                // Remove the file and its fragments from the relics directory
                char file_path[1024];
                snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, file->name);
                unlink(file_path);

                size_t chunk_num = 0;
                while (1) {
                    snprintf(file_path, sizeof(file_path), "%s/%s.%03zu", dir_path, file->name, chunk_num);
                    if (unlink(file_path) == -1) {
                        if (errno == ENOENT) {
                            break;
                        }
                        return -errno;
                    }
                    chunk_num++;
                }

                free(file);

                return 0;
            }

        Fungsi do_unlink ini melakukan beberapa hal:

        * Mencari node file dalam linked list file_list.
        
        * Menghapus node file dari linked list.
        
        * Menghapus file utama dari direktori relics.
        
        * Menghapus semua fragmen file dari direktori relics.
        
        * Membebaskan memori yang dialokasikan untuk node file.

                static struct fuse_operations operations = {
                    .getattr = do_getattr,
                    .readdir = do_readdir,
                    .open = do_open,
                    .read = do_read,
                    .create = do_create,
                    .write = do_write,
                    .truncate = do_truncate,
                    .unlink = do_unlink, // Tambahkan baris ini
                };

    - Hasil Modifikasi

        ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/fe5e6ed1-bc01-417e-8457-94c19002f714)

### Screenshot Hasil Pengerjaan Soal 3

* Soal b point 1:

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/86f8bef7-03ab-4775-bd19-22dfc1905593)

* Soal b point 2:

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/68e97b2b-c13b-4766-ad48-60ad826a2e4d)

* Soal b point 3 dan 4:

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/992e7005-d02c-43b8-b26a-78f5bdfae7ff)

* Soal b point 5:

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/fe5e6ed1-bc01-417e-8457-94c19002f714)

* Soal c:

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/20399610-9b2d-4c1f-bbbd-1654d7ac1b4c)

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/e7fb793b-0f32-492c-a379-97b60e20e732)

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/6b00e9e7-9ce4-40a5-996a-6815e27f7d39)

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/bc7d5bd5-ceee-4e57-85b9-6b2aa6eb8edc)

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/3a6b9794-9d0e-425f-9bda-8ad7abbe1ff4)

    ![image](https://github.com/fioreenza/Sisop-4-2024-MH-IT25/assets/144349814/81655ce5-beb9-466d-82ec-977915c56620)






