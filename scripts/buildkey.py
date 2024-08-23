#!/usr/bin/env python3

import time
import secrets
import argparse

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output_filename')
    args = parser.parse_args()

    timestamp = int(time.time())
    timestamp = 1715129253
    timestamp_bytes = timestamp.to_bytes(4, 'big')

    myriota_sdk_version = '2.0.1-FlexSense'
    version_core = myriota_sdk_version.split('-')[0]
    version_core_components = list(map(int, version_core.split('.')))

    buildkey = []
    buildkey[0:4] = timestamp_bytes[0:4]
    buildkey[4:3] = version_core_components[0:3]
    buildkey[7:9] = secrets.token_bytes(9)
    buildkey[16:] = [0] * 16

    to_hex_with_no_0x = lambda x: '{0:02x}'.format(x)
    buildkey_hex = ''.join(map(to_hex_with_no_0x, buildkey))

    with open(args.output_filename, 'w') as file:
        file.write('0: {0}'.format(buildkey_hex))

if __name__ == "__main__":
    main()
