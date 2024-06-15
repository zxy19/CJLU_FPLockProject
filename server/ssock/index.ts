import { createServer } from "https";
import { readFileSync } from "fs";
import { TLSSocket, Server, TlsOptions } from "tls";
import DataParser from "../dataParser";
import { debug } from "console";
export default class SOCKET {
    server: Server
    connections: Record<number, TLSSocket> = {};
    connectCount = 1;
    events: Record<string, ((from: number, data: Record<string, any>) => void)[]> = {};
    constructor(port: number = 19981, serverConfig?: { ca: any, cert: any, key: any }) {
        let option: TlsOptions = {}
        if (serverConfig) {
            option.ca = serverConfig.ca
            option.cert = serverConfig.cert
            option.key = serverConfig.key
            option.passphrase = "123456987"
        }
        this.connections = {};
        this.connectCount = 1;
        this.server = new Server(option, socket => {
            let conId = this.connectCount++;
            if (this.connectCount > 100000000) this.connectCount = 1;
            this.connections[conId] = socket;
            console.log(`[LOCK][CONNECT]用户已连接，分配ID#${conId}`);
            this.emitEvent("connect", conId, {});
            let parser = new DataParser();
            socket.setEncoding("ascii");
            parser.onMessage((query, data) => {
                this.emitEvent(query, conId, { data });
            });
            socket.on("data", (data: string) => {
                parser.append(data);
            });
            socket.on("close", () => {
                delete this.connections[conId];
                console.log(`[LOCK][CLOSE]用户已断开连接，ID#${conId}`);
                this.emitEvent("close", conId, {})
            })
            socket.on("error", (e) => {
                delete this.connections[conId];
                console.log(`[LOCK][CLOSE]用户连接意外中断，ID#${conId}`);
                this.emitEvent("close", conId, {})
            })
        });
        this.server.listen(port);
        this.server.on("tlsClientError", e => {
            console.log(e);
        })
    }
    private emitEvent(type: string, from: number, data: Record<string, any>) {
        if (!this.events[type]) return;
        this.events[type].forEach((cb) => {
            cb(from, data);
        })
    }
    on(type: string, cb: (from: number, data: Record<string, any>) => void) {
        if (!this.events[type]) this.events[type] = [];
        this.events[type].push(cb);
    }
    send(conId: number, data: { type: string, data: string } | String) {
        if (!this.connections[conId]) return;
        let toSendData: string;
        if (typeof data === "string") {
            toSendData = data;
        } else {
            toSendData = `DATA${(data as any).type}\n${(data as any).data}\n`
        }
        this.connections[conId].write(toSendData);
    }
    close(conId: number) {
        if (!this.connections[conId]) return;
        this.connections[conId].destroy();
    }
}
