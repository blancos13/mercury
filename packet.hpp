#pragma once

string local_host = "127.0.0.1"; //you can also cast with vps and connect
int local_port = 1101;
string meta = "defined";

string server_ip = "";
int server_port = 17091; //No need to change it, there's auto port scraper

bool private_server = true;
int custom_port = 0;

bool ingame = false;

//Features
bool weird = false;
bool invisible = false;
int lastpx = 0;
int lastpy = 0;
//Features


struct gamepacket_ {
    int type;
    int netid;
    float x;
    float y;
    int charstate;
    long long tile;
    float xspeed;
    float yspeed;
    int px;
    int py;
    int netid2;

};

BYTE* tankupdatepacket(ENetPacket* packet)
{
    BYTE* tankupdate = nullptr;
    if (packet->dataLength >= 0x3C)
    {
        BYTE* packetData = packet->data;
        tankupdate = packetData + 4;
        if (*static_cast<BYTE*>(packetData + 16) & 8)
        {
            if (static_cast<int>(packet->dataLength) < *reinterpret_cast<int*>(packetData + 56) + 60) {
                tankupdate = nullptr;
                return tankupdate;
            }
        }
        else
        {
            int zero = 0;
            memcpy(packetData + 56, &zero, 4);
        }
    }
    return tankupdate;
}

void bytetostruct(BYTE* &a, gamepacket_* &b)
{
    memcpy(&b->type, a, 4);
    memcpy(&b->netid, a + 4, 4);
    memcpy(&b->charstate, a + 12, 4);
    memcpy(&b->tile, a + 20, 4);
    memcpy(&b->x, a + 24, 4);
    memcpy(&b->y, a + 28, 4);
    memcpy(&b->xspeed, a + 32, 4);
    memcpy(&b->yspeed, a + 36, 4);
    memcpy(&b->px, a + 44, 4);
    memcpy(&b->py, a + 48, 4);
    return;
}

void structtobyte(BYTE* &a, gamepacket_* &b)
{
    for (auto i = 0; i < 64; i++) a[i] = 0;
    memcpy(a, &b->type, 4);
    memcpy(a + 4, &b->netid, 4);
    memcpy(a + 12, &b->charstate, 4);
    memcpy(a + 20, &b->tile, 4);
    memcpy(a + 24, &b->x, 4);
    memcpy(a + 28, &b->y, 4);
    memcpy(a + 32, &b->xspeed, 4);
    memcpy(a + 36, &b->yspeed, 4);
    memcpy(a + 44, &b->px, 4);
    memcpy(a + 48, &b->py, 4);
    return;
}

BYTE* structtobyte(gamepacket_* b)
{
    const auto a = new BYTE[64];
    for (auto i = 0; i < 64; i++) a[i] = 0;
    memcpy(a, &b->type, 4);
    memcpy(a + 4, &b->netid, 4);
    memcpy(a + 12, &b->charstate, 4);
    memcpy(a + 20, &b->tile, 4);
    memcpy(a + 24, &b->x, 4);
    memcpy(a + 28, &b->y, 4);
    memcpy(a + 32, &b->xspeed, 4);
    memcpy(a + 36, &b->yspeed, 4);
    memcpy(a + 44, &b->px, 4);
    memcpy(a + 48, &b->py, 4);
    return a;
}

//Credit - Growtopia Noobs
void sendbyte(int type, void* packetData, const size_t packetDataSize, void* a4, ENetPeer* peer,ENetHost* host) {
    ENetPacket* p;
    if (peer) {
        if (type == 4 && *(static_cast<BYTE*>(packetData) + 12) & 8) {
            p = enet_packet_create(nullptr, packetDataSize + *(static_cast<DWORD*>(packetData) + 13) + 5, ENET_PACKET_FLAG_RELIABLE);
            auto four = 4;
            memcpy(p->data, &four, 4);
            memcpy(reinterpret_cast<char*>(p->data) + 4, packetData, packetDataSize);
            memcpy(reinterpret_cast<char*>(p->data) + packetDataSize + 4, a4, *(static_cast<DWORD*>(packetData) + 13));
            enet_peer_send(peer, 0, p);
            enet_host_flush(host);
        }
        else {
            try {
                p = enet_packet_create(nullptr, packetDataSize + 5, ENET_PACKET_FLAG_RELIABLE);
                memcpy(p->data, &type, 4);
                memcpy(reinterpret_cast<char*>(p->data) + 4, packetData, packetDataSize);
                enet_peer_send(peer, 0, p);
                enet_host_flush(host);
            }
            catch (...) {
                cout << "Failed to send packet raw (sendpacketraw)" << endl;
            }
        }
    }
    delete static_cast<char*>(packetData);
}

//Credit - <Ama6nen>
struct group { int32_t type; char data; };
void sendchar(bool client,std::string text, int32_t type = 2) {
    auto peer = client ? cpeer : speer;
    auto host = client ? client_ : server_;
    if (!peer || !host) return;

    auto packet = enet_packet_create(0, text.length() + 5, ENET_PACKET_FLAG_RELIABLE);
    auto game_packet = reinterpret_cast<group*>(packet->data);
    game_packet->type = type;
    memcpy(&game_packet->data, text.c_str(), text.length());
    memset(&game_packet->data + text.length(), 0, 1); //cleaning

    enet_peer_send(peer, 0, packet);
    enet_host_flush(host);
}


void replacestr(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return;
    str.replace(start_pos, from.length(), to);
    return;
}

void replacetextptr(std::string& cch, std::string target, std::string replace)
{
    int a = cch.find(target);
    string b = cch.substr(a + target.size());
    int c = b.find("\n");
    string d = b.substr(0, c);
    
    replacestr(cch, target + d, target + replace);

}

#define DO_ONCE            \
    ([]() {                \
        static char o = 0; \
        return !o && ++o;  \
    }())

std::mt19937 rng;
int random(int min, int max) {
    if (DO_ONCE)
        rng.seed((unsigned int)std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(rng);
}

std::string random(uint32_t length) {
    static auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "qwertyuiopasdfghjklzxcvbnm"
            "QWERTYUIOPASDFGHJKLZXCVBNM";
        const uint32_t max_index = (sizeof(charset) - 1);
        return charset[random(INT16_MAX, INT32_MAX) % max_index];
    };

    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

const char hexmap_s[17] = "0123456789abcdef";
std::string hex_str(unsigned char data) {
    std::string s(2, ' ');
    s[0] = hexmap_s[(data & 0xF0) >> 4];
    s[1] = hexmap_s[data & 0x0F];
    return s;
}

std::string generate_rid() {
    std::string rid_str;

    for (int i = 0; i < 16; i++)
        rid_str += hex_str(random(0, 255));

    std::transform(rid_str.begin(), rid_str.end(), rid_str.begin(), ::toupper);

    return rid_str;
}
uint32_t hashstr(uint8_t* str, uint32_t len) {
    if (!str)
        return 0;
    uint8_t* n = (uint8_t*)str;
    uint32_t acc = 0x55555555;
    if (!len)
        while (*n)
            acc = (acc >> 27) + (acc << 5) + *n++;
    else
        for (uint32_t i = 0; i < len; i++)
            acc = (acc >> 27) + (acc << 5) + *n++;
    return acc;
}

std::string generate_mac(const std::string& prefix) {
    std::string x = prefix + ":";
    for (int i = 0; i < 5; i++) {
        x += hex_str(random(0, 255));
        if (i != 4)
            x += ":";
    }
    return x;
}