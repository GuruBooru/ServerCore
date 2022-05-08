from typing import List


class ProtoParser():
    def __init__(self, start_id, recv_prefix, send_prefix):
        self.recv_packet = [];
        self.send_packet = [];
        self.total_packet = [];
        self.start_id = start_id
        self.id = start_id
        self.recv_prefix = recv_prefix
        self.send_prefix = send_prefix

    def parse_proto(self, path):
        f = open(path, 'r')
        lines = f.readlines()

        for line in lines:
            if line.startswith('message') == False:
                continue

            packet_name = line.split()[1].upper()

            if packet_name.startswith(self.recv_prefix):
                self.recv_packet.append(Packet(packet_name, self.id))
            elif packet_name.startswith(self.send_prefix):
                self.send_packet.append(Packet(packet_name, self.id))
            else:
                continue

            self.total_packet.append(Packet(packet_name, self.id))
            self.id += 1

        f.close()


class Packet:
    def __init__(self, name, id):
        self.name = name
        self.id = id