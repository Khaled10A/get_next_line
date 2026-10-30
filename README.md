*This project has been created as part of the 42 curriculum by kalnajja.*

# get_next_line

## Description

`get_next_line` is a C function that returns one line at a time from a file
descriptor. Each call to `get_next_line(fd)` returns the next line read from
`fd`, including the terminating `\n` (except for the last line if the file does
not end with one). When there is nothing left to read, or when an error occurs,
it returns `NULL`.

The goal of the project is to learn about **static variables** (data that
survives between calls), manual memory management, and reading from file
descriptors with `read()` using a configurable buffer size.

Allowed functions: `read`, `malloc`, `free`.

### Files

| File | Role |
| --- | --- |
| `get_next_line.c` / `get_next_line.h` | Mandatory part (one file descriptor) |
| `get_next_line_utils.c` | Helper functions |
| `get_next_line_bonus.c` / `get_next_line_bonus.h` | Bonus: several file descriptors at once |
| `get_next_line_utils_bonus.c` | Helper functions for the bonus |

## Instructions

Compile the mandatory part together with your own `main.c`:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c
```

Compile the bonus part:

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c
```

`BUFFER_SIZE` is the number of bytes passed to each `read()` call. If the flag is
omitted, the default value is `42`.

Usage example:

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int	main(void)
{
	int		fd;
	char	*line;

	fd = open("file.txt", O_RDONLY);
	line = get_next_line(fd);
	while (line)
	{
		printf("%s", line);
		free(line);
		line = get_next_line(fd);
	}
	close(fd);
	return (0);
}
```

The function also works with standard input (`fd = 0`).

## Algorithm

The algorithm is built around a **stash**: a `static char *` that keeps the text
that was read from the file descriptor but not yet returned.

Each call to `get_next_line(fd)` does the following:

1. **Validate** the input: `fd` must be non-negative and `BUFFER_SIZE` must be
   positive.
2. **Read into the stash** (`ft_read_stash`): while the stash does not contain
   a `\n` and `read()` has not reached the end of the file, read `BUFFER_SIZE`
   bytes into a temporary buffer, add a terminating `\0`, and append it to the
   stash with `ft_strjoin`.
3. **Check for end of file**: if the stash is `NULL` or empty, there is nothing
   left to return, so it is freed and `NULL` is returned.
4. **Extract the line** (`ft_get_line`): copy everything from the beginning of
   the stash up to and including the first `\n` (or up to the end of the stash if
   there is none).
5. **Update the stash** (`ft_new_stash`): keep only what comes after the
   returned line, so the next call starts from there.

### Call tree

Who calls whom (the bonus has the same structure, only the stash is an array):

```
get_next_line(fd)                     entry point, owns the static stash
├── ft_read_stash(fd, &stash)         fill the stash until it holds a '\n' or EOF
│   ├── malloc                        temporary read buffer (BUFFER_SIZE + 1)
│   ├── ft_strchr(*stash, '\n')       loop condition: is a full line already stored?
│   ├── read(fd, buffer, BUFFER_SIZE) get more data from the file descriptor
│   ├── ft_update_stash(...)          append the new data to the stash
│   │   └── ft_strjoin(*stash, buffer)
│   │       └── ft_strlen (x2)        sizes needed to allocate the joined string
│   ├── ft_error(buffer, stash)       failure path: read error or failed join
│   └── free(buffer)                  buffer is no longer needed
├── free(stash)                       nothing left to return (EOF): reset the stash
└── ft_get_result(&stash)             split the stash into "line" + "remainder"
    ├── ft_get_line(*stash)           build the line to return
    ├── ft_new_stash(*stash)          build the remainder for the next call
    ├── ft_error(NULL, stash)         failure path: a malloc failed
    └── free(*stash)                  the old stash is replaced by the remainder
```

| Function | Called by | Why |
| --- | --- | --- |
| `ft_read_stash` | `get_next_line` | Read from `fd` until the stash contains a full line or the file ends |
| `ft_strchr` | `ft_read_stash` | Detect whether the stash already contains a `\n`, to stop reading |
| `ft_update_stash` | `ft_read_stash` | Terminate the buffer with `\0`, join it to the stash, free the old stash |
| `ft_strjoin` | `ft_update_stash` | Allocate a new string made of the stash followed by the buffer |
| `ft_strlen` | `ft_strjoin` | Compute the size of the new allocation (static, used only in the utils file) |
| `ft_get_result` | `get_next_line` | Produce the line and update the stash in one step |
| `ft_get_line` | `ft_get_result` | Copy the stash up to and including the first `\n` |
| `ft_new_stash` | `ft_get_result` | Copy what follows the first `\n`, so the next call resumes there |
| `ft_error` | `ft_read_stash`, `ft_get_result` | Free the buffer and the stash, reset the stash to `NULL`, return `NULL` |

### Why this design

- **It reads as little as possible.** The loop stops as soon as a `\n` is
  in the stash, so the function never reads the whole file at once, as the
  subject requires.
- **It does not depend on `BUFFER_SIZE`.** The stash is the only place where
  state lives, so the result is the same with `BUFFER_SIZE=1`, `42`, `9999` or
  `10000000`. A buffer larger than a line simply means that the extra bytes stay
  in the stash for the next call.
- **It handles all the edge cases in the same way:** empty file, file made only
  of newlines, last line without a `\n`, very long lines, invalid file
  descriptor.
- **Errors never leave memory behind.** Every failure path (a failed `malloc` or
  a failed `read`) goes through `ft_error`, which frees the buffer and the stash
  and resets the stash to `NULL` before returning `NULL`.

### Trade-offs

`ft_strjoin` allocates a new string and copies the whole stash each time data is
appended, and `ft_strchr` scans the stash on every iteration. With a very small
`BUFFER_SIZE` and a very long line this is quadratic. It was kept for
simplicity and readability, since it is correct for every input.

### Bonus

The bonus uses **a single static variable**, `static char *stash[1024]`, with one
slot per file descriptor. Calls can alternate between fd 3, 4 and 5 without
mixing their reading state. File descriptors outside the range `0..1023` return
`NULL`.

## Resources

- `man 2 read`, `man 3 malloc`, `man 3 free`
- Static variables and storage duration in C (C reference documentation)
- The 42 Norm and the `get_next_line` subject
- Peer discussions with other 42 Amman students

### Use of AI

AI was used as a support tool, **after** the first implementation was
written, for these tasks:

- **Code review**: finding memory-leak paths when `malloc` fails
  (`ft_read_stash`, `ft_get_result`) and the out-of-bounds risk with a fixed-size
  stash array.
- **Testing**: writing a test harness with AddressSanitizer and a `malloc`
  wrapper that fails on demand, to check every failure path.
- **Bonus files**: producing the `_bonus` versions of the three files from the
  mandatory ones.
- **Tooling help**: Git and SSH setup, and drafting this README.

All code was read, understood and tested by me, and I can explain every
function during the evaluation.
