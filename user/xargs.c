#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define MAXARGS 10
#define MAXLEN 512

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs <command> [args...]\n");
        exit(1);
    }

    char *cmd[MAXARGS];
    for (int i = 1; i < argc; i++) {
        cmd[i - 1] = argv[i];  // Sao chép lệnh và đối số đầu tiên
    }

    char buf[MAXLEN];
    int len = 0;
    
    while (read(0, &buf[len], 1) == 1) {  // Đọc từ stdin
        if (buf[len] == '\n') {  // Khi gặp dòng mới
            buf[len] = 0;
            cmd[argc - 1] = buf;  // Thêm tham số từ stdin
            cmd[argc] = 0;  // Đảm bảo danh sách tham số kết thúc

            if (fork() == 0) {
                exec(cmd[0], cmd);  // Thực thi lệnh
                fprintf(2, "xargs: exec failed\n");
                exit(1);
            } else {
                wait(0);
            }
            len = 0;  // Reset bộ đệm
        } else {
            len++;
        }
    }

    exit(0);
}
