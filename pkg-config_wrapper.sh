#!/bin/sh
PKG_CONFIG_SYSROOT_DIR=/home/denizozbilici/rk3566-qt-sdk/buildroot-RK3566-Qt5.12.2-20221213/aarch64-buildroot-linux-gnu/sysroot
export PKG_CONFIG_SYSROOT_DIR
PKG_CONFIG_LIBDIR=/home/denizozbilici/rk3566-qt-sdk/buildroot-RK3566-Qt5.12.2-20221213/aarch64-buildroot-linux-gnu/sysroot/usr/lib/pkgconfig:/home/denizozbilici/rk3566-qt-sdk/buildroot-RK3566-Qt5.12.2-20221213/aarch64-buildroot-linux-gnu/sysroot/usr/share/pkgconfig
export PKG_CONFIG_LIBDIR
exec pkg-config "$@"
