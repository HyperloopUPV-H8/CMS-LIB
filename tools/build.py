#!/usr/bin/python3
import os
import argparse
import shutil
import subprocess

# Requirements:
# colorama==0.4.6
from colorama import Fore


parser = argparse.ArgumentParser(
    prog="Build CMS-LIB",
    description="Build using CMake the CMS-LIB"
)

parser.add_argument(
    '-bb',
    '--build_behaviour',
    choices=['Release', 'ReleaseDebug', 'Debug'],
    required=True
)
parser.add_argument(
    '-target',
    '--target',
    choices=['NUCLEO','BOARD'],
    required=True
)
parser.add_argument(
    '-eth',
    '--ethernet_config',
    choices=['ON','OFF'],
    required=True
)

cmslib_path = os.environ.get('CMSLIB_PATH')


def main(args: argparse.Namespace):
    if not cmslib_path:
        print(Fore.RED + "CMSLIB_PATH env variable is missing")
        print(Fore.RESET, end="")
        return 1

    print(Fore.BLUE + "Building CMS-LIB:")
    print(Fore.BLUE + f"\tCMS-LIB path:    {cmslib_path}")
    print(Fore.BLUE + f"\tTarget:          {args.target}")
    print(Fore.BLUE + f"\tBuild Behaviour: {args.build_behaviour}")
    print(Fore.BLUE + f"\tEthernet:        {args.ethernet_config}")
    print(Fore.RESET)

    output_dir = os.path.join(cmslib_path, "build")
    os.makedirs(output_dir, exist_ok=True)

    cmake_exit_code = subprocess.call([
        "cmake",
        cmslib_path,
        "-B", output_dir,
        f"-DRELEASE={args.build_behaviour}",
        f"-DNUCLEO={'TRUE' if args.target == 'NUCLEO' else 'FALSE'}",
        f"-DETHERNET={'TRUE' if args.ethernet_config == 'ON' else 'FALSE'}",
        "-G", "Unix Makefiles"
    ])

    if cmake_exit_code != 0:
        print(Fore.RED, "\nCMake failed, aborted")
        print(Fore.RESET, end="")
        return cmake_exit_code

    make_exit_code = subprocess.call([
        "make",
        "-j", str(os.cpu_count()),
        "-C", output_dir
    ])

    if make_exit_code != 0:
        print(Fore.RED + "\nMake failed, aborted")
        print(Fore.RESET, end="")
        return make_exit_code

    final_cmslib_location = os.path.join(output_dir, args.build_behaviour)
    os.makedirs(final_cmslib_location, exist_ok=True)
    shutil.copyfile(
        os.path.join(output_dir, "libcms-lib.a"),
        os.path.join(final_cmslib_location, "libcms-lib.a")
    )

    print()
    print(Fore.GREEN + "CMS-LIB built successfuly!")
    print(Fore.GREEN + f"\tOutput path:     {final_cmslib_location}")
    print(Fore.GREEN + f"\tTarget:          {args.target}")
    print(Fore.GREEN + f"\tBuild Behaviour: {args.build_behaviour}")
    print(Fore.GREEN + f"\tEthernet:        {args.ethernet_config}")
    print(Fore.RESET, end="")

    return 0

if __name__ == "__main__":
    exit(main(parser.parse_args()))