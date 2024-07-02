#!/usr/bin/env python3

import os
import argparse
import requests

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('output_directory')
    args = parser.parse_args()

    output_directory = args.output_directory
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)
    output = os.path.join(output_directory, 'network_info')

    satellites = "DefaultNetworkInfo"
    access_nodes_baseurl = "https://static.myriota.com/access_nodes"
    access_node_revision = "r10"
    url = "{0}/{1}/{2}".format(
      access_nodes_baseurl,
      access_node_revision,
      satellites)

    response = requests.get(url, allow_redirects=True)
    with open(output, 'wb') as file:
        file.write(response.content)

if __name__ == "__main__":
    main()
