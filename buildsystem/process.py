import os
import subprocess


def run(command, *, cwd=None, env=None):
    return subprocess.run(command, cwd=cwd, env=env, check=False).returncode


def process_environment(environment):
    result = os.environ.copy()
    result.update({str(key): str(value) for key, value in environment.get("ENV", {}).items()})
    return result
