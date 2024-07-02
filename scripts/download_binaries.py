#!/usr/bin/env python3
import os
import re
import sys
import requests
import hashlib
import myriota_auth as auth
import myriota_download as download
from dataclasses import dataclass

def license_disclaimer(filename):
    flex_sdk_version = '1.0.0'
    print('==================================== License Disclaimer ====================================')
    print(f'{filename} for Flex SDK {flex_sdk_version} has been downloaded.')
    print('This file and the related binary are licensed under the Permissive Binary License, Version 1.0 (the "License"); you may not use these files except in compliance with the License.')
    print('You may obtain a copy of the License here: LICENSE-permissive-binary-license-1.0.txt and at https://www.mbed.com/licenses/PBL-1.0')
    print('See the License for the specific language governing permissions and limitations under the License.')

# NOTE: The location of the subprojects directory is calculated relative to
# the location of the script.
def get_subprojects_directory():
  script_name = os.path.basename(__file__)
  scripts_directory = os.path.dirname(os.path.realpath(__file__))
  project_directory = os.path.dirname(scripts_directory)
  subprojects_directory = os.path.join(project_directory, 'subprojects')
  if (not os.path.isdir(subprojects_directory)):
      print(f"ERROR: Subprojects directory not found: {subprojects_directory}", file=sys.stderr)
      print(f"WARNING: {script_name} relies on the script being located in the 'scripts' directory of the Flex SDK.", file=sys.stderr)
      sys.exit(1)
  return subprojects_directory

def make_directory(dirname):
  if (not os.path.isdir(dirname)):
      try:
          os.makedirs(dirname, exist_ok=True)
      except Exception as e:
          print(f"ERROR: Couldn't make directory {dirname}: {e}", file=sys.stderr)
          sys.exit(1)

def download_artifact(filename, output_directory):
    try:
        token = auth.auth()["IdToken"]
        url = download.get_download_url(token, filename)
        output = os.path.join(output_directory, filename)
        download.download_file(url, output)
    except requests.exceptions.HTTPError as e:
        if e.response.status_code >= 500:
            print(f"ERROR: Failed to download {filename}. Received Server Error {e.response.status_code}", file=sys.stderr)
            sys.exit(1)
        elif e.response.status_code >= 400:
            print(f"ERROR: Failed to download {filename}. File does not exist {e.response.status_code}", file=sys.stderr)
            sys.exit(1)
    except requests.exceptions.RequestException as e:
        print(f"ERROR: Failed to download {filename}. Exception {e}", file=sys.stderr)
        sys.exit(1)

def download_artifact_checksum(filename, output_directory):
    download_artifact(filename + '.sha256', output_directory)

@dataclass
class ChecksumEntry:
  checksum: int
  filename: str

def read_checksum_file(filename):
    checksums = []
    # Matches hexadecimal checksum followed by its filename
    checksum_regex = re.compile(r'(^[0-9A-Fa-f]+)\s+(\S.*)$')
    try:
        with open(filename) as file:
            for line in file:
                match = checksum_regex.match(line)
                if match:
                    checksums.append(ChecksumEntry(match.group(1), match.group(2)))
    except Exception as e:
        print(f"ERROR: Failed open {filename}. Exception: {e}.", file=sys.stderr)
        sys.exit(1)
    return checksums

def calculate_checksum(filename):
    result = hashlib.sha256()
    try:
        with open(filename, 'rb') as file:
            while True:
                data = file.read(4096)
                if len(data) == 0:
                    break
                else:
                    result.update(data)
    except Exception as e:
        print(f"ERROR: Failed open {filename}. Exception: {e}.", file=sys.stderr)
        sys.exit(1)
    return result.hexdigest()

def check_checksum(filename, output_directory):
    output_filename = os.path.join(output_directory, filename)
    checksum_filename = output_filename + '.sha256'
    checksum_entries = read_checksum_file(checksum_filename)
    for entry in checksum_entries:
        if filename == entry.filename:
            calculated_checksum = calculate_checksum(output_filename)
            return calculated_checksum == entry.checksum
    print(f"ERROR: Couldn't find checksum for {filename} in {checksum_filename}.", file=sys.stderr)
    sys.exit(1)

def main(argv=None):
    subprojects_directory = get_subprojects_directory()

    system_image = 'system_image-1.5.8-FlexSense.bin'
    system_image_directory = os.path.join(subprojects_directory, 'libflex', 'system_image')
    make_directory(system_image_directory)

    download_artifact(system_image, system_image_directory)
    download_artifact_checksum(system_image, system_image_directory)
    if not check_checksum(system_image, system_image_directory):
        print(f"ERROR: Invalid checksum for {system_image}.", file=sys.stderr)
        sys.exit(1)
    else:
        license_disclaimer(system_image)

    libflex = 'libflex-v1.1.0.a'
    libflex_directory =  os.path.join(subprojects_directory, 'libflex', 'lib')
    make_directory(libflex_directory)

    download_artifact(libflex, libflex_directory)
    download_artifact_checksum(libflex, libflex_directory)
    if not check_checksum(libflex, libflex_directory):
        print(f"ERROR: Invalid checksum for {libflex}.", file=sys.stderr)
        sys.exit(1)
    else:
        license_disclaimer(libflex)

if __name__ == "__main__":
    main()
