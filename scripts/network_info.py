#!/usr/bin/env python3

import os
import argparse
import requests

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output')
    parser.add_argument('--cold', action='store_true',
                        help='Download cold start network information.')
    args = parser.parse_args()

    # Select from warm start or cold start network information
    cold = args.cold
    url = "https://static.myriota.com/n2/ModuleNetworkConfigWarmStart"
    if cold == True:
        url = "https://static.myriota.com/n2/ModuleNetworkConfigColdStart"

    output = args.output
    response = requests.get(url, allow_redirects=True)
    with open(output, 'wb') as file:
        file.write(response.content)

if __name__ == "__main__":
    main()
