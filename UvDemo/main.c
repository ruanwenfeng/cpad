

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0600
#include <stdio.h>
#include <stdlib.h>
#include <uv.h>
#include <assert.h>
#include <fcntl.h>

// #ifdef WIN32
// #pragma comment(lib, "ws2_32.lib")
// #pragma comment(lib, "Iphlpapi.lib")
// #pragma comment(lib, "Psapi.lib")
// #pragma comment(lib, "Userenv.lib")
// #endif
#pragma warning(disable:4244 4267) 




void on_read(uv_fs_t *req);

uv_fs_t open_req;
uv_fs_t read_req;
uv_fs_t write_req;

static char buffer[1024];

static uv_buf_t iov;

void on_write(uv_fs_t *req) {
    if (req->result < 0) {
        fprintf(stderr, "Write error: %s\n", uv_strerror((int)req->result));
    }
    else {
        uv_fs_read(uv_default_loop(), &read_req, open_req.result, &iov, 1, -1, on_read);
    }
}

void on_read(uv_fs_t *req) {
    if (req->result < 0) {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
    }
    else if (req->result == 0) {
        uv_fs_t close_req;
        // synchronous
        uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
    }
    else if (req->result > 0) {
        iov.len = req->result;
        uv_fs_write(uv_default_loop(), &write_req, 1, &iov, 1, -1, on_write);
    }
}

void on_open(uv_fs_t *req) {
    // The request passed to the callback is the same as the one the call setup
    // function was passed.
    assert(req == &open_req);
    if (req->result >= 0) {
        iov = uv_buf_init(buffer, sizeof(buffer));
        uv_fs_read(uv_default_loop(), &read_req, req->result,
                   &iov, 1, -1, on_read);
    }
    else {
        fprintf(stderr, "error opening file: %s\n", uv_strerror((int)req->result));
    }
}

int main(int argc, char **argv) {
    LoadLibrary(L"advapi32.dll");

    uv_fs_open(uv_default_loop(), &open_req, argv[1], O_RDONLY, 0, on_open);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    uv_fs_req_cleanup(&open_req);
    uv_fs_req_cleanup(&read_req);
    uv_fs_req_cleanup(&write_req);
    return 0;
}



// typedef struct {
//     uv_write_t req;
//     uv_buf_t buf;
// } write_req_t;

// uv_loop_t *loop;
// uv_pipe_t stdin_pipe;
// uv_pipe_t stdout_pipe;
// uv_pipe_t file_pipe;

// void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
//     *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
// }

// void free_write_req(uv_write_t *req) {
//     write_req_t *wr = (write_req_t*) req;
//     free(wr->buf.base);
//     free(wr);
// }

// void on_stdout_write(uv_write_t *req, int status) {
//     free_write_req(req);
// }

// void on_file_write(uv_write_t *req, int status) {
//     free_write_req(req);
// }

// void write_data(uv_stream_t *dest, size_t size, uv_buf_t buf, uv_write_cb cb) {
//     write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
//     req->buf = uv_buf_init((char*) malloc(size), size);
//     memcpy(req->buf.base, buf.base, size);
//     uv_write((uv_write_t*) req, (uv_stream_t*)dest, &req->buf, 1, cb);
// }

// void read_stdin(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
//     if (nread < 0){
//         if (nread == UV_EOF){
//             // end of file
//             uv_close((uv_handle_t *)&stdin_pipe, NULL);
//             uv_close((uv_handle_t *)&stdout_pipe, NULL);
//             uv_close((uv_handle_t *)&file_pipe, NULL);
//         }
//     } else if (nread > 0) {
//         write_data((uv_stream_t *)&stdout_pipe, nread, *buf, on_stdout_write);
//         write_data((uv_stream_t *)&file_pipe, nread, *buf, on_file_write);
//     }

//     // OK to free buffer as write_data copies it.
//     if (buf->base)
//         free(buf->base);
// }

// int main(int argc, char **argv) {
//     loop = uv_default_loop();

//     uv_pipe_init(loop, &stdin_pipe, 0);
//     uv_pipe_open(&stdin_pipe, 0);

//     uv_pipe_init(loop, &stdout_pipe, 0);
//     uv_pipe_open(&stdout_pipe, 1);
    
//     uv_fs_t file_req;
//     int fd = uv_fs_open(loop, &file_req, "a.txt", O_CREAT | O_RDWR, 0644, NULL);
//     uv_pipe_init(loop, &file_pipe, 0);
//     uv_pipe_open(&file_pipe, fd);

//     uv_read_start((uv_stream_t*)&stdin_pipe, alloc_buffer, read_stdin);

//     uv_run(loop, UV_RUN_DEFAULT);
//     printf("end");
//     return 0;
// }
