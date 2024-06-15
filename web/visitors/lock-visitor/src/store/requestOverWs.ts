import { SERVER_URL } from '@/config/config';
import { Events } from '@/utils/events';
import { defineStore } from 'pinia'
import { ref } from 'vue';

export const requestOverWs = defineStore('ROW', () => {

    let ws: WebSocket | null = null;
    let pingInterval: number | null = null;
    const events = new Events<{ [key: string]: any }>();
    let globalVc = 0;
    let replyHandlers: Record<string, (data: any) => void> = {};
    let globalRid = 0;

    const createWs = () => {
        const localVc = ++globalVc;
        if (ws) {
            ws.close();
            ws = null;
            if (pingInterval) {
                clearInterval(pingInterval);
            }
        }
        ws = new WebSocket(SERVER_URL);
        ws.onclose = (ev) => {
            if (localVc == globalVc)
                events.emit("close", ev);
        }
        ws.onmessage = (ev) => {
            if (localVc != globalVc) return;
            let data = JSON.parse(ev.data.toString());
            if (data.type == "reply") {

            } else {
                events.emit(data.type, data);
            }
        }
        ws.onopen = () => {
            if (localVc == globalVc)
                events.emit("open", {});
        }
        ws.onerror = (ev) => {
            if (localVc == globalVc)
                events.emit("error", ev);
        }
        pingInterval = setInterval(() => send({ type: "ping" }), 10000);
    }
    const query = (data: Record<string, string>): Promise<Record<string, string>> => {
        return new Promise((resolve, reject) => {
            const id = ++globalRid;
            data['reply'] = id.toString();
            replyHandlers[id] = res => {
                delete replyHandlers[id];
                resolve(res);
            }
            setTimeout(() => {
                delete replyHandlers[id];
                reject({ msg: 'timeout' });
            }, 10000);
            send(data);
        });
    }
    const send = (data: Record<string, string>): void => {
        if (ws) {
            ws.send(JSON.stringify(data));
        }
    }

    return {
        send,query,createWs,events
    }
})