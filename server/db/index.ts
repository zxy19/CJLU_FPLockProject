import * as mysql from "mysql";
import config from "../config"
var connection = mysql.createConnection(config.mysql);
connection.connect();
export function query(q: string, p?: any[]): Promise<any> {
  return new Promise(function (s, e) {
    connection.query(q, p, function (error, results) {
      if (error) e(error);
      else s(results);
    });
  });
}