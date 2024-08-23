#!/usr/bin/env python3

import os
import argparse
import requests

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output')
    args = parser.parse_args()

    output = args.output
    url =  "https://static.myriota.com/n2/ModuleNetworkConfig"
    response = requests.get(url, allow_redirects=True)
    with open(output, 'wb') as file:
        file.write(response.content)

if __name__ == "__main__":
    main()
