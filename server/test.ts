import { connect } from "tls";
import { readFileSync } from 'fs';
let a = connect(19192, "127.0.0.1", {
    ca: readFileSync("./lock/ca.crt"),
    cert: readFileSync("./lock/lock.crt"),
    key: readFileSync("./lock/lock.key"),
    rejectUnauthorized: false
}, () => {
    console.log("OK");
    debugger;
})