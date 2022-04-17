#pragma once
bool connect() {
    cout << "[Client] Connecting to the server\n";
    ENetAddress address;
    enet_address_set_host(&address, server_ip.c_str());
    address.port = server_port;
    
    server_ = enet_host_create(0, 1, 2, 0, 0);
    server_->usingNewPacket = (private_server ? false : true);
    if (!server_) {
        cout << "[Client] Error - Couldn't start ENet server (connect)" << endl;
        system("PAUSE");
        return false;
    }
    server_->checksum = enet_crc32;
    auto code = enet_host_compress_with_range_coder(server_);
    if (code != 0) {
        cout << "[Client] Error - ENet Host compress (connect)" << endl;
        system("PAUSE");
        return false;
    }
    enet_host_flush(server_);

    speer = enet_host_connect(server_, &address, 2, 0);
    if (!server_) {
        cout << "[Client] Failed to connect server\n";
        return false;
    }
    return true;
}

bool command(std::string cmd)
{
    if (cmd == "weird")
    {
        weird = (weird ? false : true);
        string a = (weird ? "`2Weird state has been enabled. You can't see but others do.``" : "`4Weird state has been disabled.``");
        sendchar(true, "action|log\nmsg|" + a, 3);
        return true;
    }
    else if (cmd == "invisible")
    {
        invisible = (invisible ? false : true);
        string a = (invisible ? "`1Invisible Ninja mode enabled, now others cant see you.``" : "`4Invisible mode has been disabled.``");
        sendchar(true, "action|log\nmsg|" + a, 3);
        auto gamepacket = new gamepacket_;
        gamepacket->type = 0;
        sendbyte(4, structtobyte(gamepacket), 56, nullptr, speer, server_);
        delete gamepacket;
        return true;
    }
    else if (cmd.find("wear ") != string::npos)
    {
        int tile = atoi(cmd.substr(5).c_str());
        auto gamepacket = new gamepacket_;
        gamepacket->type = 10;
        gamepacket->tile = tile;
        sendbyte(4, structtobyte(gamepacket), 56, nullptr, speer, server_);
        delete gamepacket;
        sendchar(true, "action|log\nmsg|`2Sent wear packet to id " + to_string(tile) + ".``", 3);
        return true;
    }
    else if (cmd.find("crash ") != string::npos)
    {
        int q = atoi(cmd.substr(6).c_str());
        for(int i = 0;i < q;i++) sendchar(false, "action|join_request\nname|START\ninvitedWorld|0", 3);
        sendchar(true, "action|log\nmsg|`1Sent lots of packets.``", 3);
        return true;
    }
    else if (cmd == "safe")
    {
        if (lastpx == 0 || lastpy == 0)
        {
            sendchar(true, "action|log\nmsg|`4Punch to safe vault before using this.``", 3);
            return true;
        }
        sendchar(true, "action|log\nmsg|`o`4NOTE:`` You need to punch safe vault 3 times to save coordinates.``", 3);
        sendchar(false, "action|dialog_return\ndialog_name|safe_withdrawConfirm\nsafewithdraw_" + std::to_string(lastpy) + std::to_string(lastpx) + "_1|1");
        sendchar(true, "action|log\nmsg|`2Sent safe vault packet.``", 3);
        return true;
    }
    else if (cmd.find("editsign ") != string::npos)
    {
        sendchar(true, "action|log\nmsg|`o`4NOTE:`` You need to save coordinates first (Only GTPS Developers can understand it).``", 3);
        sendchar(false, "action|dialog_return\ndialog_name|editsign\nsigntextas|" + cmd.substr(9));
        sendchar(true, "action|log\nmsg|`2Sent sign packet.``", 3);
        return true;
    }
    else if (cmd.find("warp ") != string::npos)
    {
        string a = cmd.substr(5);
        std::transform(a.begin(), a.end(), a.begin(), ::toupper);
        sendchar(false, "action|join_request\nname|" + a + "\ninvitedWorld|0",3);
        sendchar(true, "action|log\nmsg|`1Warping to " + a + "...``", 3);
        return true;
    }
    else if (cmd == "mag")
    {
        sendchar(true, "action|log\nmsg|`o`4NOTE:`` You need to wrench magplant to save coordinates.``", 3);
        sendchar(false, "action|dialog_return\ndialog_name|magplantupdate\ncheckbox_gemcollection|0");
        sendchar(true, "action|log\nmsg|`2Sent magplant packet.``", 3);
        return true;
    }
    else if (cmd == "proxy")
    {
        sendchar(true, "action|log\nmsg|`oCommands : /register,/mag,/warp <WORLD>,/editsign <TEXT>,/safe,/wear <ID>,/weird,/invisible,/crash <QUANTITY>``", 3);
        return true;
    }
    else if (cmd == "register")
    {
        string a = std::to_string(random(1, 9000));
        sendchar(false, "action|dialog_return\ndialog_name|register\nusername|viam" + a + "\npassword|viam\npassword_verify|viam\nemail|cop@server.com");
        sendchar(true, "action|log\nmsg|`2Sent register packet.``", 3);
        return true;

    }
    
    return false;
}
void clientevent_() {
    
    ENetEvent event;
    while (enet_host_service(client_, &event, 0) > 0) {
        cpeer = event.peer;
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
        {
            cout << "[Client] New Connection\n";
            if(!connect()) return;
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            int type = 0;
            if (event.packet->dataLength > 3u) type = *(event.packet->data);
            cout << "[Client] Packet Type: " << type << endl;
            switch (type)
            {
            case 2:
            { //text pointer
                char zero = 0;
                memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);
                char* textptr = (char*)(event.packet->data + 4);
                string txt = textptr;
                
                if (txt.find("requestedName|") != string::npos)
                {
                  
                    auto mac = generate_mac("00");
                    auto wk = generate_rid();
                    auto rid = generate_rid();
                    std::string fz = std::to_string(12400976 - random(1, 700));
                    std::string zf = std::to_string(1089975202 - random(1, 700));
                    std::string hash1 = std::to_string(-1557517681 + random(1, 700));
                    string hash2 = std::to_string(-716928004 + random(1,700));

                    replacetextptr(txt, "country|", "tr");
                    replacetextptr(txt, "game_version|", "3.84");
                    replacetextptr(txt, "mac|", mac);
                    replacetextptr(txt, "wk|", wk);
                    replacetextptr(txt, "rid|", rid);
                    replacetextptr(txt, "fz|", fz);
                    replacetextptr(txt, "zf|", zf);
                    replacetextptr(txt, "hash|", hash1);
                    replacetextptr(txt, "fhash|", hash2);
                    replacetextptr(txt, "platformID|", "0");
                    replacetextptr(txt, "deviceVersion|", "0");
                   
                    sendchar(false, txt);
                    sendchar(true, "action|log\nmsg|`1Spoofed login credentials.``", 3);
                    cout << "[Spoofed] " << txt << endl;
                    return;
                }
                cout << "[Client] - TextPacket 2: " << txt << endl;
                
                if (txt.find("action|input\n|text|/") != string::npos) {
                    string cmd = txt.substr(20);
                    if(command(cmd)) return;
                }
                //enet_packet_destroy(event.packet);
                break;
            }
            case 3:
            { //game packet
                char zero = 0;
                memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);
                char* textptr = (char*)(event.packet->data + 4);
                string txt = textptr;
                cout << "[Client] - TextPacket 3: " << txt << endl;
                if (txt.find("action|quit\n") != string::npos)
                {
                    ingame = false;
                    if (server_) server_ = nullptr;
                    if (speer) speer = nullptr;
                    return;
                }

                break;
            }
            case 4:
            {
                BYTE* tankupdate = tankupdatepacket(event.packet);
                if (!tankupdate) break;
                auto gamepacket = new gamepacket_;
                bytetostruct(tankupdate, gamepacket);
                

                if (gamepacket->tile == 18)
                {

                    lastpx = gamepacket->px;
                    lastpy = gamepacket->py;
                }

                switch (gamepacket->type)
                {
                case 0:
                {
                    if (weird) gamepacket->charstate = 0x10;
                    if (invisible)
                    {
                        gamepacket->x = -1;
                        gamepacket->y = -1;
                    }
                    sendbyte(4, structtobyte(gamepacket), 56, nullptr, speer, server_);
                    delete gamepacket;
                    return;
                }
                case 21:
                {   //ping request
                    break;
                }
                default:break;
                }
                delete gamepacket;
                break;
            }
            case 6: break;
            case 8: return;
            default: break;
            }

            if (!server_ || !speer) return;
            enet_peer_send(speer, 0, event.packet);
            enet_host_flush(server_);
            break;
        }
        default: break;
        }
    }
    return;
}