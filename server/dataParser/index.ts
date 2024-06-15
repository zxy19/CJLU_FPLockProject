export default class DataParser {
    buffer: string;
    query: string;
    cb: ((query: string, data: string) => void)[];
    constructor() {
        this.buffer = "";
        this.query = "";
        this.buffer = "";
        this.cb = [];
    }
    onMessage(cb: (query: string, data: string) => void) {
        this.cb.push(cb);
    }
    append(c: string) {
        let temp = c.replace(/\r/ig, "").split("\n");
        if (!temp.length) return;
        temp[0] = temp[0] + this.buffer;
        this.buffer = temp.pop()!;
        temp.forEach(v => {
            if (v.startsWith("DATA")) {
                this.query = v.substring(4);
            } else if (this.query) {
                this.cb.forEach(_v => _v(this.query, v));
            }
        });
    }
}