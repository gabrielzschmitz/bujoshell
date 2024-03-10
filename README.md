<h1 align="center">
    <br>
    <img src="./icons/bujoshell.svg" alt="bujoshell icon"/>
    <br>
    bujoshell
    <br>
</h1>

<h4 align="center">A bullet journal app for the terminal.</h4>

<p align="center">
    <a href="./LICENSE"><img src="https://img.shields.io/badge/license-GPL-3.svg" alt="License"></a>
    <a href="https://www.buymeacoffee.com/gabrielzschmitz" target="_blank">
        <img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee" style="height: 20px !important;width: 87px;">
    </a>
    <a href="https://github.com/gabrielzschmitz/bujoshell">
        <img src="https://img.shields.io/github/stars/gabrielzschmitz/bujoshell?style=social" alt="Give me a Star">
    </a>
</p>

<p align="center">
  <a href="#-how-to-use">How to Use</a> •
  <a href="#-dependencies">Dependencies</a> •
  <a href="#-how-to-install">How to Install</a> •
  <a href="#-preferences">Preferences</a> •
  <a href="#-to-do">To-do</a> •
  <a href="#-contribute">Contribute</a> •
  <a href="#-license">License</a>
</p>

## 🚀 How to Use

To use, simply <b>type it</b> in the <b>terminal</b>:

```shell
$ bujoshell
```

## ⚓ Dependencies

It only needs [gcc](https://gcc.gnu.org/) or [tcc](https://bellard.org/tcc/)
to compile, [ncurses](https://invisible-island.net/ncurses/) as the graphic
library, [SQLite](https://www.sqlite.org/index.html) to manage the databases and
[pkg-config](https://github.com/freedesktop/pkg-config) to proper library's
linking.

**Note**: you'll need a [Nerd Font](https://www.nerdfonts.com/) patched font
for the icons.

```shell
ARCH LINUX
$ sudo pacman -S base-devel ncurses sqlite3 pkgconf

UBUNTU
$ sudo apt install build-essential libncurses5-dev libncursesw5-dev sqlite3 pkg-config

FEDORA
$ sudo dnf groupinstall 'Development Tools' && sudo dnf install ncurses-devel sqlite sqlite-devel sqlite-tcl pkgconf

MACOS
$ brew install gcc sqlite ncurses
```

## 💾 How to Install

To install you just need to copy and paste this 3 commands in the terminal:

```shell
$ git clone https://github.com/gabrielzschmitz/bujoshell.git
$ cd bujoshell
$ sudo make install
```

**Note**: a good practice is to clone the repo at _$HOME/.local/src/_

**Note**: first install all the _dependencies_!

## 🔧 Preferences

To change the default configurations, edit the
[config.h](./config.h),
then `sudo make install` to take effect. You can change those configs:

- <b><i>ICONS</i></b>: iconsoff - iconson - nerdicons;
- <b><i>MIN_WIDTH</i></b>: 1 - ∞;
- <b><i>MIN_HEIGTH</i></b>: 1 -∞;

## 📝 To-do

- [ ] Implement migranting and scheduling entrys
- [ ] Implement a template system
- [ ] Implement mouse support
- [ ] Make a welcome screen
- [ ] Implement a colorscheme system

## 🤝 Contribute

Feel free to contribute to the project!

## 📜 License

This software is licensed under the [GPL-3](./LICENSE) license.
