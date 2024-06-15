import { readFileSync } from 'fs';
import WSS from './wss';
import SOCKET from './ssock';
import config from "./config"
import { query } from './db';
import { randomUUID, createHash } from 'crypto';
let cfg: { cert: any, key: any } | undefined = undefined;
try {
    cfg = {
        "cert": readFileSync("./ssl/fullchain.pem"),
        "key": readFileSync("./ssl/privkey.pem")
    };
} catch (e) { }
const wss = new WSS(config.port, cfg);

let lockCfg: { cert: any, key: any, ca: any } | undefined = undefined;
try {
    lockCfg = {
        "cert": readFileSync("./lock/lock.crt"),
        "key": readFileSync("./lock/lock.key"),
        "ca": readFileSync("./lock/ca.crt")
    };
} catch (e) { }
const lockSock = new SOCKET(config.lockPort, lockCfg);

let lockUser2con: Record<string, number> = {};
let lockCon2user: Record<number, string> = {};
let lockCon2pass: Record<number, string> = {};
let lockCon2lock: Record<number, string> = {};
let lock2lockCon: Record<string, number> = {};
let lockCon2userIds: Record<number, number[]> = {};

let isAuth: Record<number, boolean> = {};
let premission: Record<number, number> = {};
let user: Record<number, string> = {};
let userId: Record<number, number> = {};
let lockId: Record<number, string> = {};
let failReason: Record<number, string> = {};


function boardcastLock(lockConId: number, data: string) {
    lockCon2userIds[lockConId].forEach((id: number) => {
        wss.send(id, data);
    });
}

wss.on("connect", (conId, data) => {
    isAuth[conId] = false;
});

wss.on("auth", async (conId, data) => {
    try {
        //用户登录
        if (data.auth == "user") {
            const username: string = data.username;
            const password: string = data.password;

            if (!username || !password) {
                throw new Error("用户名或密码为空");
            }
            if (!lockUser2con[username]) {
                const id = lockUser2con[username];
                if (lockCon2pass[id] == password) {
                    isAuth[conId] = true;
                    user[conId] = username;
                    userId[conId] = -2;
                    premission[conId] = 10;
                    lockId[conId] = lockCon2lock[id];
                    if (!lockCon2userIds[id])
                        lockCon2userIds[id] = [];
                    lockCon2userIds[id].push(conId);
                    wss.send(conId, { reply: data.reply, type: "auth", status: isAuth[conId] });
                    return;
                }
            }
            const res = await query("SELECT * FROM `user` WHERE `username` = ? AND `password` = ?", [username, password]);
            if (res.length == 0)
                throw new Error("用户名或密码错误");
            isAuth[conId] = true;
            user[conId] = res[0].username;
            userId[conId] = res[0].id;
            premission[conId] = res[0].premission;
            lockId[conId] = res[0].lock;
            if (!lock2lockCon[lockId[conId]]) {
                throw new Error("锁不在线");
            }
            if (!lockCon2userIds[lock2lockCon[lockId[conId]]])
                lockCon2userIds[lock2lockCon[lockId[conId]]] = [];
            lockCon2userIds[lock2lockCon[lockId[conId]]].push(conId);
        } else if (data.auth == "token") {
            const token: string = data.token;
            const res = await query("SELECT * FROM `token` WHERE `token` = ?", [token]);
            if (res.length == 0) {
                throw new Error("token不存在或过期");
            }
            if (res.expire < new Date().getTime() || res.usage <= 0) {
                throw new Error("token已过期");
            }
            isAuth[conId] = true;
            user[conId] = "";
            userId[conId] = -1;
            premission[conId] = 0;
            lockId[conId] = res[0].lock;
            if (!lock2lockCon[lockId[conId]]) {
                throw new Error("锁不在线");
            }
            if (!lockCon2userIds[lock2lockCon[lockId[conId]]])
                lockCon2userIds[lock2lockCon[lockId[conId]]] = [];
            lockCon2userIds[lock2lockCon[lockId[conId]]].push(conId);
        }
        wss.send(conId, { type: "reply", toType: "auth", reply: data.reply, status: isAuth[conId] });
    } catch (e: any) {
        wss.send(conId, { type: "reply", toType: "auth", reply: data.reply, status: isAuth[conId], msg: e.toString() });;
    }
});
wss.on("getUsers", async (conId, data) => {
    if (isAuth[conId] && premission[conId] >= 10) {
        try {
            const res = await query("SELECT * FROM `user` WHERE `lock` = ?", [lockId[conId]]);
            const table = res.map((r: any) => {
                return {
                    id: r.id,
                    username: r.username
                }
            });
            wss.send(conId, { type: "reply", toType: "getUsers", reply: data.reply, status: true, data: table });
        } catch (e) {
            wss.send(conId, { type: "reply", toType: "getUsers", reply: data.reply, status: false });
        }
    }
});
wss.on("getTokens", async (conId, data) => {
    if (premission[conId] >= 5) {
        try {
            const res = await query("SELECT * FROM `token` WHERE `lock` = ?", [lockId[conId]]);
            const table = res.map((r: any) => {
                return {
                    id: r.id,
                    token: r.token,
                    name: r.name
                }
            });
            wss.send(conId, { type: "reply", toType: "getTokens", reply: data.reply, status: true, data: table });
        } catch (e) {
            wss.send(conId, { type: "reply", toType: "getTokens", reply: data.reply, status: false });
        }
    }
})

