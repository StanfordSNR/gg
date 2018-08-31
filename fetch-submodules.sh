#!/bin/sh -ex

exec git submodule update --recursive --init test_vectors
exec git submodule update --init toolchain
exec git submodule update --init cpptoml
