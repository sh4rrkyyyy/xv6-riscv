#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_LOG_FILE "log.txt"
#define DEFAULT_FIFO_NAME "fifo"
#define BUFSIZE 1 << 11
#define PERIOD 10
#define FIFO_KEY "-fifo"
#define MODE_KEY "-mode"
#define FILE_KEY "-file"

volatile sig_atomic_t sigterm_flag = 0;
volatile sig_atomic_t sigint_flag = 0;
volatile sig_atomic_t sigalrm_flag = 0;
volatile sig_atomic_t sigusr1_flag = 0;
volatile sig_atomic_t sighup_flag = 0;

enum mode { DAEMON, FOREGROUND };
typedef struct {
  char fifo_name[BUFSIZE];
  enum mode mode;
  char file_name[BUFSIZE];
} config_t;

void print(FILE *f, char *ptr, int len, size_t *vol) {
  char buf[len + 10];
  memcpy(buf, ptr, len);
  buf[len] = '\0';
  if (buf[len - 1] != '\n') {
    buf[len++] = '\n';
    buf[len] = '\0';
  }
  *vol += strlen(buf);
  fputs(buf, f);
  fflush(f);
}

void print_stat(FILE *f, size_t cnt, size_t *vol, size_t cnt_alrm) {
  fprintf(f,
          "[STAT] Number of iterations %ld, Volume of messages %ld, "
          "Number of alarms %ld\n",
          cnt, *vol, cnt_alrm);
  fflush(f);
}

void handle_sigint(int sig) { sigint_flag = 1; }

void handle_sigterm(int sig) { sigterm_flag = 1; }

void handle_sigalrm(int sig) { sigalrm_flag = 1; }

void handle_sigusr1(int sig) { sigusr1_flag = 1; }

void handle_sighup(int sig) { sighup_flag = 1; }

void set_signals() {
  struct sigaction sa_int = {.sa_handler = handle_sigint};
  struct sigaction sa_term = {
      .sa_handler = handle_sigterm,
  };
  struct sigaction sa_quit = {
      .sa_handler = SIG_IGN,
  };
  struct sigaction sa_alrm = {
      .sa_handler = handle_sigalrm,
  };
  struct sigaction sa_usr1 = {
      .sa_handler = handle_sigusr1,
  };
  struct sigaction sa_hup = {
      .sa_handler = handle_sighup,
  };

  if (sigaction(SIGINT, &sa_int, NULL) == -1 ||
      sigaction(SIGTERM, &sa_term, NULL) == -1 ||
      sigaction(SIGQUIT, &sa_quit, NULL) == -1 ||
      sigaction(SIGALRM, &sa_alrm, NULL) == -1 ||
      sigaction(SIGUSR1, &sa_usr1, NULL) == -1 ||
      sigaction(SIGHUP, &sa_hup, NULL) == -1) {
    perror("sigaction");
    exit(EXIT_FAILURE);
  }
}

