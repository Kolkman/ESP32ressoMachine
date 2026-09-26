import os
from pathlib import Path
import subprocess

Import("env")


def read_firmware_version(project_dir):
    return (project_dir / "firmwareVersion").read_text(encoding="utf-8").strip()


def option_enabled(name):
    value = env.GetProjectOption(name, "")
    return value.strip().lower() in {"1", "true", "yes", "on"}


def extend_pythonpath(existing, extra_path):
    if not extra_path:
        return existing

    parts = [part for part in existing.split(os.pathsep) if part] if existing else []
    if extra_path in parts:
        return existing

    return os.pathsep.join([extra_path, *parts])


def find_intelhex_site_dir():
    pio_cache = Path.home() / ".platformio" / ".cache" / "uv" / "archive-v0"
    for module_init in sorted(pio_cache.glob("*/intelhex/__init__.py")):
        return str(module_init.parent.parent)
    return ""


project_dir = Path(env.subst("$PROJECT_DIR"))
intelhex_site_dir = find_intelhex_site_dir()

if intelhex_site_dir:
    env["ENV"]["PYTHONPATH"] = extend_pythonpath(
        env["ENV"].get("PYTHONPATH", ""), intelhex_site_dir
    )

subprocess.run(["make"], cwd=project_dir, check=True)

cppdefines = [("FIRMWAREVERSION", f'\\"{read_firmware_version(project_dir)}\\"')]

hostname = env.GetProjectOption("custom_hostname", "").strip()
if hostname:
    cppdefines.append(("HOSTNAME", f'\\"{hostname}\\"'))

if option_enabled("custom_only_black_button"):
    cppdefines.append("ONLY_BLACK_BUTTON")

if option_enabled("custom_testupdate"):
    cppdefines.append("TESTUPDATE")

env.Append(CPPDEFINES=cppdefines)