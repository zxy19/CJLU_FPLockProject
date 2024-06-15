import { WebSocketServer } from "ws";
import { createServer } from "https";
import { readFileSync } from "fs";
export default class WSS {
    wss: WebSocketServer
    connections: Record<number, any> = {};
    connectCount = 1;
    events: Record<string, ((from: number, data: Record<string, any>) => void)[]> = {};
    constructor(port: number = 19981, serverConfig?: { cert: any, key: any }) {
        if (serverConfig) {
            const server = createServer(serverConfig).listen(port);
            this.wss = new WebSocketServer({ server });
        } else {
            this.wss = new WebSocketServer({
                port: port
            })
        }
        this.connections = {};
        this.connectCount = 1;

        this.wss.on("connection", (ws) => {
            let conId = this.connectCount++;
            if (this.connectCount > 100000000) this.connectCount = 1;
            this.connections[conId] = ws;
            console.log(`[CONNECT]用户已连接，分配ID#${conId}`);
            this.emitEvent("connect", conId, {})
            ws.on("message", (data) => this.wsMsg(data.toString(), conId));
            ws.on("close", () => {
                delete this.connections[conId];
                console.log(`[CLOSE]用户已断开连接，ID#${conId}`);
                this.emitEvent("close", conId, {})
            })
        });
    }
    private emitEvent(type: string, from: number, data: Record<string, any>) {
        if (!this.events[type]) return;
        this.events[type].forEach((cb) => {
            cb(from, data);
        })
    }
    wsMsg(data: string, conId: number) {
        try {
            var dat = JSON.parse(data);
            if (dat.type == "ping") {
                this.send(conId, { type: "pong", tick: dat.tick });
                return;
            }
        } catch (e) {
            return;
        }
        this.emitEvent(dat.type, conId, dat);
    }
    on(type: string, cb: (from: number, data: Record<string, any>) => void) {
        if (!this.events[type]) this.events[type] = [];
        this.events[type].push(cb);
    }
    send(conId: number, data: Record<string, any> | String) {
        if (!this.connections[conId]) return;
        let toSendData: string;
        if (typeof data == "string") {
            toSendData = data;
        } else {
            toSendData = JSON.stringify(data);
        }
        this.connections[conId].send(toSendData);
    }
    close(conId: number) {
        if (!this.connections[conId]) return;
        this.connections[conId].close();
    }
}    
