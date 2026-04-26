# merge_firmware.py
import os, subprocess
Import("env")

def merge_bins(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    esptool = env.subst("$PYTHONEXE") 
    result = subprocess.run([
        esptool, "-m", "esptool", "--chip", "esp32c3", "merge_bin",
        "-o", os.path.join(build_dir, "merged.bin"),
        "0x0",     os.path.join(build_dir, "bootloader.bin"),
        "0x8000",  os.path.join(build_dir, "partitions.bin"),
        "0x10000", os.path.join(build_dir, "firmware.bin"),
    ])
    if result.returncode == 0:
        print("✅ merged.bin generated at", build_dir)
    else:
        print("❌ merge_bin failed with code", result.returncode)

env.AddPostAction("buildprog", merge_bins)