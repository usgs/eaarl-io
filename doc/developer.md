# Developer Notes {#md_developer}

This is a collection of miscellaneous information of interest to developers
contributing to the eaarl-io library.

## Coding style

The project is set up to use clang-format to enforce a consistent coding style.
The style is configured in .clang-format and is based on the WebKit style, with
some modifications.

In Bash on Linux, you can run clang=format on all files in a directory
recursively using this command:

```sh
find -type f \( -name '*.c' -o -name '*.h' \) -print0 | \
    xargs -0 clang-format -style=file -i
```

In PowerShell on Windows, you can run clang-format on all files in a directory
recursively using these commands:

```ps1
clang-format -style=file -i @( Get-ChildItem -Recurse -Name -Filter *.c )
clang-format -style=file -i @( Get-ChildItem -Recurse -Name -Filter *.h )
```
