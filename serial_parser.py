#! /usr/bin/env python




def parser(read_byte_func):
    packet = []
    packet_start = False
    half_byte = None

    def packet_is_complete():
        if not packet_start or half_byte != None:
            return False
        l = len(packet)
        if l < 5:
            return False
        if packet[0] == 0xff:
            expected_l = 5
        elif packet[0] < 0xfc:
            expected_l = packet[0] * 2 + 5
        else:
            raise Exception("0x%02x is an invalid packet type" % (packet[0]))
        if l < expected_l:
            return False
        if l == expected_l:
            return True
        raise Exception("packet type 0x%02x dictates a %u-byte packet, but "
                        "packet is %u bytes long" % (packet[0], expected_l, l))

    while True:
        if packet_is_complete():
            yield packet
            packet = []
            packet_start = False
        # packet is not yet complete
        b = read_byte_func()
        if len(b) == 0:
            raise StopIteration()
        b = ord(b)
        if b == 0xfe:
            if packet_start:
                raise Exception("restart")
            packet = []
            packet_start = True
            half_byte = None
            continue
        if not packet_start:
            continue
        if b == 0xfc or b == 0xfd:
            if half_byte == None:
                half_byte = b
                continue
            full_byte = ((half_byte - 0xfc) << 1) + b
            if full_byte == 0xff:
                raise Exception('fdfd is currently an illegal byte pair')
            packet.append(full_byte)
            half_byte = None
            continue
        if half_byte != None:
            raise Exception("expected either 0xfc or 0fd")
        packet.append(b)


if __name__ == '__main__':
    import sys

    packet_iter = parser(lambda : sys.stdin.read(1))
    for packet in packet_iter:
        print packet