wss.on("addUser", async (conId, data) => {
    if (premission[conId] >= 10) {
        try {
            const username: string = data.username;
            const password: string = data.password;
            const lock: string = lockId[conId];
            const res = await query('INSERT INTO users (`username`, `password`,`permission`,`lock`) VALUES (?,?,5,?)', [username, password, lock]);
            wss.send(conId, { type: "reply", reply: data.reply, toType: "addUser", status: 1 });
        } catch (e) {
            wss.send(conId, { reply: data.reply, toType: "addUser", type: "reply", status: 0, msg: e.toString() });
        }
    }
})
wss.on("addToken", async (conId, data) => {
    if (premission[conId] >= 5) {
        try {
            const name: string = data.name;
            const usage: string = data.usage;
            const lock: string = lockId[conId];
            const hash = createHash("md5");
            hash.push(name);
            hash.push(lock);
            hash.push(randomUUID());
            const token: string = hash.digest().toString("hex");
            const res = await query('INSERT INTO token (`name`,`token`,`usage`,`lock`) VALUES (?,?,?,?)', [name, token, usage, lock]);
            wss.send(conId, { type: "reply", reply: data.reply, toType: "addUser", status: 1 });
        } catch (e) {
            wss.send(conId, { reply: data.reply, toType: "addToken", type: "reply", status: 0, msg: e.toString() });
        }
    }
})


wss.on("open", (conId, data) => {
    if (isAuth[conId]) {
        if (lockId[conId]) {
            if (lock2lockCon[lockId[conId]]) {
                lockSock.send(lock2lockCon[lockId[conId]], { type: "open", data: "" });
                wss.send(conId, { type: "reply", toType: "openRequest", reply: data.reply, status: true });
                return;
            }
        }
    }
    wss.send(conId, { type: "reply", reply: data.reply, toType: "openRequest", status: false });
});
wss.on("close", (conId, data) => {
    if (isAuth[conId]) delete isAuth[conId];
    if (premission[conId]) delete premission[conId];
    if (user[conId]) delete user[conId];
    if (userId[conId]) delete userId[conId];
    if (lockId[conId]) delete lockId[conId];
});

lockSock.on("close", (conId, data) => {
    const user = lockCon2user[conId];
    if (user && lockUser2con[user]) delete lockUser2con[user];
    const lock = lockCon2lock[conId];
    if (lock && lock2lockCon[lock]) delete lock2lockCon[lock];
    if (lockCon2lock[conId]) delete lockCon2lock[conId];
    if (lockCon2user[conId]) delete lockCon2user[conId];
    if (lockCon2pass[conId]) delete lockCon2pass[conId];
});
lockSock.on("reg", (conId, data) => {
    const [lockId, username, password] = data.data.split("|");
    lockCon2lock[conId] = lockId;
    lockCon2pass[conId] = password;
    lockCon2user[conId] = username;
    lockUser2con[username] = conId;
    lock2lockCon[lockId] = conId;
    console.log(`[LOCK][REG]用户${username};${password}注册锁${lockId}`);
});

lockSock.on("door", (conId, data) => {
    boardcastLock(conId, data.data);
})