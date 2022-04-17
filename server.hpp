#pragma once

void serverevent_() {
    ENetEvent event;
    while (enet_host_service(server_, &event, 0) > 0) {
        speer = event.peer;
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:
        {
            cout << "[Server] Connection accepted\n";
            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            int type = 0;
            if (event.packet->dataLength > 3u) type = *(event.packet->data);
            char zero = 0;
            
            switch (type)
            {
            case 2:
            { //text pointer
                break;
            }
            case 3:
            { //game packet
                char zero = 0;
                memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);
                char* textptr = (char*)(event.packet->data + 4);
                cout << "[Server] - TextPacket 3: " << textptr << endl;
                break;
            }
            case 4:
            {
                BYTE* tankupdate = tankupdatepacket(event.packet);
                if (!tankupdate) break;
                auto gamepacket = new gamepacket_;
                bytetostruct(tankupdate, gamepacket);
                switch (gamepacket->type)
                {
                case 21:
                {   //ping request
                    cout << "Ping Request\n";
                    break;
                }
                default:break;
                }
                delete gamepacket;
                break;
            }
            case 6: 
            {
                char zero = 0;
                memcpy(event.packet->data + event.packet->dataLength - 1, &zero, 1);
                char* textptr = (char*)(event.packet->data + 4);
                string cch = textptr;
                cout << "Tracked : " + cch << endl;
                enet_packet_destroy(event.packet);
                return;
            }break;
            case 7: return;
            default: break;
            }

            if (!cpeer || !client_) return;
            enet_peer_send(cpeer, 0, event.packet);
            enet_host_flush(client_);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            ingame = false;
            if (server_) server_ = nullptr;
            if (speer) speer = nullptr;
            return;
        }
        default: break;
        }
    }
    return;
}
