# PHP Module Player

The PHP Module Player is an extension for the PHP language written in pure C code, built on top of the libmikmod that is linked as a submodule in this repository.

The intent of this extension is to enabled the support to play [tracker music](https://en.wikipedia.org/wiki/Module_file), also know as chiptune songs, from the PHP CLI SAPI.

The extension supports all tracker related extensios, like .MOD, .XM, .IT, .S3M and etc. For more format support information, check the [libmikmod](http://mikmod.sourceforge.net/) documentation.

Here is a [video](https://www.youtube.com/watch?v=rSjuUIpTbYk) demonstrating the usage of the extension.

> I made this extension just for fun and research and I couldn't support it in full-time, so, if you find any bug or need to make a contribution, feel free to send me a PR.

# Requirements

- PHP 7 (for older versions, checkout to the git tag 1.1.0)
- libmikmod (included in the repository)

# Installation

As this extension is not really useful for everyone, it will not be available in the PHP repository for easy installation, but the process is quite simple.

### 1. Clone the repository

The first thing you need to do, is to clone this repository and initialize it submodule:

```
$ git clone git@github.com:devsdmf/php-mod-player.git /tmp/path/to/extension
$ cd /tmp/path/to/extension
$ git submodule update --init
```

### 2. Installing the libmikmod

This extension requires the libmikmod to be installed and located in the system path, se we need to compile and install it:

> To compile the library, there is a lot of ways, here I'll demonstrate the simplest way, but if you have any problem, you could check the library documentation or ask me.

```
# assuming that you're in the repository root
$ cd mikmod/libmikmod
$ ./configure
$ make
$ sudo make install
```

> If the configure command fails, you could recompile the autoconf script using the following commands in the library root.
```
$ autoreconf --install && autoconf configure.ac > configure
$ chmod +x configure
```
Now, try to compile the library again as describe above.

### 3. Installing the extension

Now, we just need to compile the extension as any other PHP extension.

```
# assuming that you're in the repository root
$ phpize
$ ./configure
$ make
$ sudo make install
```

> You may need to add the path to the modplayer.so file in your php.ini file.

And it's done!

# Usage

The extension itself, has only three functions, all of them is described below:

### modplayer_play

(PHP 5 >= 5.5, PHP 7)
modplayer_play - Play a module file from the disk

#### Description
`int modplayer_play( string $filename [, int max_channels [, bool curious [, int reverb ]]] )`

Start the module player and play the $filename file in background.

#### Parameters

- *$filename* - The module audio file in the folder, or with the absolute path.
- *$max_channels* - The tracker channels to be used (default: 64).
- *$curious* - Sets the mixer to search and mix the audio on the fly (default: false).
- *$reverb* - Sets the reverb level from 0 to 15 (default: 1).

#### Return Values

The function returns the PID of the player sub-process or FALSE if an error occurs.

#### Examples

Playing a sample module file:

```php
if (!modplayer_getpid()) {
    $pid = modplayer_play('sample.mod');
} else {
    echo 'Module already playing.';
}
```

### modplayer_getpid

(PHP 5>= 5.5, PHP 7)
modplayer_getpid - Get the PID of the player subprocess

#### Description
`int modplayer_getpid( )`

Get the PID of the player subprocess.

#### Parameters

N/A

#### Return Values

The function returns the PID of the player sub-process or FALSE if none is playing.

#### Examples

Getting the PID of the player sub-process:

```php
$pid = modplayer_getpid();
```

### modplayer_stop

(PHP 5>= 5.5, PHP 7)
modplayer_stop - Stop a currently playing song.

#### Description
`bool modplayer_stop( )`

Stop a currently playing song.

#### Parameters

N/A

#### Return Values

Returns **TRUE** if the process was stopped otherwise **FALSE** if an error occurs.

#### Examples

Stopping the player

```php
if (modplayer_getpid()) {
    if (modplayer_stop()) {
        echo "Success!";
    } else {
        echo "An error occured!";
    }
}
```

# License

This library is licensed under the [MIT license](LICENSE).