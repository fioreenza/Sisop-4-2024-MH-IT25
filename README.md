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

### Screenshot Hasil Pengerjaan Soal 2

## Soal 3

**oleh**

### Deskripsi Soal 3

### Penyelesaian Soal 3

### Penjelasan Soal 3

### Kendala Pengerjaan Soal 3

### Screenshot Hasil Pengerjaan Soal 2

### Screenshot Hasil Pengerjaan Soal 3

