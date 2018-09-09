#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := ESP32_Audio
CXXFLAGS += -std=c++14 -O2

include $(IDF_PATH)/make/project.mk