void daemonize(FILE **f, size_t cnt, size_t *vol, size_t cnt_alrm,
               config_t *config) {
  FILE *new_f = fopen(config->file_name, "a");
  if (!new_f) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  fprintf(*f, "DAEMONIZE\n");
  print_stat(*f, cnt, vol, cnt_alrm);
  dup2(fileno(new_f), STDOUT_FILENO);
  dup2(fileno(new_f), STDERR_FILENO);
  if (fclose(*f) < 0) {
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  *f = new_f;
}

void clean(FILE *f, int fd, config_t *config) {
  if (fclose(f) < 0) {
    perror("fclose");
    exit(EXIT_FAILURE);
  }
  if (fd != -1 && close(fd) < 0) {
    perror("close");
    exit(EXIT_FAILURE);
  }
  if (unlink(config->fifo_name) != 0) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }
  exit(EXIT_SUCCESS);
}
void parse_args(int argc, char **argv, config_t *config) {
  if (argc < 3) {
    fprintf(stderr, "Too few args\n");
    exit(EXIT_FAILURE);
  }
  for (int i = 1; i < argc;) {
    if (strcmp(argv[i], FIFO_KEY) == 0) {
      strcpy(config->fifo_name, argv[i + 1]);
      i += 2;
    } else if (strcmp(argv[i], MODE_KEY) == 0) {
      if (strcmp(argv[i + 1], "foreground") == 0) {
        config->mode = FOREGROUND;
      } else if (strcmp(argv[i + 1], "daemon") == 0) {
        config->mode = DAEMON;
      } else {
        fprintf(stderr, "Incorrect mode\n");
        exit(EXIT_FAILURE);
      }
      i += 2;
    } else if (strcmp(argv[i], FILE_KEY) == 0) {
      strcpy(config->file_name, argv[i + 1]);
      i += 2;
    } else {
      fprintf(stderr, "Incorrect input\n");
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv) {
  config_t config = {.mode = FOREGROUND,
                     .fifo_name = DEFAULT_FIFO_NAME,
                     .file_name = DEFAULT_LOG_FILE};
  parse_args(argc, argv, &config);
  set_signals();
  if (config.mode == DAEMON) {
    if (daemon(1, 0) == -1) {
      perror("daemon");
      exit(EXIT_FAILURE);
    }
  }
  FILE *f = (config.mode == DAEMON) ? fopen(config.file_name, "a") : stdout;
  if (!f) {
    perror("fopen");
    exit(EXIT_FAILURE);
  }
  if (mkfifo(config.fifo_name, 0600) == -1) {
    if (errno == EEXIST) {
      struct stat st;
      if (stat(config.fifo_name, &st) == -1) {
        perror("stat");
      }
      if (!S_ISFIFO(st.st_mode)) {
        fprintf(stderr,
                "File with name %s already exists, but it is not FIFO\n",
                config.fifo_name);
        exit(EXIT_FAILURE);
      }
      fprintf(f, "File with name %s already exists and it is FIFO\n",
              config.fifo_name);
      fflush(f);
    } else {
      perror("mkfifo");
      exit(EXIT_FAILURE);
    }
  } else {
    fprintf(f, "File with name %s has been created (it is FIFO)\n",
            config.fifo_name);
    fflush(f);
  }
  alarm(PERIOD);
  size_t cnt = 0;
  size_t cnt_alrm = 0;
  size_t vol = 0;
  while (1) {
    ++cnt;
    int fd;
    if ((fd = open(config.fifo_name, O_RDONLY)) == -1) {
      if (errno == EINTR) {
        if (sigterm_flag) {
          fprintf(f, "SIGTERM received in open\n");
          print_stat(f, cnt, &vol, cnt_alrm);
          clean(f, fd, &config);
        } else if (sigint_flag) {
          fprintf(f, "SIGINT received in open\n");
          print_stat(f, cnt, &vol, cnt_alrm);
          clean(f, fd, &config);
        } else if (sigusr1_flag) {
          print_stat(f, cnt, &vol, cnt_alrm);
          sigusr1_flag = 0;
        } else if (sigalrm_flag) {
          ++cnt_alrm;
          sigalrm_flag = 0;
          fprintf(f, "SIGALRM received in open\n");
          fflush(f);
          alarm(PERIOD);
        } else if (sighup_flag && !(config.mode == DAEMON)) {
          sighup_flag = 0;
          daemonize(&f, cnt, &vol, cnt_alrm, &config);
          config.mode = DAEMON;
        }
        continue;
      } else {
        perror("open");
        exit(EXIT_FAILURE);
      }
    }
    char buf[BUFSIZE];
    int len;
    bool is_sigint = false;
    while (1) {
      len = read(fd, buf, BUFSIZE);
      if (len < 0) {
        if (errno == EINTR) {
          if (sigterm_flag) {
            fprintf(f, "SIGTERM received in read\n");
            print_stat(f, cnt, &vol, cnt_alrm);
            clean(f, fd, &config);
          } else if (sigint_flag) {
            fprintf(f, "SIGINT received in read\n");
            sigint_flag = 0;
            print_stat(f, cnt, &vol, cnt_alrm);
            is_sigint = true;
          } else if (sigusr1_flag) {
            print_stat(f, cnt, &vol, cnt_alrm);
            sigusr1_flag = 0;
          } else if (sigalrm_flag) {
            ++cnt_alrm;
            sigalrm_flag = 0;
            fprintf(f, "SIGALRM received in read\n");
            fflush(f);
            alarm(PERIOD);
          } else if (sighup_flag && !(config.mode == DAEMON)) {
            sighup_flag = 0;
            daemonize(&f, cnt, &vol, cnt_alrm, &config);
            config.mode = DAEMON;
          }
          continue;
        } else {
          perror("read");
          exit(EXIT_FAILURE);
        }
      } else if (len == 0) {
        if (is_sigint) {
          clean(f, fd, &config);
        }
        if (fd != -1 && close(fd) < 0) {
          perror("close");
          exit(EXIT_FAILURE);
        }
        break;
      } else {
        char *ptr = buf;
        print(f, ptr, len, &vol);
      }
    }
  }
}
