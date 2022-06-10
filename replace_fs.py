# for mklitlefs
# see https://github.com/earlephilhower/mklittlefs/releases
Import("env")
print("Replace MKSPIFFSTOOL with mklittlefs")
env.Replace (MKSPIFFSTOOL = "mklittlefs")