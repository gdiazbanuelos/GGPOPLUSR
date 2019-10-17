#!/usr/bin/env python3
from xml.etree import ElementTree


def main():
    tree = ElementTree.parse('GGXXACPR_Win.CT')
    root = tree.getroot()

    for cheat_entry in root.findall('.//CheatEntry'):
        last_state = cheat_entry.find('LastState')
        if last_state is not None:
            cheat_entry.remove(last_state)

    tree.write('GGXXACPR_Win.CT', xml_declaration=True, encoding='utf-8')


if __name__ == '__main__':
    main()
