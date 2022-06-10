from os.path import join, isfile

Import("env")

LIBRARY_DIR=".pio/libdeps/esp32dev/LittleFS_esp32/src/"
patchflag_path = join(LIBRARY_DIR, ".patching-done")

# patch file only if we didn't do it before
if not isfile(join(LIBRARY_DIR, ".patching-done")):
    original_file = join(LIBRARY_DIR , "LITTLEFS.cpp")
    patched_file = join("patches", "LITTLEFSC.cpp.patch")

    assert isfile(original_file) and isfile(patched_file)

    env.Execute("patch %s %s" % (original_file, patched_file))
    # env.Execute("touch " + patchflag_path)


    def _touch(path):
        with open(path, "w") as fp:
            fp.write("")

    env.Execute(lambda *args, **kwargs: _touch(patchflag_path))


