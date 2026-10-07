# gmsv_file
A simple file i/o library for Garry's Mod written in ANSI C.

## Installing
1. Go to the "Releases" of this GitHub repository.
2. Download the latest `.dll` file for your platform (see the table below).
3. Copy that file to `garrysmod/lua/bin` of your server.

| Server branch    | Windows               | Linux                   |
|------------------|-----------------------|-------------------------|
| default (32-bit) | `gmsv_file_win32.dll` | `gmsv_file_linux.dll`   |
| x86-64 (64-bit)  | `gmsv_file_win64.dll` | `gmsv_file_linux64.dll` |

## Building
1. Install premake5
2. In the `gmsv_file` folder, use `premake5` followed by your platform (`vs2022` for windows, `gmake` for linux).
3. Navigate to the `project` folder.
4. If on Windows, open the `.sln` file and build the `x86` or `x86_64` configuration, if on Linux, run `make config=x86` or `make config=x86_64`.
5. The compiled binary should be in the `gmsv_file/bin` folder.

Building the `x86` configuration on a 64-bit Linux host requires a multilib toolchain (e.g. `gcc-multilib` and `g++-multilib` on Debian/Ubuntu).

## Using
The module provides barebones functionality for file manipulation. It's being worked on, more features are coming soon!
(All functions are relative to `garrysmod/` folder!!)

```lua
--- Appends contents at the end of file.
-- @return[Boolean success]
File.append(filename, contents)

--- Deletes file.
-- @return[Boolean success]
File.delete(filename)

--- Creates a folder with specified name.
-- Returns false if the folder exists, or was unable to be created.
-- @return[Boolean success]
File.mkdir(directory)

--- Reads specified file.
-- @return[String contents]
File.read(filename)

--- Writes contents to file. Overwrites existing contents.
-- @return[Boolean success]
File.write(filename, contents)
```
