import * as fs from "fs";
const obj: {
    port: number,
    lockPort: number,
    mysql: {
        host: string,
        user: string,
        password: string,
        database: string
    }
} = JSON.parse(fs.readFileSync(`./config.json`, 'utf-8'));
export default obj;