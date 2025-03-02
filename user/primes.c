#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int p_left) __attribute__((noreturn));

void primes(int p_left) {
    int prime;
    int p_right[2];

    // Đọc số nguyên tố đầu tiên từ pipe
    if (read(p_left, &prime, sizeof(prime)) == 0) {
        close(p_left);
        exit(0);
    }

    printf("prime %d\n", prime);
    
    // Tạo pipe mới để gửi số cho tiến trình con tiếp theo
    pipe(p_right);

    if (fork() == 0) {
        // Tiến trình con: đóng tất cả các file descriptor không cần thiết
        close(p_left);      // Đóng pipe cũ vì đã không cần nữa. Không có dòng này thì sau một số lần lặp, các pipe cũ bị rò rỉ và dữ liệu có thể bị trộn lẫn giữa các pipe khác nhau, gây ra đầu ra không mong muốn.
        close(p_right[1]);  // Đóng đầu ghi của pipe mới
        primes(p_right[0]); // Tiếp tục với đầu đọc của pipe mới
    } else {
        // Tiến trình cha: lọc và gửi các số
        close(p_right[0]);  // Đóng đầu đọc của pipe mới
        
        int num;
        while (read(p_left, &num, sizeof(num)) > 0) {
            if (num % prime != 0) {
                write(p_right[1], &num, sizeof(num));
            }
        }
        
        // Đóng tất cả các pipe khi đã sử dụng xong
        close(p_left);
        close(p_right[1]);
        
        // Đợi tiến trình con kết thúc
        wait(0);
        exit(0);
    }
}

int main() {
    int p[2];
    pipe(p);
    
    if (fork() == 0) {
        // Tiến trình con: chỉ cần đọc từ pipe
        close(p[1]);  // Đóng đầu ghi vì không cần
        primes(p[0]); // Bắt đầu xử lý số nguyên tố
    } else {
        // Tiến trình cha: gửi các số từ 2 đến 280
        close(p[0]);  // Đóng đầu đọc vì không cần
        
        for (int i = 2; i <= 280; i++) {
            write(p[1], &i, sizeof(i));
        }
        
        close(p[1]);  // Đóng đầu ghi khi đã gửi xong
        
        // Đợi tiến trình con kết thúc
        wait(0);
    }
    
    exit(0);
}